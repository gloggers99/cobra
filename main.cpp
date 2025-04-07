#include "src/cobra.hpp"

#include <glad/gl.h>

#include <shaders/test.hcs>

class test_app final : public cobra::app {
    cobra::camera camera;
    cobra::model ogre;
public:
    explicit test_app(cobra::renderer &renderer)
        : app(renderer),
          camera(renderer),
          ogre("test.obj") {
        shader::compile(test);
        shader::use(test);
    }

    ~test_app() override = default;

    cobra::app_status update(cobra::renderer &renderer, const double delta_time) override {
        renderer.clear();
        renderer.clear_color(cobra::color(0.0f, 1.0f));

        this->camera.correlate(test.camera);

        this->ogre.draw();

        return cobra::app_status::CONTINUE;
    }

    cobra::app_status keypress(cobra::renderer &renderer, int key, cobra::key_event type) override {
        if (key == GLFW_KEY_ESCAPE)
            return cobra::app_status::BREAK;

        return cobra::app_status::CONTINUE;
    }
};

int main() {
    const cobra::window_spec spec {
        .fullscreen = true
    };

    cobra::renderer renderer { spec };
    renderer.run_app<test_app>();

    return 0;
}
