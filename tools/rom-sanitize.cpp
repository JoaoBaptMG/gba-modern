#include "pch.hpp"
#include <vector>
#include <fstream>
#include <numeric>

static const unsigned char Logo[] =
{
    0x24, 0xFF, 0xAE, 0x51, 0x69, 0x9A, 0xA2, 0x21, 0x3D, 0x84, 0x82, 0x0A,
    0x84, 0xE4, 0x09, 0xAD, 0x11, 0x24, 0x8B, 0x98, 0xC0, 0x81, 0x7F, 0x21,
    0xA3, 0x52, 0xBE, 0x19, 0x93, 0x09, 0xCE, 0x20, 0x10, 0x46, 0x4A, 0x4A,
    0xF8, 0x27, 0x31, 0xEC, 0x58, 0xC7, 0xE8, 0x33, 0x82, 0xE3, 0xCE, 0xBF,
    0x85, 0xF4, 0xDF, 0x94, 0xCE, 0x4B, 0x09, 0xC1, 0x94, 0x56, 0x8A, 0xC0,
    0x13, 0x72, 0xA7, 0xFC, 0x9F, 0x84, 0x4D, 0x73, 0xA3, 0xCA, 0x9A, 0x61,
    0x58, 0x97, 0xA3, 0x27, 0xFC, 0x03, 0x98, 0x76, 0x23, 0x1D, 0xC7, 0x61,
    0x03, 0x04, 0xAE, 0x56, 0xBF, 0x38, 0x84, 0x00, 0x40, 0xA7, 0x0E, 0xFD,
    0xFF, 0x52, 0xFE, 0x03, 0x6F, 0x95, 0x30, 0xF1, 0x97, 0xFB, 0xC0, 0x85,
    0x60, 0xD6, 0x80, 0x25, 0xA9, 0x63, 0xBE, 0x03, 0x01, 0x4E, 0x38, 0xE2,
    0xF9, 0xA2, 0x34, 0xFF, 0xBB, 0x3E, 0x03, 0x44, 0x78, 0x00, 0x90, 0xCB,
    0x88, 0x11, 0x3A, 0x94, 0x65, 0xC0, 0x7C, 0x63, 0x87, 0xF0, 0x3C, 0xAF,
    0xD6, 0x25, 0xE4, 0x8B, 0x38, 0x0A, 0xAC, 0x72, 0x21, 0xD4, 0xF8, 0x07
};

constexpr auto HeaderSize = 192;
constexpr auto GameTitleSize = 12;
constexpr auto GameCodeSize = 4;
constexpr auto MakerCodeSize = 2;

inline static std::vector<char> readFile(std::string str)
{
    std::ifstream in;
    in.exceptions(std::ofstream::failbit | std::ofstream::badbit);
    in.open(str, std::ios::binary);
    in.seekg(0, std::ios::end);
    std::vector<char> res(in.tellg());
    in.seekg(0, std::ios::beg);
    in.read(res.data(), res.size());
    in.close();
    return res;
}

inline static void writeFile(std::string str, const std::vector<char>& val)
{
    std::ofstream of;
    of.exceptions(std::ofstream::failbit | std::ofstream::badbit);
    of.open(str, std::ios::binary);
    of.write(val.data(), val.size());
    of.close();
}

int romSanitize(int argc, char **argv)
{
    if (argc < 5) throw std::out_of_range("rom-sanitize expects 3 arguments!");

    std::string in = argv[2];
    std::string inc = argv[3];
    std::string out = argv[4];

    // GBA ROM information taken from https://problemkaputt.de/gbatek.htm#gbacartridgeheader
    auto rom = readFile(in);
    auto rptr = rom.data();
    rptr += 4;

    rptr = std::copy_n(Logo, sizeof(Logo), rptr);

    std::string gameTitle, gameCode, makerCode;
    std::uint8_t versionNumber;

    {
        std::ifstream inf;
        inf.exceptions(std::ifstream::badbit | std::ifstream::failbit);
        inf.open(inc);

        nlohmann::json j;
        inf >> j;

        j.at("game-title").get_to(gameTitle);
        j.at("game-code").get_to(gameCode);
        j.at("maker-code").get_to(makerCode);
        j.at("version-number").get_to(versionNumber);
    }

    gameTitle.resize(GameTitleSize);
    gameCode.resize(GameCodeSize);
    makerCode.resize(MakerCodeSize);

    rptr = std::copy(gameTitle.begin(), gameTitle.end(), rptr);
    rptr = std::copy(gameCode.begin(), gameCode.end(), rptr);
    rptr = std::copy(makerCode.begin(), makerCode.end(), rptr);

    *rptr++ = 0x96;

    rptr = std::fill_n(rptr, 9, 0);
    *rptr++ = versionNumber;

    std::uint8_t chk = std::accumulate(rptr-29, rptr, 0x19);
    *rptr++ = -chk;

    rptr = std::fill_n(rptr, 2, 0);

    writeFile(out, rom);

    return 0;
}