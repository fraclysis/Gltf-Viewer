#include "pch.h"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_opengl3.h"
#include "light_program.h"
#include "morph.h"
#include "renderer.h"
#include "viewer.h"
#include <codecvt>
#include <locale>

ImGuiKey libfrac_key_to_imgui(int t_key);

std::vector<char> read_to_vector(const WCHAR* path);

namespace viewer {

    extern morph_program g_tfp;

    bool g_show_warnings = false;
    std::vector<std::string> g_warn_messages;

    libfrac::error Viewer::init(libfrac::EventHandler<Viewer>* t_event_handler) noexcept {
        m_running = true;
        m_camera.m_step = m_camera.m_radius / 100.0f * m_camera_step_percent;

        libfrac::error err = 0;
        libfrac::WindowBuilder wb;
        if (err = libfrac::GlDisplayBuilder().build(t_event_handler, &wb, &m_window, &m_display)) {
            return err;
        }

        libfrac::GlContextBuilder cb;
        if (err = cb.build(m_display, &m_context)) {
            return err;
        }

        if (err = m_gpu_queue.init(m_display, m_context, m_window)) {
            log_lf(err);
            return err; // TODO use main thread if fails
        }

        unsigned int core_count = std::thread::hardware_concurrency();
        if (m_file_queue.init(std::max(core_count - 2, 1u), &m_gpu_queue) == 0) {
            return libfrac::error::message("Failed to initialize texture loader threads.");
        }

        if (err = m_display.make_current(m_context)) {
            return err;
        }

        GLenum glew_err;
        if (glew_err = glewInit() != GLEW_OK) {
            return libfrac::error::message((const char*)glewGetErrorString(glew_err));
        }

        if (!ImGui::CreateContext()) {
            return libfrac::error::message("Failed to initialize ImGui.");
        }

        RECT rect;
        ImGuiIO& io = ImGui::GetIO();
        m_window.client_rect(&rect);
        io.DisplaySize.x = rect.right - rect.left;
        io.DisplaySize.y = rect.bottom - rect.top;

        // ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\msyh.ttc", 16.0f, NULL,
        //                                             io.Fonts->GetGlyphRangesChineseFull());

        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
        if (!ImGui_ImplOpenGL3_Init()) {
            return libfrac::error::message("Failed to initialize ImGui OpenGl.");
        }
        if (!ImGui_ImplOpenGL3_CreateFontsTexture()) {
            return libfrac::error::message("Failed to create fonts texture.");
        }

        log_renderer_api_info();

        GL(glDebugMessageCallback(myOpenGLCallback, nullptr));
        GL(glClearColor(0.0f, 0.0f, 0.0f, 1.0f));
        GL(glEnable(GL_DEPTH_TEST));
        GL(glEnable(GL_CULL_FACE));
        GL(glEnable(GL_TEXTURE_2D));
        GL(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

        load_programs();

        g_tfp.load();

        m_running = true;

        const char* display_message;
        m_window.show();

        LONG res = ChangeDisplaySettingsW(NULL, CDS_FULLSCREEN);
        switch (res) {
        case DISP_CHANGE_SUCCESSFUL:
            display_message = "The settings change was successful.";
            break;
        case DISP_CHANGE_BADDUALVIEW:
            display_message = "The settings change was unsuccessful because the system is DualView capable.";
            break;
        case DISP_CHANGE_BADFLAGS:
            display_message = "An invalid set of flags was passed in.";
            break;
        case DISP_CHANGE_BADMODE:
            display_message = "The graphics mode is not supported.";
            break;
        case DISP_CHANGE_BADPARAM:
            display_message =
                "An invalid parameter was passed in.This can include an invalid flag or combination of flags.";
            break;
        case DISP_CHANGE_FAILED:
            display_message = "The display driver failed the specified graphics mode.";
            break;
        case DISP_CHANGE_NOTUPDATED:
            display_message = "Unable to write settings to the registry.";
            break;
        case DISP_CHANGE_RESTART:
            display_message = "The computer must be restarted for the graphics mode to work.";
            break;
        default:
            display_message = "";
            break;
        }

        log_info(display_message);

        redraw_times(3);

        return 0;
    }

    void Viewer::free() noexcept {
        m_gpu_queue.free();
        m_file_queue.free();
        g_tfp.free();
        m_gltf.free(m_file_queue, m_gpu_queue);

        ImGui_ImplOpenGL3_DestroyFontsTexture();
        ImGui_ImplOpenGL3_Shutdown();

        log_lf_c(m_display.make_current());
        log_lf_c(m_display.make_current());
        log_lf_c(m_context.destroy());
        log_lf_c(m_display.destroy());

        log_lf_c(m_window.destroy());
    }

    void Viewer::render() noexcept {
        GL(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

        if (m_animating) {
            if (m_gltf.m_gl_animations_size > m_target_animation)
                m_gltf.m_gl_animations[m_target_animation].update(1.0f / 165.0f, m_gltf);
        }

        if (m_gltf.m_scenes.size() > 0) {
            GL(glUseProgram(m_program.m_program));
            GL(glUniform3fv(m_program.lightColors, 4, &m_light_color[0][0]));
            GL(glUniform3fv(m_program.lightPositions, 4, &m_light_position[0][0]));
            GL(glUniform3f(m_program.directionalLight, m_directional_light_normalized.x,
                           m_directional_light_normalized.y, m_directional_light_normalized.z));

            m_camera.update();

            glm::mat4 view_projection = m_camera.m_projection * m_camera.m_view;
            m_gltf.update();
            m_gltf.render(m_program, view_projection, m_camera.m_eye, 0, 0);
        }

        ImGui::NewFrame();
        ImGui_ImplOpenGL3_NewFrame();

        render_gui();

        ImGui::render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        log_flush();
        log_lf_c(m_display.swap_buffers());

        load_gltf_if_requested();

        redraw_handler();
    }

    void Viewer::redraw_handler() {
        if (m_redraw_times > 0) {
            --m_redraw_times;
            redraw();
        }
    }

    void Viewer::load_gltf_if_requested() {
        if (m_open_dialog) {
            m_open_dialog = false;
            try {
                WCHAR path_name[MAX_PATH];

                OPENFILENAME ofn{0};
                ofn.lStructSize = sizeof(OPENFILENAME);
                ofn.lpstrFile = path_name;
                ofn.lpstrFile[0] = '\0';
                ofn.nMaxFile = MAX_PATH;
                ofn.lpstrFilter = L"glTF model files.\0*.GLTF\0\0";
                ofn.nFilterIndex = 1;
                ofn.lpstrTitle = L"Choose a file to view.";
                ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

                if (GetOpenFileName(&ofn)) {
                    std::filesystem::path p(path_name);
                    load_gltf_model(p.string().c_str());
                }

            } catch (const std::string& e) {
            } catch (...) {
            }
        }
    }

    void Viewer::render_gui() {
        if (g_show_warnings) {
            ImGui::Begin("Warnings", &g_show_warnings);

            for (size_t ii = g_warn_messages.size(); ii > 0; ii--) {
                ImGui::Text(g_warn_messages[ii - 1].c_str());
            }

            ImGui::End();
        }

        ImGui::SetNextWindowPos(ImVec2(16.0f, 16.0f));
        ImGui::SetNextWindowSize(ImVec2(270.0f, 480.0f), ImGuiCond_Appearing);
        ImGui::Begin("glTF Viewer", NULL, ImGuiWindowFlags_NoMove);

        m_gui_width = ImGui::GetWindowWidth();
        m_gui_height = ImGui::GetWindowHeight();

        if (ImGui::Button("Open glTF")) {
            m_open_dialog = true;
        }

        ImGui::Text("Model Name: %s", m_gltf.m_name.c_str());
        ImGui::Combo("Scene", &m_gltf.m_current_scene, m_gltf.m_scenes_names.get(), m_gltf.m_scenes.size());
        ImGui::Combo("Animation", &m_target_animation, m_gltf.m_animation_names.get(), m_gltf.m_gl_animations_size);
        ImGui::Checkbox("Animate", &m_animating);

        ImGui::Spacing();

        if (ImGui::CollapsingHeader("Animations")) {
            float a = 0.0f;

            if (m_gltf.m_gl_animations_size > m_target_animation) {
                gl_animation& animation = m_gltf.m_gl_animations[m_target_animation];
                a = animation.m_time / animation.m_animation_length;

                if (ImGui::SliderFloat("Time", &a, 0.0f, 1.0f)) {
                    animation.m_time = a * animation.m_animation_length;
                    animation.reset_animation();
                }
            } else {
                ImGui::SliderFloat("Time", &a, 0.0f, 1.0f);
            }
        }

        if (ImGui::CollapsingHeader("Asset")) {
            bool c = true;
            if (!m_gltf.m_asset.m_copyright.empty()) {
                ImGui::Text("Copyright: %s", m_gltf.m_asset.m_copyright.c_str());
                c = false;
            }

            if (!m_gltf.m_asset.m_generator.empty()) {
                ImGui::Text("Generator: %s", m_gltf.m_asset.m_generator.c_str());
                c = false;
            }

            if (!m_gltf.m_asset.m_version.empty()) {
                ImGui::Text("Version: %s", m_gltf.m_asset.m_version.c_str());
                c = false;
            }

            if (!m_gltf.m_asset.m_min_version.empty()) {
                ImGui::Text("Min Version: %s", m_gltf.m_asset.m_min_version.c_str());
                c = false;
            }

            if (c) {
                ImGui::Text("Empty :)");
            }
        }

        if (ImGui::CollapsingHeader("Cameras")) {
            if (m_gltf.m_cameras.size() == 0) {
                ImGui::Text("Empty :)");
            }
        }

        if (ImGui::CollapsingHeader("Images")) {
            if (m_gltf.m_textures.size() == 0) {
                ImGui::Text("Empty :)");
            }

            for (size_t ii = 0; ii < m_gltf.m_images.size(); ii++) {
                ImGui::BulletText("Name: %s", m_gltf.m_images[ii].m_name.c_str());
                if (ImGui::IsItemHovered()) {
                    ImGui::BeginTooltip();
                    ImGui::Image((ImTextureID)m_gltf.m_gl_images[ii + 2], ImVec2(400.0f, 400.0f)); // 2 is offset
                    ImGui::EndTooltip();
                }
            }
        }

        ImGui::Spacing();

        ImGui::Text("Renderer Version: %s", glGetString(GL_VERSION));
        ImGui::Spacing();

        if (ImGui::CollapsingHeader("Renderer")) {
            GLint samples = -1;
            // TODO cache it
            glGetIntegerv(GL_SAMPLES, &samples);
            ImGui::Text("Samples: %d", samples);
        }

        if (ImGui::CollapsingHeader("Controls")) {
            if (ImGui::InputFloat3("Directional Light", &m_directional_light.x)) {
                m_directional_light_normalized = glm::normalize(m_directional_light);
            }

            if (ImGui::InputFloat("Camera zoom step", &m_camera_step_percent, 1.0f, 10.0f)) {
                m_camera.m_step = m_camera.m_object_radius / 100.0f * m_camera_step_percent;
            }
            ImGui::Text("Camera Step: %f", m_camera.m_step);

            if (ImGui::InputFloat("Offset", &m_offset_sensitivity)) {
            }
        }

        ImGui::End();
    }

    void Viewer::load_programs() noexcept {
        try {
            program new_program = {};
            new_program.load(L"shaders/gltf.vert", L"shaders/gltf.frag");
            m_program.free();
            m_program = new_program;

        } catch (const std::string& err) {
            g_warn_messages.push_back(err);
            g_show_warnings = true;
        } catch (...) {
            g_warn_messages.push_back("Failed to load program.");
            g_show_warnings = true;
        }
    }

    void Viewer::resize(int t_width, int t_height, libfrac::Window t_window) noexcept {
        if (t_width * t_height) {
            ImGuiIO& io = ImGui::GetIO();

            GL(glViewport(0, 0, t_width, t_height));
            m_camera.perspective((float)t_width / (float)t_height);

            io.DisplaySize.x = t_width;
            io.DisplaySize.y = t_height;
        }
    }

    void Viewer::key_down(int t_vk_code, int t_scan_code, libfrac::Window t_window) noexcept {
        ImGuiIO& io = ImGui::GetIO();
        io.AddKeyEvent(libfrac_key_to_imgui(t_vk_code), true);

        redraw_times(3);
    }

    void Viewer::key_up(int t_vk_code, int t_scan_code, libfrac::Window t_window) noexcept {
        ImGuiIO& io = ImGui::GetIO();
        io.AddKeyEvent(libfrac_key_to_imgui(t_vk_code), false);

        if (t_vk_code == KEY_R) {
            load_programs();
        }

        redraw_times(3);
    }

    void Viewer::mouse_wheel(float t_dx, float t_dy, libfrac::Window t_window) noexcept {
        if ((16 < m_last_x && m_last_x < m_gui_width + 16) && (16 < m_last_y && m_last_y < m_gui_height + 16)) {
            ImGuiIO& io = ImGui::GetIO();
            io.AddMouseWheelEvent(t_dx, t_dy);

        } else {
            m_camera.zoom(t_dy);
        }

        redraw_times(3);
    }

    void Viewer::mouse_move(int t_x, int t_y, libfrac::Window t_window) noexcept {
        float dx = (float)t_x - m_last_x;
        float dy = (float)t_y - m_last_y;

        ImGuiIO& io = ImGui::GetIO();
        io.AddMousePosEvent(t_x, t_y);

        if (m_button_state[mouse_button_left]) {
            m_camera.control(dx * m_sensitivity, dy * m_sensitivity);
            m_camera.update();
        }

        if (m_button_state[mouse_button_middle]) {
            glm::vec3 d = m_camera.m_eye - m_camera.m_center;
            m_camera.m_center += glm::normalize(d) * dy * m_offset_sensitivity;
        }

        if (m_button_state[mouse_button_right]) {
            glm::vec3 d = m_camera.m_eye - m_camera.m_center;

            glm::vec3 w = glm::cross(-glm::normalize(d), glm::vec3(0.0f, 1.0f, 0.0f));
            glm::vec3 h = glm::cross(-glm::normalize(d), w);

            m_camera.m_center += (w * -dx * m_offset_sensitivity) + (h * -dy * m_offset_sensitivity);
        }

        m_last_x = (float)t_x;
        m_last_y = (float)t_y;

        redraw_times(3);
    }

    void Viewer::raw_mouse_delta(long t_dx, long t_dy, libfrac::Window t_window) noexcept {}

    void Viewer::mouse_button_down(int t_button, libfrac::Window t_window) noexcept {
        m_button_state[t_button] = true;
        if (!m_mouse_captured) {
#if _WIN32
            SetCapture(t_window.m_hWnd);
#endif
            m_mouse_captured = true;
        }

        ImGuiIO& io = ImGui::GetIO();
        io.AddMouseButtonEvent(t_button, true);

        redraw_times(3);
    }

    void Viewer::mouse_button_up(int t_button, libfrac::Window t_window) noexcept {
        ImGuiIO& io = ImGui::GetIO();
        io.AddMouseButtonEvent(t_button, false);
        m_button_state[t_button] = false;

        // TODO while cursor is captured pressing and releasing another mouse button causes capture to relase
        if (m_mouse_captured) {
#if _WIN32
            if (ReleaseCapture() == 0) {
                log_lf(libfrac::error(GetLastError()));
            }
#endif
            m_mouse_captured = false;
        }

        redraw_times(3);
    }

    void Viewer::character(int t_character, libfrac::Window t_window) noexcept {
        ImGuiIO& io = ImGui::GetIO();
        io.AddInputCharacterUTF16(t_character);
    }

    void Viewer::file_drop(std::filesystem::path t_path, libfrac::Window t_window) noexcept {
        load_gltf_model(t_path.string().c_str());
        redraw_times(3);
    }

    void Viewer::notify(libfrac::Window t_window) noexcept {
        log_info("Notified %p", (void*)t_window.m_hWnd);
        redraw();
    }

    void Viewer::focus(bool t_focus, libfrac::Window t_window) noexcept {
        ImGuiIO& io = ImGui::GetIO();
        io.AddFocusEvent(t_focus);
    }

    void Viewer::redraw_times(int t_times) {
        m_redraw_times = t_times - 1;
        redraw();
    }

    void Viewer::load_gltf_model(const char* t_model_path) noexcept {
        m_gltf.free(m_file_queue, m_gpu_queue);
        m_gltf.load_from_path(t_model_path);
        m_gltf.load_gl(m_file_queue);
        m_gltf.fit_camera(m_camera, m_camera_step_percent);
    }

    void Viewer::close(libfrac::Window t_window) noexcept { m_running = false; }

    void Viewer::paint(libfrac::Window t_window) noexcept {
        libfrac::Event::paint(t_window); // Validate window
        render();
    }

} // namespace viewer

ImGuiKey libfrac_key_to_imgui(int t_key) {
    switch (t_key) {
    case KEY_TAB:
        return ImGuiKey_Tab;
    case KEY_LEFTARROW:
        return ImGuiKey_LeftArrow;
    case KEY_RIGHTARROW:
        return ImGuiKey_RightArrow;
    case KEY_UPARROW:
        return ImGuiKey_UpArrow;
    case KEY_DOWNARROW:
        return ImGuiKey_DownArrow;
    case KEY_PAGEUP:
        return ImGuiKey_PageUp;
    case KEY_PAGEDOWN:
        return ImGuiKey_PageDown;
    case KEY_HOME:
        return ImGuiKey_Home;
    case KEY_END:
        return ImGuiKey_End;
    case KEY_INSERT:
        return ImGuiKey_Insert;
    case KEY_DELETE:
        return ImGuiKey_Delete;
    case KEY_BACKSPACE:
        return ImGuiKey_Backspace;
    case KEY_SPACE:
        return ImGuiKey_Space;
    case KEY_ENTER:
        return ImGuiKey_Enter;
    case KEY_ESCAPE:
        return ImGuiKey_Escape;
    case KEY_APOSTROPHE:
        return ImGuiKey_Apostrophe;
    case KEY_COMMA:
        return ImGuiKey_Comma;
    case KEY_MINUS:
        return ImGuiKey_Minus;
    case KEY_PERIOD:
        return ImGuiKey_Period;
    case KEY_SLASH:
        return ImGuiKey_Slash;
    case KEY_SEMICOLON:
        return ImGuiKey_Semicolon;
    case KEY_EQUAL:
        return ImGuiKey_Equal;
    case KEY_LEFTBRACKET:
        return ImGuiKey_LeftBracket;
    case KEY_BACKSLASH:
        return ImGuiKey_Backslash;
    case KEY_RIGHTBRACKET:
        return ImGuiKey_RightBracket;
    case KEY_GRAVEACCENT:
        return ImGuiKey_GraveAccent;
    case KEY_CAPSLOCK:
        return ImGuiKey_CapsLock;
    case KEY_SCROLLLOCK:
        return ImGuiKey_ScrollLock;
    case KEY_NUMLOCK:
        return ImGuiKey_NumLock;
    case KEY_PRINTSCREEN:
        return ImGuiKey_PrintScreen;
    case KEY_PAUSE:
        return ImGuiKey_Pause;
    case KEY_KEYPAD0:
        return ImGuiKey_Keypad0;
    case KEY_KEYPAD1:
        return ImGuiKey_Keypad1;
    case KEY_KEYPAD2:
        return ImGuiKey_Keypad2;
    case KEY_KEYPAD3:
        return ImGuiKey_Keypad3;
    case KEY_KEYPAD4:
        return ImGuiKey_Keypad4;
    case KEY_KEYPAD5:
        return ImGuiKey_Keypad5;
    case KEY_KEYPAD6:
        return ImGuiKey_Keypad6;
    case KEY_KEYPAD7:
        return ImGuiKey_Keypad7;
    case KEY_KEYPAD8:
        return ImGuiKey_Keypad8;
    case KEY_KEYPAD9:
        return ImGuiKey_Keypad9;
    case KEY_KEYPADDECIMAL:
        return ImGuiKey_KeypadDecimal;
    case KEY_KEYPADDIVIDE:
        return ImGuiKey_KeypadDivide;
    case KEY_KEYPADMULTIPLY:
        return ImGuiKey_KeypadMultiply;
    case KEY_KEYPADSUBTRACT:
        return ImGuiKey_KeypadSubtract;
    case KEY_KEYPADADD:
        return ImGuiKey_KeypadAdd;
        // case KEY_KEYPADENTER: return ImGuiKey_KeypadEnter;
    case KEY_LEFTSHIFT:
        return ImGuiKey_LeftShift;
    case KEY_LEFTCTRL:
        return ImGuiKey_LeftCtrl;
    case KEY_LEFTALT:
        return ImGuiKey_LeftAlt;
    case KEY_LEFTSUPER:
        return ImGuiKey_LeftSuper;
    case KEY_RIGHTSHIFT:
        return ImGuiKey_RightShift;
    case KEY_RIGHTCTRL:
        return ImGuiKey_RightCtrl;
    case KEY_RIGHTALT:
        return ImGuiKey_RightAlt;
    case KEY_RIGHTSUPER:
        return ImGuiKey_RightSuper;
    case KEY_MENU:
        return ImGuiKey_Menu;
    case KEY_0:
        return ImGuiKey_0;
    case KEY_1:
        return ImGuiKey_1;
    case KEY_2:
        return ImGuiKey_2;
    case KEY_3:
        return ImGuiKey_3;
    case KEY_4:
        return ImGuiKey_4;
    case KEY_5:
        return ImGuiKey_5;
    case KEY_6:
        return ImGuiKey_6;
    case KEY_7:
        return ImGuiKey_7;
    case KEY_8:
        return ImGuiKey_8;
    case KEY_9:
        return ImGuiKey_9;
    case KEY_A:
        return ImGuiKey_A;
    case KEY_B:
        return ImGuiKey_B;
    case KEY_C:
        return ImGuiKey_C;
    case KEY_D:
        return ImGuiKey_D;
    case KEY_E:
        return ImGuiKey_E;
    case KEY_F:
        return ImGuiKey_F;
    case KEY_G:
        return ImGuiKey_G;
    case KEY_H:
        return ImGuiKey_H;
    case KEY_I:
        return ImGuiKey_I;
    case KEY_J:
        return ImGuiKey_J;
    case KEY_K:
        return ImGuiKey_K;
    case KEY_L:
        return ImGuiKey_L;
    case KEY_M:
        return ImGuiKey_M;
    case KEY_N:
        return ImGuiKey_N;
    case KEY_O:
        return ImGuiKey_O;
    case KEY_P:
        return ImGuiKey_P;
    case KEY_Q:
        return ImGuiKey_Q;
    case KEY_R:
        return ImGuiKey_R;
    case KEY_S:
        return ImGuiKey_S;
    case KEY_T:
        return ImGuiKey_T;
    case KEY_U:
        return ImGuiKey_U;
    case KEY_V:
        return ImGuiKey_V;
    case KEY_W:
        return ImGuiKey_W;
    case KEY_X:
        return ImGuiKey_X;
    case KEY_Y:
        return ImGuiKey_Y;
    case KEY_Z:
        return ImGuiKey_Z;
    case KEY_F1:
        return ImGuiKey_F1;
    case KEY_F2:
        return ImGuiKey_F2;
    case KEY_F3:
        return ImGuiKey_F3;
    case KEY_F4:
        return ImGuiKey_F4;
    case KEY_F5:
        return ImGuiKey_F5;
    case KEY_F6:
        return ImGuiKey_F6;
    case KEY_F7:
        return ImGuiKey_F7;
    case KEY_F8:
        return ImGuiKey_F8;
    case KEY_F9:
        return ImGuiKey_F9;
    case KEY_F10:
        return ImGuiKey_F10;
    case KEY_F11:
        return ImGuiKey_F11;
    case KEY_F12:
        return ImGuiKey_F12;
    default:
        return ImGuiKey_None;
    }
}