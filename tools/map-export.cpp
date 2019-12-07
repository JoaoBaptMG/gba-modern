#include "pch.hpp"
#include "tinyxml2.h"
#include "actors.hpp"

struct TmxTileset
{
    std::string sourceName;
    std::size_t firstgid;
};

void checkError(tinyxml2::XMLDocument& doc)
{
    if (doc.Error()) throw std::domain_error(doc.ErrorStr());
}

static std::string getFolderName(const std::string& file)
{
    auto pos = file.find_last_of('/');
    if (pos == std::string::npos) return "";
    return file.substr(0, pos);
}

static std::string getBaseName(const std::string& file)
{
    // This works because npos is equal to (unsigned)-1,
    // so npos+1 will wrap back to 0
    return file.substr(file.find_last_of('/')+1);
}

std::vector<std::uint32_t> getTiles(tinyxml2::XMLHandle map, const std::map<std::uint32_t, std::uint32_t> &tileEquivalences);

struct Actor
{
    std::string type;
    std::vector<std::string> insertedParams;
};

struct ActorReturn final
{
    std::vector<Actor> actors;
    std::set<std::string> occurences;
};

ActorReturn getActorsFromMap(tinyxml2::XMLHandle map, const std::map<std::string, ActorType>& actorTypes);

int mapExport(int argc, char **argv)
{
    using namespace tinyxml2;

    if (argc < 6) throw std::out_of_range("map-export expects 4 arguments!");

    // Load the TMX file
    XMLDocument doc;
    doc.LoadFile(argv[2]);
    checkError(doc);
    XMLHandle map(doc.FirstChildElement("map"));
    checkError(doc);

    // Get its main properties
    std::size_t width = map.ToElement()->UnsignedAttribute("width");
    std::size_t height = map.ToElement()->UnsignedAttribute("height");
    checkError(doc);

    // Some range checks
    if (width > 65535) throw std::out_of_range("Map width must be at most 65535!");
    if (height > 65535) throw std::out_of_range("Map height must be at most 65535!");

    // Get its tilesets
    std::vector<TmxTileset> tilesets;
    for (auto elm = map.FirstChildElement("tileset"); elm.ToElement(); elm = elm.NextSiblingElement("tileset"))
    {
        auto tset = elm.ToElement();
        TmxTileset tmx;
        tmx.sourceName = tset->Attribute("source");
        tmx.firstgid = tset->UnsignedAttribute("firstgid");
        checkError(doc);
        tilesets.push_back(std::move(tmx));
    }

    // Some sanity checks
    if (tilesets.empty()) throw std::domain_error("The tileset collection cannot be empty");
    auto tilesetFolder = getFolderName(tilesets[0].sourceName);
    for (const auto& ts : tilesets)
        if (getFolderName(ts.sourceName) != tilesetFolder)
            throw std::domain_error("All tileset folders name must match!");
    
    // Get the number of autotiles (and also assert the file exists)
    std::size_t numAutotiles = 0;
    {
        std::ifstream inj;
        inj.exceptions(std::istream::badbit | std::istream::failbit);
        auto filename = getFolderName(argv[2]) + "/../data/" + tilesetFolder + ".png.json";
        inj.open(simplifyPath(filename));
        nlohmann::json j;
        inj >> j;

        if (j.contains("num-autotiles")) j.at("num-autotiles").get_to(numAutotiles);
    }

    // Build the tile equivalences
    std::map<std::uint32_t, std::uint32_t> tileEquivalences;
    tileEquivalences[0] = 0;
    for (const auto& ts : tilesets)
    {
        std::uint32_t equiv = 0;
        if (ts.sourceName == tilesetFolder + "/tileset.tsx")
            equiv = 1 + 47 * numAutotiles;
        else for (std::size_t i = 0; i < numAutotiles; i++)
            if (ts.sourceName == tilesetFolder + "/autotile" + std::to_string(i) + ".tsx")
            {
                equiv = 1 + 47 * i;
                break;
            }
        tileEquivalences[ts.firstgid] = equiv;
    }

    // Get the tiles array
    auto tiles = getTiles(map, tileEquivalences);

    // Trim the array to the correct size
    tiles.resize(width*height, 0);
    
    // Get the actor types
    auto actorTypes = getActorTypes(simplifyPath(getFolderName(argv[2]) + "/../data/actors.json"));

    // And now, get the actors
    auto [actors, occurences] = getActorsFromMap(map, actorTypes);

    // Now, we should start create the maps
    auto name = deriveSpecialName(argv[3]);
    auto tsname = deriveSpecialName("data/" + tilesetFolder).mangledName;

    // Write the data
    {
        std::string out = argv[3];
        std::ofstream of;
        of.exceptions(std::ofstream::failbit | std::ofstream::badbit);
        of.open(out);

        of << "@ " << std::endl;
        of << "@ " << out << std::endl;
        of << "@ " << std::endl << std::endl;

        of << "    .section .rodata" << std::endl;
        of << "    .align 2" << std::endl;
        of << "    .global " << name.mangledName << std::endl;
        of << "    .hidden " << name.mangledName << std::endl;
        of << name.mangledName << ":" << std::endl;

        // Write the metadata
        of << "    .hword " << width << ", " << height << std::endl;
        of << "    .word " << tsname << std::endl;
        of << "    .word map_" << name.fileName << "_data, map_" << name.fileName << "_init" << std::endl << std::endl;

        // Write the main data
        of << "    .section .rodata" << std::endl;
        of << "    .align 2" << std::endl;
        of << "    .hidden map_" << name.fileName << "_data" << std::endl;
        of << "map_" << name.fileName << "_data:";

        std::size_t index = 0;
        for (auto tile : tiles)
        {
            if (index % 16 == 0) of << std::endl << "    .byte ";
            else of << ", ";
            of << toHex(tile, 2);
            index++;
        }

        of << std::endl << std::endl;
    }

    // Write the initialization code
    {
        std::string outc = argv[4];
        std::ofstream cof;
        cof.exceptions(std::ofstream::failbit | std::ofstream::badbit);
        cof.open(outc);

        cof << "// " << std::endl;
        cof << "// " << outc << std::endl;
        cof << "// " << std::endl << std::endl;
        cof << "#include \"gameplay/GameScene.hpp\"" << std::endl;

        for (const auto& type : occurences)
            cof << "#include \"" << actorTypes.at(type).includeName << "\"" << std::endl;

        cof << std::endl;
        cof << "extern \"C\" void map_" << name.fileName << "_init(GameScene& scene)" << std::endl;
        cof << "{" << std::endl;

        std::size_t i = 0;
        for (const auto& actor : actors)
        {
            cof << "    scene.addActor<" << actor.type << ">(";
            bool nf = false;
            for (const auto& param : actor.insertedParams)
            {
                if (nf) cof << ", ";
                cof << param;
                nf = true;
            }
            cof << ");" << std::endl;
            i++;
        }

        cof << "}" << std::endl << std::endl;
    }

    // Write the header
    {
        std::string outh = argv[5];
        std::ofstream hof;
        hof.exceptions(std::ofstream::failbit | std::ofstream::badbit);
        hof.open(outh);
        hof << "// " << std::endl;
        hof << "// " << outh << std::endl;
        hof << "// " << std::endl;
        hof << "#pragma once" << std::endl << std::endl;
        hof << "namespace " << name.nmspace << std::endl;
        hof << '{' << std::endl;
        hof << "    extern const MapData " << name.fileName << ';' << std::endl;
        hof << '}' << std::endl << std::endl;
    }

    return 0;
}

std::vector<std::uint32_t> decodeDataLayer(std::string datatext, std::string encoding);

std::vector<std::uint32_t> getTiles(tinyxml2::XMLHandle map, const std::map<std::uint32_t, std::uint32_t> &tileEquivalences)
{
    // Find the map layer
    tinyxml2::XMLElement* layer = nullptr;
    for (auto elm = map.FirstChildElement("layer"); elm.ToElement(); elm = map.NextSiblingElement("layer"))
    {
        auto curLayer = elm.ToElement();
        if (curLayer->Attribute("name", "map"))
        {
            layer = curLayer;
            break;
        }
    }
    if (layer == nullptr) throw std::domain_error("No map layer found!");

    // Extract data from it
    auto data = layer->FirstChildElement("data");
    if (data == nullptr) throw std::domain_error("Map layer is found but no data element!");
    auto datatext = data->FirstChild()->ToText();
    if (datatext == nullptr) throw std::domain_error("Data element must have a text child element!");
    auto tiles = decodeDataLayer(datatext->Value(), data->Attribute("encoding"));

    // Now, we rearrange the tiles
    for (auto& tile : tiles)
    {
        auto it = tileEquivalences.upper_bound(tile);
        it--;

        if (it->second == 0) tile = 0;
        else tile = tile + it->second - it->first;
    }

    return tiles;
}

std::string base64_decode(std::string const& encoded_string);

std::vector<std::uint32_t> decodeDataLayer(std::string datatext, std::string encoding)
{
    std::vector<std::uint32_t> curData;

    if (encoding == "base64")
    {
        auto data = base64_decode(datatext);
        curData.assign(reinterpret_cast<std::uint32_t*>(data.data()),
            reinterpret_cast<std::uint32_t*>(data.data() + data.size()));
    }
    else if (encoding == "csv")
    {
        // Run through each comma
        for (auto curSegment : split(datatext, ','))
            curData.push_back(std::stoul(std::string(curSegment)));
    }
    else throw std::invalid_argument("Data layer encoding must be either csv or base64");

    return curData;
}

ActorReturn getActorsFromMap(tinyxml2::XMLHandle map, const std::map<std::string, ActorType>& actorTypes)
{
    ActorReturn result;

    // Find the actors objectgroup
    tinyxml2::XMLElement* group = nullptr;
    for (auto elm = map.FirstChildElement("objectgroup"); elm.ToElement(); elm = map.NextSiblingElement("objectgroup"))
    {
        auto curGroup = elm.ToElement();
        if (curGroup->Attribute("name", "actors"))
        {
            group = curGroup;
            break;
        }
    }
    if (group == nullptr) throw std::domain_error("No actors objectgroup found!");

    // Now, find all objects into that group
    for (auto obj = group->FirstChildElement("object"); obj; obj = obj->NextSiblingElement("object"))
    {
        // Gets the important information
        Actor actor;
        auto ty = obj->Attribute("type");
        if (ty == nullptr) throw std::domain_error("Object's type must not be empty!");
        actor.type = ty;
        actor.insertedParams.push_back(obj->Attribute("x"));
        actor.insertedParams.push_back(obj->Attribute("y"));

        // Find the actor's type
        auto it = actorTypes.find(actor.type);
        if (it == actorTypes.end())
            throw std::domain_error("No actor type named " + actor.type + ".");
        const auto& actorType = it->second;

        // Check the actor's properties
        auto props = obj->FirstChildElement("properties");
        if (props == nullptr && !actorType.parameters.empty())
            throw std::domain_error("Parameters must be provided for actor type " + actor.type + ".");
        
        // Checks for the other parameters
        std::vector<bool> providedParameters(actorType.parameters.size(), false);
        actor.insertedParams.resize(providedParameters.size() + 2);
        if (!actorType.parameters.empty())
        {
            for (auto prop = props->FirstChildElement("property"); prop; prop = prop->NextSiblingElement("property"))
            {
                // Checks the name
                auto name = prop->Attribute("name");
                if (name == nullptr) continue;

                auto it = std::find_if(actorType.parameters.begin(), actorType.parameters.end(),
                    [&name] (const Parameter& param) { return param.name == name; });
                if (it == actorType.parameters.end()) continue;

                // Places the parameter in the correct place
                auto ofs = it - actorType.parameters.begin();
                switch (it->type)
                {
                    case ParamType::Literal: actor.insertedParams[2+ofs] = prop->Attribute("value"); break;
                    case ParamType::String: actor.insertedParams[2+ofs] = std::string("\"") + prop->Attribute("value") + '"'; break;
                    case ParamType::Color: throw std::domain_error("Color ParamType not implemented yet!");
                    default: break;
                }

                providedParameters[ofs] = true;
            }

            if (std::any_of(providedParameters.begin(), providedParameters.end(), std::logical_not<bool>()))
                throw std::domain_error("Missing parameters for actor type " + actor.type + ".");
        }

        // Inserts the parameter in the list
        result.occurences.insert(actor.type);
        result.actors.emplace_back(std::move(actor));
    }

    return result;
}
