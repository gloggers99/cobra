#pragma once
#include <type_traits>
#include <GLFW/glfw3.h>

namespace cobra {
    class renderer;

    enum class app_status {
        CONTINUE,
        BREAK
    };

    enum class key_event {
        PRESS = GLFW_PRESS,
        RELEASE = GLFW_RELEASE
    };

    class app {
    public:
        explicit app(renderer &renderer);
        virtual ~app() = default;

        /**
         * This function will be called every
         * frame for your drawing purposes.
         * @param renderer A reference to the renderer.
         * @param delta_time Time between this frame and the last frame.
         * @return Should return the app's status.
         */
        [[nodiscard]] virtual app_status update(
            renderer &renderer,
            double delta_time
        );

        /**
         * This function wil be run every time a key is pressed.
         * @param renderer A reference to the renderer.
         * @param key The key pressed, comparable to GLFW_KEY_{key}
         * @param type The keypress type, pressed or released
         * @return Should return the app's status
         */
        [[nodiscard]] virtual app_status keypress(
            renderer &renderer,
            int key,
            key_event type
        );
    };

    template <typename T>
    concept app_type = std::is_base_of_v<app, T>;
}

