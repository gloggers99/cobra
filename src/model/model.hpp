#pragma once

#include <string>
#include <utility>

#include "vertex_stack.hpp"

/**
 * The format of each vertex that the model imported will use.
 */
#define COBRA_MODEL_VERTEX glm::vec3

/**
 * The amount of bytes before using the file stream loading method by default,
 * this is to prevent memory being overloaded.
 */
#define COBRA_MODEL_MAX_SIZE 10000000

namespace cobra {
    enum class model_load_method {
        FILE,
        ALLOCATE
    };

    /**
     * Model is used for importing .obj files & more in the future,
     * under the hood C functionality is used for optimal performance.
     */
    class model {
        std::string path;
        vertex_stack<COBRA_MODEL_VERTEX> vertex_stack;

        /**
         * Load the model by reading directly from the model's file
         */
        void fileLoad(FILE *handle);

        /**
         * Load the model by pre-allocating the file's content into memory
         */
        void allocate_load(FILE *handle, long size);

        void load_model();

    public:
        void draw();

        /**
         * Construct a model
         * @param path The filepath to the model
         */
        explicit model(std::string path);
        ~model();
    };
}