#include "pch.hpp"

#include <ft2build.h>
#include FT_FREETYPE_H

// Errors
#undef FTERRORS_H_
#define FT_ERRORDEF( e, v, s ) { e, s },
#define FT_ERROR_START_LIST
#define FT_ERROR_END_LIST

static std::unordered_map<FT_Error, const char*> errors =
{
#include FT_ERRORS_H
};

void checkError(FT_Error error)
{
    if (error) throw std::domain_error(std::string("FreeType error: ") + errors.find(error)->second);
}

struct Rectangle { int x, y, width, height; };

struct GlyphData
{
    std::vector<std::size_t> data;
    int offsetX, offsetY, advanceX, attr;
};

GlyphData getGlyph(FT_Face face, int codepoint);

struct GlyphBuffer
{
    std::vector<std::uint16_t> smallGlyphPool;
    std::vector<std::uint32_t> largeGlyphPool;
    std::vector<std::size_t> glyphIndices;
};

GlyphBuffer buildGlyphBuffer(const std::vector<GlyphData>& glyphs);

int fontExport(int argc, char **argv)
{
    if (argc < 5) throw std::out_of_range("sprite-export expects 3 arguments!");

    std::string in = argv[2];
    std::string out = argv[3];
    std::string outh = argv[4];

    // Parameters
    std::size_t exportSize;
    std::vector<std::vector<std::size_t>> exportIntervals;
    int verticalStride = -1;

    {
        nlohmann::json j;
        std::ifstream mdin;
        mdin.exceptions(std::ofstream::failbit | std::ofstream::badbit);
        mdin.open(in + ".json");
        mdin >> j;
        mdin.close();

        j.at("export-size").get_to(exportSize);
        j.at("export-range").get_to(exportIntervals);
        if (j.contains("override-vertical-stride"))
            j.at("override-vertical-stride").get_to(verticalStride);
    }

    if (exportIntervals.empty())
        throw std::domain_error("You must export at least one interval!");

    // Initialize FreeType and set the font parameters
    FT_Library library;
    FT_Face face;
    checkError(FT_Init_FreeType(&library));
    checkError(FT_New_Face(library, argv[2], 0, &face));
    checkError(FT_Set_Char_Size(face, 0, exportSize*64, 72, 72));

    // Get the vertical stride
    if (verticalStride == -1)
        verticalStride = face->size->metrics.height >> 6;

    // Push each glyph to the rectangle
    std::vector<GlyphData> glyphs;
    for (const auto& v : exportIntervals)
        for (std::size_t i = v[0]; i <= v[1]; i++)
            glyphs.push_back(getGlyph(face, i));

    // Compute the glyph buffer
    auto [smallGlyphs, largeGlyphs, glyphIndices] = buildGlyphBuffer(glyphs);

    // Compute the minimum vertical ascent of the font
    int minOfsY = 1000000000;
    for (const GlyphData& glyph : glyphs)
        minOfsY = std::min(minOfsY, (int)glyph.offsetY);

    auto name = deriveSpecialName(in);

    // Write the data file
    {
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

        // Write the glyph metadata
        for (std::size_t i = 0; i < glyphs.size(); i++)
        {
            const auto& glyph = glyphs[i];

            // Mount the word
            std::uint32_t wrd = std::uint8_t(glyph.data.size());
            wrd |= std::uint8_t(glyph.offsetX) << 8;
            wrd |= std::uint8_t(glyph.offsetY) << 16;
            wrd |= std::uint8_t(glyph.advanceX) << 24;
            wrd |= std::uint32_t(glyph.attr) << 31;

            of << "    .word " << toHex(wrd, 8) << ", fnt_" << name.fileName << "_glyphdata+";
            of << glyphIndices[i] << std::endl;
        }
        of << std::endl;

        // Now the glyph data properly
        of << "    .section .rodata" << std::endl;
        of << "    .align 2" << std::endl;
        of << "    .hidden fnt_" << name.fileName << "_glyphdata" << std::endl;
        of << "fnt_" << name.fileName << "_glyphdata:";

        std::size_t index = 0;
        for (std::uint16_t r : smallGlyphs)
        {
            if (index % 8 == 0) of << std::endl << "    .hword ";
            else of << ", ";
            of << toHex(r, 4);
            index++;
        }

        of << std::endl;
        index = 0;
        for (std::uint32_t r : largeGlyphs)
        {
            if (index % 4 == 0) of << std::endl << "    .word ";
            else of << ", ";
            of << toHex(r, 8);
            index++;
        }

        of << std::endl << std::endl;
    }

    // Write the header
    {
        std::ofstream hof;
        hof.exceptions(std::ofstream::failbit | std::ofstream::badbit);
        hof.open(outh);
        hof << "// " << std::endl;
        hof << "// " << outh << std::endl;
        hof << "// " << std::endl;
        hof << "#pragma once" << std::endl << std::endl;
        hof << "#include \"data/Font.hpp\"" << std::endl << std::endl;
        hof << "namespace " << name.nmspace << std::endl;
        hof << "{" << std::endl;
        hof << "    extern const FontHandle<" << verticalStride << "," << std::endl << "        ";

        bool nf = false;
        for (const auto& v : exportIntervals)
        {
            if (nf) hof << "," << std::endl << "        ";
            hof << "detail::font::GlyphInterval<" << v[0] << ", " << v[1] << '>';
            nf = true;
        }

        hof << std::endl << "    > " << name.fileName << ';' << std::endl;
        hof << "}" << std::endl;
    }

    return 0;
}

std::size_t reverseBits(std::size_t v, std::size_t numBits)
{
    std::size_t x = 0;

    for (std::size_t i = 0; i < numBits; i++)
    {
        x = (x << 1) | (v&1);
        v >>= 1;
    }

    return x;
} 

GlyphData getGlyph(FT_Face face, int codepoint)
{
    // Load and render the glyph
    auto glyphIndex = FT_Get_Char_Index(face, codepoint);
    checkError(FT_Load_Glyph(face, glyphIndex, FT_LOAD_DEFAULT));
    checkError(FT_Render_Glyph(face->glyph, FT_RENDER_MODE_MONO));

    GlyphData glyphData;

    // fill the data attributes
    glyphData.advanceX = face->glyph->advance.x >> 6;
    glyphData.offsetX = face->glyph->bitmap_left;
    glyphData.offsetY = -face->glyph->bitmap_top;

    const auto& bitmap = face->glyph->bitmap;
    glyphData.attr = bitmap.width > 16;

    // Bitpack the bitmap
    for (std::size_t j = 0; j < bitmap.rows; j++)
    {
        std::size_t row = 0;

        // The loading we do here is because of FreeType's bitmap format.
        // It uses a byte-by-byte, MSB-first 1bpp format, while we use a
        // hword (or word) LSB-first 1bpp format, so we need to convert
        // between those
        for (int i = bitmap.pitch-1; i >= 0; i--)
            row = (row << 8) | reverseBits(bitmap.buffer[j * bitmap.pitch + i], 8);
        glyphData.data.push_back(row);
    }

    // Trim the amount of blank at the beginning and the end
    auto it = std::find_if(glyphData.data.begin(), glyphData.data.end(),
        [](std::size_t r) { return r != 0; });
    glyphData.offsetY += it - glyphData.data.begin();
    glyphData.data.erase(glyphData.data.begin(), it);
    while (!glyphData.data.empty() && !glyphData.data.back()) glyphData.data.pop_back();

    return glyphData;
}

GlyphBuffer buildGlyphBuffer(const std::vector<GlyphData>& glyphs)
{
    // We are doing to do "dumb" glyph building here
    GlyphBuffer glyphBuffer;

    for (const auto& glyph : glyphs)
    {
        if (!(glyph.attr&1))
        {
            glyphBuffer.glyphIndices.push_back(sizeof(std::uint16_t) * glyphBuffer.smallGlyphPool.size());
            glyphBuffer.smallGlyphPool.insert(glyphBuffer.smallGlyphPool.end(), glyph.data.begin(), glyph.data.end());
        }
        else
        {
            glyphBuffer.glyphIndices.push_back(sizeof(std::uint32_t) * glyphBuffer.largeGlyphPool.size());
            glyphBuffer.largeGlyphPool.insert(glyphBuffer.largeGlyphPool.end(), glyph.data.begin(), glyph.data.end());
        }
    }

    auto offset = sizeof(std::uint16_t) * glyphBuffer.smallGlyphPool.size();
    for (std::size_t i = 0; i < glyphs.size(); i++)
        if (glyphs[i].attr) glyphBuffer.glyphIndices[i] += offset;

    return glyphBuffer;
}
