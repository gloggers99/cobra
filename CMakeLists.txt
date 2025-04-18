cmake_minimum_required(VERSION 3.28)
project(cobra)

set(CMAKE_CXX_STANDARD 26)

add_custom_target(
        EmbedShaders ALL
        COMMAND ${CMAKE_COMMAND} -E env python3 ${CMAKE_SOURCE_DIR}/scripts/shader_embedder.py
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        COMMENT "Embedding shaders"
)

add_library(cobra STATIC
        src/cobra.hpp
        src/renderer/renderer.cpp
        src/renderer/renderer.hpp
        shaders/include/shader.hpp
        shaders/include/shader.cpp
        shaders/include/uniform.hpp
        src/model/vertex_stack.hpp
        shaders/include/color.hpp
        src/model/model.cpp
        src/model/model.hpp
        src/matrix/camera.cpp
        src/matrix/camera.hpp
        src/matrix/matrix_interface.hpp
        src/renderer/window_spec.hpp
        src/app.cpp
        src/app.hpp
        src/renderer/font.cpp
        src/renderer/font.hpp
        src/matrix/ortho.cpp
        src/matrix/ortho.hpp
)

set(CMAKE_EXE_LINKER_FLAGS "--static -mwindows")
add_dependencies(cobra EmbedShaders)
target_include_directories(cobra PUBLIC include)
target_include_directories(cobra PUBLIC shaders/include/)
target_include_directories(cobra PUBLIC "C:\\msys64\\mingw64\\include\\freetype2\\")
target_link_libraries(cobra PUBLIC glfw3 freetype png harfbuzz bz2 graphite2 z ole32 brotlidec brotlicommon rpcrt4)

add_executable(cobra_test main.cpp)
target_link_libraries(cobra_test PRIVATE cobra)