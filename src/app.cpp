#include "app.hpp"

namespace cobra {
    app::app(renderer &renderer) {}

    app_status app::update(renderer &renderer,
                           double delta_time) {
        return app_status::CONTINUE;
    }

    app_status app::keypress(renderer &renderer, int key, key_event type) {
        return app_status::CONTINUE;
    }
}
