#include "font.hpp"

#include <iostream>
#include <stdexcept>

#include <glad/gl.h>

namespace cobra {
    FT_Library font_library = nullptr;
    bool font_library_initialized = false;

    font::font(const std::string &font_path)
        : face(nullptr),
          characters({}) {
        if (!font_library_initialized) {
            if (FT_Init_FreeType(&font_library))
                throw std::runtime_error("cobra: Failed to load freetype library!");
            font_library_initialized = true;
        }

        if (FT_New_Face(
            font_library,
            font_path.c_str(),
            0,
            &this->face)) {
            throw std::runtime_error("cobra: Failed to load freetype font face!");
        }

        // TODO: figure out if we need to disable this afterwards:
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

        for (unsigned char c = 0; c < ASCII_TABLE_CHARACTER_COUNT; c++) {
            if (FT_Load_Char(this->face, c, FT_LOAD_RENDER)) {
                std::cerr << "cobra: Failed to load glyph \'" << c << "\'";
                continue;
            }

            unsigned int texture;
            glGenTextures(1, &texture);
            glBindTexture(GL_TEXTURE_2D, texture);
            glTexImage2D(
                GL_TEXTURE_2D,
                0,
                GL_RED,
                this->face->glyph->bitmap.width,
                this->face->glyph->bitmap.rows,
                0,
                GL_RED,
                GL_UNSIGNED_BYTE,
                this->face->glyph->bitmap.buffer
            );

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            character character_struct {
                texture,
                glm::ivec2(this->face->glyph->bitmap.width, this->face->glyph->bitmap.rows),
                glm::ivec2(this->face->glyph->bitmap_left, this->face->glyph->bitmap_top),
                face->glyph->advance.x
            };

            this->characters.insert(std::pair<char, character>(c, character_struct));
        }
    }

    font::~font() {
        FT_Done_Face(this->face);
    }

    /**
     * Destroy freetype library automatically
     */
    __attribute__((destructor)) void destroy_freetype() {
        FT_Done_FreeType(font_library);
    }
}
