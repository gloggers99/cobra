#include "src/cobra.hpp"

#include <glad/gl.h>

#include <shaders/test.hcs>
#include <shaders/text.hcs>

class test_app final : public cobra::app {
    cobra::camera camera;
    cobra::ortho ortho;
    cobra::model ogre;

    cobra::font font;
public:
    explicit test_app(cobra::renderer &renderer)
        : app(renderer),
          camera(renderer, {
              {GLFW_KEY_W, cobra::direction::FORWARD},
              {GLFW_KEY_A, cobra::direction::LEFT},
              {GLFW_KEY_S, cobra::direction::BACKWARD},
              {GLFW_KEY_D, cobra::direction::RIGHT},
          }),
          ortho(renderer),
          ogre("test.obj"),
          font("roboto.ttf") {
        shader::compile(test);
        shader::compile(text);

        this->ortho.correlate(text.projection);
    }

    ~test_app() override = default;

    cobra::app_status update(cobra::renderer &renderer, const double delta_time) override {
        renderer.clear();
        renderer.clear_color(cobra::color(0.2f, 0.3f, 0.3f, 1.0f));

        shader::use(test);
        this->camera.input_hook(delta_time);
        this->camera.correlate(test.camera);

        this->ogre.draw();

        shader::use(text);

        return cobra::app_status::CONTINUE;
    }

    cobra::app_status keypress(cobra::renderer &renderer, const int key, cobra::key_event type) override {
        if (key == GLFW_KEY_ESCAPE)
            return cobra::app_status::BREAK;

        return cobra::app_status::CONTINUE;
    }
};

int main() {
    const cobra::window_spec spec {
        .fullscreen = false
    };

    cobra::renderer renderer { spec };
    renderer.run_app<test_app>();

    return 0;
}
