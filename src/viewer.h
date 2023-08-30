#include "pch.h"

#ifndef VIEWER_VIEWER_H_
#define VIEWER_VIEWER_H_

#include "gltf/gltf.h"
#include "orbit_camera.h"
#include "program.h"
#include "queue.h"

enum render_mode : int {
    render_mode_none,
    render_mode_normals,
    render_mode_base_color,
    render_mode_metalic,
    render_mode_count,
};

namespace viewer {

    struct Viewer : libfrac::Event {
        bool m_running = false;
        bool m_animating = false;
        bool m_open_dialog = false;

        bool m_mouse_captured = false;
        bool m_button_state[mouse_button_count];

        float m_camera_step_percent = 40.0f;
        float m_sensitivity = 0.01f;
        float m_offset_sensitivity = 0.01f;
        int m_redraw_times = 0;

        float m_gui_width;
        float m_gui_height;

        libfrac::GlContext m_context;
        libfrac::GlDisplay m_display;
        libfrac::Window m_window;

        FileTextureQueue m_file_queue;
        GpuTextureQueue m_gpu_queue;

        program m_program;
        program m_shadow_map_program;
        orbit_camera m_camera;

        int m_target_animation = 0;
        gltf m_gltf;

        float m_last_x = 0.0f;
        float m_last_y = 0.0f;

        glm::vec3 m_light_position[4] = {glm::vec3(1.0f)};
        glm::vec3 m_light_color[4] = {glm::vec3(0.0f)};
        glm::vec3 m_directional_light = glm::vec3(10.0f, 10.0f, 10.0f);
        glm::vec3 m_directional_light_normalized = glm::normalize(glm::vec3(10.0f, 10.0f, 10.0f));

        void load_gltf_model(const char* t_model_path) noexcept;

        libfrac::error init(libfrac::EventHandler<Viewer>* t_event_handler) noexcept;
        void free() noexcept;

        void render_gui();
        void render() noexcept;
        void redraw_handler();
        void load_gltf_if_requested();
        void paint(libfrac::Window t_window) noexcept;
        void close(libfrac::Window t_window) noexcept;
        void load_programs() noexcept;
        void resize(int t_width, int t_height, libfrac::Window t_window) noexcept;

        void key_down(int t_vk_code, int t_scan_code, libfrac::Window t_window) noexcept;
        void key_up(int t_vk_code, int t_scan_code, libfrac::Window t_window) noexcept;

        void mouse_wheel(float t_dx, float t_dy, libfrac::Window t_window) noexcept;
        void mouse_move(int t_x, int t_y, libfrac::Window t_window) noexcept;
        void raw_mouse_delta(long t_dx, long t_dy, libfrac::Window t_window) noexcept;

        void mouse_button_down(int t_button, libfrac::Window t_window) noexcept;
        void mouse_button_up(int t_button, libfrac::Window t_window) noexcept;

        void character(int t_character, libfrac::Window t_window) noexcept;
        void file_drop(std::filesystem::path t_path, libfrac::Window t_window) noexcept;

        void notify(libfrac::Window t_window) noexcept;

        inline bool is_valid() noexcept { return m_running; }

        void focus(bool t_focus, libfrac::Window t_window) noexcept;

        inline void redraw() {
            if (!m_open_dialog)
                if (!m_animating)
                    m_window.redraw();
        }

        void redraw_times(int t_times);
    };

} // namespace viewer

#endif // !VIEWER_VIEWER_H_
