#include "pch.hpp"

using Tool = int(*)(int argc, char **argv);

int spriteExport(int argc, char **argv);
int backgroundExport(int argc, char **argv);
int fontExport(int argc, char **argv);
int romSanitize(int argc, char **argv);
int audioExportSettings(int argc, char **argv);
int soundExport(int argc, char **argv);

const std::map<std::string, Tool> toolList =
{
    { "sprite-export", spriteExport },
    { "background-export", backgroundExport },
    { "font-export", fontExport },
    { "rom-sanitize", romSanitize },
    { "audio-export-settings", audioExportSettings },
    { "sound-export", soundExport }
};

int main(int argc, char **argv)
{
    if (argc < 4)
    {
        std::cout << "Usage: " << argv[0] << " <tool> <input> <output>" << std::endl;
        return -1;
    }

    auto it = toolList.find(argv[1]);
    if (it == toolList.end())
    {
        std::cout << "Tool " << argv[1] << " not found in the tool list!" << std::endl;
        return -1;
    }

    try
    {
        return it->second(argc, argv);
    }
    catch (const std::exception &exc)
    {
        std::cout << "Error processing file " << argv[2] << ": " << exc.what() << std::endl;
        return -1;
    }
}