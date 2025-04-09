#pragma once

#include <filesystem>
#include <map>

#include <glm/glm.hpp>

#include <freetype2/freetype/freetype.h>
#define FT_FREETYPE_H

namespace cobra {
    /**
     * Define the amount of characters to load from
     * a given font face
     */
    constexpr int ASCII_TABLE_CHARACTER_COUNT = 128;

    /**
     * This structure is for storing texture information
     * for each ASCII character within the font.
     */
    struct character {
        unsigned int texture;
        glm::ivec2 size;
        glm::ivec2 bearing;
        unsigned int advance;
    };

    class font {
        FT_Face face;

        std::map<char, character> characters;
    public:
        explicit font(
            const std::filesystem::path &font_path
        );
        ~font();
    };
}
