#include "autotile-gen.hpp"
#include <stdexcept>
#include <algorithm>
#include "tgraphics.hpp"
#include "../tools/tinyxml2.h"

struct Offset { int x, y; };
extern Offset autotileOffsets[47][4];
extern std::size_t outputTileRules[47];
extern std::size_t regionTileRules[47];

Offset offsetCorners[8] = { {-1,-1}, {0,-1}, {1,-1}, {1,0}, {1,1}, {0,1}, {-1,1}, {-1,0} };

void expandAutotile(const std::string &baseName, std::size_t id, const util::grid<Color>& in)
{
    if (id >= 4) throw std::invalid_argument("Id must be from 0 to 3!");

    // Expand the tilemap according to the RPG Maker tilemap rules
    util::grid<Color> out(8 * TileSize, 6 * TileSize);

    for (std::size_t i = 0; i < 48; i++)
    {
        auto ik = std::min(i, std::size_t(46));
        for (std::size_t j = 0; j < 4; j++)
        {
            std::size_t outX = TileSize * (i % 8) + HalfTileSize * (j % 2);
            std::size_t outY = TileSize * (i / 8) + HalfTileSize * (j / 2);
            std::size_t inX = 2 * TileSize * id + HalfTileSize * autotileOffsets[ik][j].x;
            std::size_t inY = HalfTileSize * autotileOffsets[ik][j].y;
            blit(in, inX, inY, out, outX, outY, HalfTileSize, HalfTileSize);
        }
    }

    // Build the filename
    auto filename = baseName + "/autotile" + std::to_string(id) + ".png";
    saveImage(filename, out);
}

void generateTileset(const std::string &baseName, std::size_t id, Color *transparentColor)
{
    // Create the tileset in which the expanded autotile will be allocated
    using namespace tinyxml2;

    auto atname = "autotile" + std::to_string(id);
    auto atfile = atname + ".png";

    XMLDocument doc;
    auto tset = doc.NewElement("tileset");
    tset->SetAttribute("name", atname.c_str());
    tset->SetAttribute("tilewidth", (int)TileSize);
    tset->SetAttribute("tileheight", (int)TileSize);
    doc.InsertFirstChild(tset);

    auto image = doc.NewElement("image");
    image->SetAttribute("source", atfile.c_str());
    image->SetAttribute("width", (int)(8 * TileSize));
    image->SetAttribute("height", (int)(6 * TileSize));
    if (transparentColor)
        image->SetAttribute("trans", toHex(*transparentColor >> 8).c_str());
    tset->InsertFirstChild(image);

    // Save the document
    auto filename = baseName + '/' + atname + ".tsx";
    doc.SaveFile(filename.c_str());
}

std::string base64_encode(unsigned char const* , unsigned int len);

static tinyxml2::XMLElement* makeLayerWithMap(const util::grid<std::uint32_t>& grid,
    const std::string& name, tinyxml2::XMLDocument& doc, bool addStrictEmptyRule = false)
{
    auto layer = doc.NewElement("layer");
    layer->SetAttribute("name", name.c_str());
    layer->SetAttribute("width", (int)grid.width());
    layer->SetAttribute("height", (int)grid.height());

    auto data = doc.NewElement("data");
    data->SetAttribute("encoding", "base64");
    layer->InsertEndChild(data);

    auto base64 = base64_encode(reinterpret_cast<const unsigned char*>(grid.data()),
        grid.width() * grid.height() * sizeof(std::uint32_t));
    data->InsertEndChild(doc.NewText(base64.c_str()));

    if (addStrictEmptyRule)
    {
        auto properties = doc.NewElement("properties");
        auto property = doc.NewElement("property");
        property->SetAttribute("name", "StrictEmpty");
        property->SetAttribute("type", "bool");
        property->SetAttribute("value", "true");
        properties->InsertEndChild(property);
        layer->InsertFirstChild(properties);
    }

    return layer;
}

void generateRule(const std::string &baseName, std::size_t id)
{
    // Finally generate the tile rule
    using namespace tinyxml2;

    auto atname = "autotile" + std::to_string(id);
    auto atfile = atname + ".tsx";

    XMLDocument doc;
    auto map = doc.NewElement("map");
    map->SetAttribute("version", "1.0");
    map->SetAttribute("orientation", "orthogonal");
    map->SetAttribute("width", "31");
    map->SetAttribute("height", "23");
    map->SetAttribute("tilewidth", (int)TileSize);
    map->SetAttribute("tileheight", (int)TileSize);
    doc.InsertFirstChild(map);

    // Properties
    auto properties = doc.NewElement("properties");
    for (auto pname : { "MatchOutsideMap", "OverflowBorder" })
    {
        auto prop = doc.NewElement("property");
        prop->SetAttribute("name", pname);
        prop->SetAttribute("type", "bool");
        prop->SetAttribute("value", "true");
        properties->InsertEndChild(prop);
    }
    map->InsertEndChild(properties);

    // The tileset
    auto tileset = doc.NewElement("tileset");
    tileset->SetAttribute("firstgid", "1");
    tileset->SetAttribute("source", atfile.c_str());
    map->InsertEndChild(tileset);

    // Now build the regions
    util::grid<std::uint32_t> regionsInput(31, 23);
    util::grid<std::uint32_t> regionsOutput(31, 23);
    util::grid<std::uint32_t> tileOutput(31, 23);
    for (std::size_t i = 0; i < 47; i++)
    {
        std::size_t y = i / 8;
        std::size_t x = i % 8;
        std::size_t id = i;

        for (std::size_t j = 0; j < 8; j++)
        {
            const auto& cr = offsetCorners[j];
            if ((regionTileRules[id] << j) & 0x80)
                regionsInput(4*x+1 + cr.x, 4*y+1 + cr.y) = 1;
        }

        regionsInput(4*x+1, 4*y+1) = 1;
        regionsOutput(4*x+1, 4*y+1) = 1;
        tileOutput(4*x+1, 4*y+1) = id+1;
    }

    map->InsertEndChild(makeLayerWithMap(regionsInput, "regions_input", doc));
    map->InsertEndChild(makeLayerWithMap(regionsOutput, "regions_output", doc));
    map->InsertEndChild(makeLayerWithMap(tileOutput, "output_map", doc));

    for (std::size_t k = 0; k < 47; k++)
    {
        util::grid<std::uint32_t> tileInput(31, 23);
        util::grid<std::uint32_t> tileNotInput(31, 23);

        for (std::size_t i = 0; i < 47; i++)
        {
            std::size_t y = i / 8;
            std::size_t x = i % 8;
            std::size_t id = i;

            for (std::size_t j = 0; j < 8; j++)
            {
                const auto& cr = offsetCorners[j];
                if ((regionTileRules[id] << j) & 0x80)
                {
                    if ((outputTileRules[id] << j) & 0x80)
                        tileInput(4*x+1 + cr.x, 4*y+1 + cr.y) = k+1;
                    else tileNotInput(4*x+1 + cr.x, 4*y+1 + cr.y) = k+1;
                }
            }

            tileInput(4*x+1, 4*y+1) = k+1;
        }

        map->InsertEndChild(makeLayerWithMap(tileInput, "input_map", doc));
        map->InsertEndChild(makeLayerWithMap(tileNotInput, "inputnot_map", doc));
    }

    // Save the document
    auto filename = baseName + '/' + atname + ".rule.tmx";
    doc.SaveFile(filename.c_str());
}

Offset autotileOffsets[47][4] =
{
    // Corners
    { {2,4},{1,4},{2,3},{1,3} },
    { {2,0},{1,4},{2,3},{1,3} },
    { {2,4},{3,0},{2,3},{1,3} },
    { {2,0},{3,0},{2,3},{1,3} },
    { {2,4},{1,4},{2,3},{3,1} },
    { {2,0},{1,4},{2,3},{3,1} },
    { {2,4},{3,0},{2,3},{3,1} },
    { {2,0},{3,0},{2,3},{3,1} },
    { {2,4},{1,4},{2,1},{1,3} },
    { {2,0},{1,4},{2,1},{1,3} },
    { {2,4},{3,0},{2,1},{1,3} },
    { {2,0},{3,0},{2,1},{1,3} },
    { {2,4},{1,4},{2,1},{3,1} },
    { {2,0},{1,4},{2,1},{3,1} },
    { {2,4},{3,0},{2,1},{3,1} },
    { {2,0},{3,0},{2,1},{3,1} },

    // Sides
    { {0,4},{1,4},{0,3},{1,3} },
    { {0,4},{3,0},{0,3},{1,3} },
    { {0,4},{1,4},{0,3},{3,1} },
    { {0,4},{3,0},{0,3},{3,1} },
    { {2,2},{1,2},{2,3},{1,3} },
    { {2,2},{1,2},{2,3},{3,1} },
    { {2,2},{1,2},{2,1},{1,3} },
    { {2,2},{1,2},{2,1},{3,1} },
    { {2,4},{3,4},{2,3},{3,3} },
    { {2,4},{3,4},{2,1},{3,3} },
    { {2,0},{3,4},{2,3},{3,3} },
    { {2,0},{3,4},{2,1},{3,3} },
    { {2,4},{1,4},{2,5},{1,5} },
    { {2,0},{1,4},{2,5},{1,5} },
    { {2,4},{3,0},{2,5},{1,5} },
    { {2,0},{3,0},{2,5},{1,5} },

    // Pipes
    { {0,4},{3,4},{0,3},{3,3} },
    { {2,2},{1,2},{2,5},{1,5} },

    // Real corners
    { {0,2},{1,2},{0,3},{1,3} },
    { {0,2},{1,2},{0,3},{3,1} },
    { {2,2},{3,2},{2,3},{3,3} },
    { {2,2},{3,2},{2,1},{3,3} },
    { {2,4},{3,4},{2,5},{3,5} },
    { {2,0},{3,4},{2,5},{3,5} },
    { {0,4},{1,4},{0,5},{1,5} },
    { {0,4},{3,0},{0,5},{1,5} },

    // Dead-ends
    { {0,2},{3,2},{0,3},{3,3} },
    { {0,2},{1,2},{0,5},{1,5} },
    { {0,4},{3,4},{0,5},{3,5} },
    { {2,2},{3,2},{2,5},{3,5} },

    // Single block
    { {0,2},{3,2},{0,5},{3,5} }
};

std::size_t outputTileRules[47] =
{
    0b11111111, 0b01111111, 0b11011111, 0b01011111, 0b11110111, 0b01110111, 0b11010111, 0b01010111,
    0b11111101, 0b01111101, 0b11011101, 0b01011101, 0b11110101, 0b01110101, 0b11010101, 0b01010101,
    0b01111100, 0b01011100, 0b01110100, 0b01010100, 0b00011111, 0b00010111, 0b00011101, 0b00010101,
    0b11000111, 0b11000101, 0b01000111, 0b01000101, 0b11110001, 0b01110001, 0b11010001, 0b01010001,
    0b01000100, 0b00010001, 0b00011100, 0b00010100, 0b00000111, 0b00000101, 0b11000001, 0b01000001,
    0b01110000, 0b01010000, 0b00000100, 0b00010000, 0b01000000, 0b00000001, 0b00000000
};

std::size_t regionTileRules[47] =
{
    0b11111111, 0b11111111, 0b11111111, 0b11111111, 0b11111111, 0b11111111, 0b11111111, 0b11111111, 
    0b11111111, 0b11111111, 0b11111111, 0b11111111, 0b11111111, 0b11111111, 0b11111111, 0b11111111, 
    0b01111101, 0b01111101, 0b01111101, 0b01111101, 0b01011111, 0b01011111, 0b01011111, 0b01011111,
    0b11010111, 0b11010111, 0b11010111, 0b11010111, 0b11110101, 0b11110101, 0b11110101, 0b11110101,
    0b01010101, 0b01010101, 0b01011101, 0b01011101, 0b01010111, 0b01010111, 0b11010101, 0b11010101,
    0b01110101, 0b01110101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101,
};

