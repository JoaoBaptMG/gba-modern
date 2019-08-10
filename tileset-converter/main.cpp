#include <iostream>
#include <fstream>
#include <stdexcept>

#include "../tools/nlohmann/json.hpp"

#include "defs.hpp"
#include "tgraphics.hpp"
#include "autotile-gen.hpp"

#include "../tools/tinyxml2.h"

#if _MSC_VER
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#else
extern "C" int mkdir(const char* name, int mode);
#endif

void generateBaseTileset(const std::string &baseName, std::size_t height, Color *transparentColor = nullptr)
{
    // Create the tileset in which the expanded autotile will be allocated
    using namespace tinyxml2;

    auto atname = "tileset";
    auto atfile = "tileset.png";

    XMLDocument doc;
    auto tset = doc.NewElement("tileset");
    tset->SetAttribute("name", atname);
    tset->SetAttribute("tilewidth", (int)TileSize);
    tset->SetAttribute("tileheight", (int)TileSize);
    doc.InsertFirstChild(tset);

    auto image = doc.NewElement("image");
    image->SetAttribute("source", atfile);
    image->SetAttribute("width", (int)(8 * TileSize));
    image->SetAttribute("height", (int)(height - 3 * TileSize));
    if (transparentColor)
        image->SetAttribute("trans", toHex(*transparentColor >> 8).c_str());
    tset->InsertFirstChild(image);

    // Save the document
    auto filename = baseName + '/' + atname + ".tsx";
    doc.SaveFile(filename.c_str());
}

int main(int argc, char** argv)
{
    if (argc < 3)
    {
        std::cout << "Usage: " << argv[0] << " <input.png> <output-folder>" << std::endl;
        return -1;
    }

    try
    {
        // First, decode the tilemap
        Color transparentColor;
        std::string baseName = argv[2];
        auto tilemap = loadImage(argv[1], &transparentColor);

        // Make sure it has the right size
        if (tilemap.width() != 8 * TileSize)
            throw std::runtime_error("Tilemap must have width " + std::to_string(8 * TileSize) + "!");
        if (tilemap.height() % TileSize != 0)
            throw std::runtime_error("Tilemap must have height multiple of  " + std::to_string(TileSize) + "!");
        if (tilemap.height() < 3 * TileSize)
            throw std::runtime_error("Tilemap must have height greater than or equal to  " + std::to_string(3 * TileSize) + "!");

        // Read the JSON to know how many autotiles we should expect
        std::size_t numAutotiles;
        {
            nlohmann::json j;
            std::ifstream inj(argv[1] + std::string(".json"));
            inj >> j;
            numAutotiles = j.at("num-autotiles").get<std::size_t>();
        }

        // Create the required folder
        mkdir(argv[2], 0777);

        // Generate the autotiles
        for (std::size_t i = 0; i < numAutotiles; i++)
        {
            expandAutotile(baseName, i, tilemap);
            generateTileset(baseName, i, &transparentColor);
            generateRule(baseName, i);
        }

        // Generate the "remaining" of the tileset
        saveImage(baseName + "/tileset.png",
            tilemap.make_view(0, 3 * TileSize, tilemap.width(), tilemap.height() - 3 * TileSize));

        generateBaseTileset(baseName, tilemap.height(), &transparentColor);

        // Create the rules.txt
        std::ofstream rules(baseName + "/rules.txt");
        rules << "// rules" << std::endl;

        for (std::size_t i = 0; i < numAutotiles; i++)
            rules << "autotile" << i << ".rule.tmx" << std::endl;

        rules.close();
    }
    catch(const std::exception& e)
    {
        std::cout << "Error processing file " << argv[1] << ": " << e.what() << std::endl;
        return -1;
    }   
}