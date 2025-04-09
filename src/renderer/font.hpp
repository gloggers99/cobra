#pragma once

#include <string>
#include <map>

#include <glm/glm.hpp>

#include <freetype2/ft2build.h>
#include <freetype2/freetype/freetype.h>

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
            const std::string &font_path
        );
        ~font();
    };
}
