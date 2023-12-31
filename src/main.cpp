#include "pch.h"

#include "viewer.h"

int main() {
    window::error err = 0;
    if (err = window::init_gl()) {
        log_lf(err);
        return err;
    }

    window::EventHandler<viewer::Viewer> event_handler;

    {
        viewer::Viewer viewer{};
        event_handler.m_user_data = &viewer;

        if (err = window::EventHandlerBuilder().build(&event_handler)) {
            log_lf(err);
            return err;
        }

        if (err = viewer.init(&event_handler)) {
            log_lf(err);
            viewer.m_running = false;
        }

        while (viewer.m_running) {
            if (viewer.m_animating) {
                while (event_handler.poll()) {
                    event_handler.dispatch();
                }

                viewer.render();
            } else {
                event_handler.wait();
                event_handler.dispatch();
            }
        }

        viewer.free();
    }
    
    if (err = event_handler.destroy()) {
        log_lf(err);
    }
}