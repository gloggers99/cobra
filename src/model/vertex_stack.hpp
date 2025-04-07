#pragma once

#include <iostream>
#include <vector>
#include <tuple>
#include <type_traits>
#include <glm/glm.hpp>
#include <glad/gl.h>

namespace cobra {
    template<typename ...Types>
    class vertex_stack {
        typedef std::tuple<Types...> descriptor;

        std::vector<descriptor> vertices;
        std::vector<GLuint> descriptor_table;

        std::vector<unsigned int> indices;

        GLuint vao;
        GLuint vbo;
        GLuint ebo;

        /**
         * Indicates if the VertexStack is ready for use
         */
        bool compiled;

        /**
         * This variable will determine the draw mode of the VBO, by default it will start as false,
         * but if the user attempts to update the VertexStack it will automatically convert itself into
         * using GL_DYNAMIC_DRAW from GL_STATIC_DRAW
         */
        bool dynamic;

        /**
         * This variable determines if the VAO will use an EBO
         */
        bool element;

        template<typename T>
        std::vector<float> flatten(const T& value) {
            if constexpr (std::is_same_v<T, glm::vec3>)
                return {value.x, value.y, value.z};
            else if constexpr (std::is_same_v<T, glm::vec2>)
                return {value.x, value.y};
            else if constexpr (std::is_arithmetic_v<T>)
                return {static_cast<float>(value)};
            else
                static_assert(always_false_v<T>, "Unsupported type");
        }

        template <typename T>
        static constexpr bool always_false_v = false;

        template<typename Tuple, std::size_t... I>
        std::vector<float> flatten_tuple(const Tuple& tuple, std::index_sequence<I...>) {
            std::vector<float> flat_data;
            ( [&] {
                auto element_data = flatten(std::get<I>(tuple));
                flat_data.insert(flat_data.end(), element_data.begin(), element_data.end());
            }(), ...);
            return flat_data;
        }

        std::vector<float> flatten_vertices() {
            std::vector<float> flatData;
            for (const auto &vertex : vertices) {
                auto vertex_data = flatten_tuple(vertex, std::make_index_sequence<sizeof...(Types)>());
                flatData.insert(flatData.end(), vertex_data.begin(), vertex_data.end());
            }
            return flatData;
        }

        template<typename X>
        [[nodiscard]] GLuint get_element_count_from_type() const {
            if constexpr (std::is_same_v<X, glm::vec3>)
                return 3;
            else if constexpr (std::is_same_v<X, glm::vec2>)
                return 2;
            else if constexpr (std::is_arithmetic_v<X>)
                return 1;
            else
                throw std::runtime_error("Invalid types in the variadic template constructor!");
        }

        void compile() {
            this->freeBuffers();

            glGenVertexArrays(1, &vao);
            glBindVertexArray(vao);

            glGenBuffers(1, &vbo);
            glBindBuffer(GL_ARRAY_BUFFER, vbo);

            std::vector<float> flat_data = flatten_vertices();
            glBufferData(GL_ARRAY_BUFFER, flat_data.size() * sizeof(float), flat_data.data(), this->dynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);

            if (this->element) {
                glGenBuffers(1, &this->ebo);
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->ebo);
                glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->indices.size() * sizeof(unsigned int), this->indices.data(), GL_STATIC_DRAW);
            }

            GLuint index = 0;
            size_t offset = 0;

            for (GLuint element_count : descriptor_table) {
                glVertexAttribPointer(index, element_count, GL_FLOAT, GL_FALSE, sizeof(float) * element_count, reinterpret_cast<void*>(offset));
                glEnableVertexAttribArray(index);
                ++index;
                offset += sizeof(float) * element_count;
            }

            glBindVertexArray(0);

            this->compiled = true;
        }

        void freeBuffers() {
            if (this->vao != 0)
                glDeleteVertexArrays(1, &this->vao);
            if (this->vbo != 0)
                glDeleteBuffers(1, &this->vbo);
            if (this->ebo != 0)
                glDeleteBuffers(1, &this->ebo);
        }

    public:
        void draw() {
            if (!this->compiled)
                this->compile();

            glBindVertexArray(this->vao);
            if (this->element)
                glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(this->indices.size()), GL_UNSIGNED_INT, nullptr);
            else
                glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(this->vertices.size()));
        }

        void update(const std::vector<descriptor> &vertices) {
            this->vertices = vertices;

            if (!this->compiled) {
                this->compile();
                return;
            }

            if (!this->dynamic) {
                this->dynamic = true;
                this->compile();
            }

            const std::vector<float> flat_data = flatten_vertices();

            glBindVertexArray(this->vao);
            glBindBuffer(GL_ARRAY_BUFFER, this->vbo);

            glBufferSubData(GL_ARRAY_BUFFER, 0, flat_data.size() * sizeof(float), flat_data.data());

            glBindVertexArray(0);
        }

        void setIndices(const std::vector<unsigned int> &indices) {
            this->indices = indices;
            if (!this->element) {
                this->element = true;
            }
            this->compile();
        }

        explicit vertex_stack(std::vector<descriptor> vertices) :
                vertices(std::move(vertices)),
                descriptor_table({get_element_count_from_type<Types>()...}),
                vao(0),
                vbo(0),
                ebo(0),
                compiled(false),
                dynamic(false),
                element(false) {
            this->compile();
        }

        vertex_stack() :
                descriptor_table({get_element_count_from_type<Types>()...}),
                vao(0),
                vbo(0),
                ebo(0),
                compiled(false),
                dynamic(false),
                element(false) {}

        ~vertex_stack() {
            this->freeBuffers();
        }
    };
}