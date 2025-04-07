#include "model.hpp"

#include <stdexcept>
#include <utility>
#include <chrono>
#include <cstring>

namespace cobra {
    void model::fileLoad(FILE *handle) {
        std::vector<glm::vec3> vertices;
        std::vector<unsigned int> faces;
        std::vector<std::tuple<glm::vec3>> compiled_vertices;
        std::vector<bool> vertex_added;

        char line[256]; // Buffer to hold each line of the file

        while (fgets(line, sizeof(line), handle)) {
            // Remove newline character if present
            line[strcspn(line, "\n")] = '\0';

            if (line[0] == 'v') {
                float x, y, z;
                if (sscanf(line, "v %f %f %f", &x, &y, &z) != 3)
                    throw std::runtime_error("Invalid vertex format");
                vertices.emplace_back(x, y, z);
                vertex_added.push_back(false); // Keep track of added vertices
            } else if (line[0] == 'f') {
                int x, y, z;
                if (sscanf(line, "f %i %i %i", &x, &y, &z) != 3)
                    throw std::runtime_error("Invalid face format");
                // Adjust for 1-based to 0-based index
                if (x > 0 && x <= vertices.size()) faces.push_back(x - 1);
                if (y > 0 && y <= vertices.size()) faces.push_back(y - 1);
                if (z > 0 && z <= vertices.size()) faces.push_back(z - 1);
            }
        }

        compiled_vertices.reserve(faces.size());

        for (const unsigned int i : faces) {
            if (i < vertices.size()) {
                if (!vertex_added[i]) {
                    compiled_vertices.emplace_back(vertices[i]);
                    vertex_added[i] = true;
                }
            } else {
                throw std::runtime_error("Face index out of bounds");
            }
        }

        // Update your vertex stack and set indices
        this->vertex_stack.update(compiled_vertices);
        this->vertex_stack.setIndices(faces);
    }

    void model::allocate_load(FILE *handle, long size) {
        const auto buffer = new char[size];
        fread(buffer, size, 1, handle);
        buffer[size - 1] = '\0';

        std::vector<glm::vec3> vertices;
        std::vector<unsigned int> faces;

        const char *line = std::strtok(buffer, "\n");
        while (line != nullptr) {
            if (line[0] == 'v') {
                float x, y, z;
                if (sscanf(line, "v %f %f %f", &x, &y, &z) != 3)
                    throw std::runtime_error("Invalid vertex format");
                vertices.emplace_back(x, y, z);
            } else if (line[0] == 'f') {
                int x, y, z;
                if (sscanf(line, "f %i %i %i", &x, &y, &z) != 3)
                    throw std::runtime_error("Invalid face format");
                // Adjust for 1-based to 0-based index
                if (x > 0 && x <= vertices.size()) faces.push_back(x - 1);
                if (y > 0 && y <= vertices.size()) faces.push_back(y - 1);
                if (z > 0 && z <= vertices.size()) faces.push_back(z - 1);
            }

            line = std::strtok(nullptr, "\n");
        }

        std::vector<std::tuple<glm::vec3>> compiledVertices;
        compiledVertices.reserve(faces.size());

        std::vector vertexAdded(vertices.size(), false);

        for (const unsigned int i : faces) {
            if (i < vertices.size()) {
                if (!vertexAdded[i]) {
                    compiledVertices.emplace_back(vertices[i]);
                    vertexAdded[i] = true;
                }
            } else {
                throw std::runtime_error("Face index out of bounds");
            }
        }

        // Update your vertex stack and set indices
        this->vertex_stack.update(compiledVertices);
        this->vertex_stack.setIndices(faces);

        delete[] buffer;
    }

    void model::load_model() {
        const auto start = std::chrono::high_resolution_clock::now();

        // Here we use "rb" for no additional overhead and straight binary streaming
        FILE *handle = fopen(this->path.c_str(), "rb");
        if (!handle)
            throw std::runtime_error("Model does not exist.");

        // Calculate how big the file is and compare to the max obj size to choose which import method to use
        fseek(handle, 0, SEEK_END);
        const long size = ftell(handle);
        rewind(handle);

        if (size > COBRA_MODEL_MAX_SIZE)
            this->fileLoad(handle);
        else
            this->allocate_load(handle, size);

        fclose(handle);

        const auto end = std::chrono::high_resolution_clock::now();
        const auto duration = duration_cast<std::chrono::milliseconds>(end - start);
        std::cout << "Time taken (model): " << duration.count() << " ms" << std::endl;
    }

    void model::draw() {
        this->vertex_stack.draw();
    }

    model::model(std::string path) : path(std::move(path)) {
        this->load_model();
    }

    model::~model() = default;
}