// libfrac.hpp v0.1
// Notation:
//    t - function in
//    r - function return
//    m - member

#ifndef LIB_FRAC_HPP_
#define LIB_FRAC_HPP_

#ifdef _WIN32
#define NOMINMAX
#include <Windows.h>
#include <Windowsx.h>
#include <hidusage.h>

#include <filesystem>
#include <string>

#define LFNODISCARD [[nodiscard]]

#define LIB_FRAC_GL
#ifdef LIB_FRAC_GL
#include <gl/gl.h>

#define WGL_CONTEXT_MAJOR_VERSION_ARB 0x2091
#define WGL_CONTEXT_MINOR_VERSION_ARB 0x2092
#define WGL_CONTEXT_LAYER_PLANE_ARB 0x2093
#define WGL_CONTEXT_FLAGS_ARB 0x2094
#define WGL_CONTEXT_PROFILE_MASK_ARB 0x9126

#define WGL_CONTEXT_DEBUG_BIT_ARB 0x0001
#define WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB 0x0002

#define WGL_CONTEXT_CORE_PROFILE_BIT_ARB 0x00000001
#define WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB 0x00000002

#define WGL_NUMBER_PIXEL_FORMATS_ARB 0x2000
#define WGL_DRAW_TO_WINDOW_ARB 0x2001
#define WGL_DRAW_TO_BITMAP_ARB 0x2002
#define WGL_ACCELERATION_ARB 0x2003
#define WGL_NEED_PALETTE_ARB 0x2004
#define WGL_NEED_SYSTEM_PALETTE_ARB 0x2005
#define WGL_SWAP_LAYER_BUFFERS_ARB 0x2006
#define WGL_SWAP_METHOD_ARB 0x2007
#define WGL_NUMBER_OVERLAYS_ARB 0x2008
#define WGL_NUMBER_UNDERLAYS_ARB 0x2009
#define WGL_TRANSPARENT_ARB 0x200A
#define WGL_TRANSPARENT_RED_VALUE_ARB 0x2037
#define WGL_TRANSPARENT_GREEN_VALUE_ARB 0x2038
#define WGL_TRANSPARENT_BLUE_VALUE_ARB 0x2039
#define WGL_TRANSPARENT_ALPHA_VALUE_ARB 0x203A
#define WGL_TRANSPARENT_INDEX_VALUE_ARB 0x203B
#define WGL_SHARE_DEPTH_ARB 0x200C
#define WGL_SHARE_STENCIL_ARB 0x200D
#define WGL_SHARE_ACCUM_ARB 0x200E
#define WGL_SUPPORT_GDI_ARB 0x200F
#define WGL_SUPPORT_OPENGL_ARB 0x2010
#define WGL_DOUBLE_BUFFER_ARB 0x2011
#define WGL_STEREO_ARB 0x2012
#define WGL_PIXEL_TYPE_ARB 0x2013
#define WGL_COLOR_BITS_ARB 0x2014
#define WGL_RED_BITS_ARB 0x2015
#define WGL_RED_SHIFT_ARB 0x2016
#define WGL_GREEN_BITS_ARB 0x2017
#define WGL_GREEN_SHIFT_ARB 0x2018
#define WGL_BLUE_BITS_ARB 0x2019
#define WGL_BLUE_SHIFT_ARB 0x201A
#define WGL_ALPHA_BITS_ARB 0x201B
#define WGL_ALPHA_SHIFT_ARB 0x201C
#define WGL_ACCUM_BITS_ARB 0x201D
#define WGL_ACCUM_RED_BITS_ARB 0x201E
#define WGL_ACCUM_GREEN_BITS_ARB 0x201F
#define WGL_ACCUM_BLUE_BITS_ARB 0x2020
#define WGL_ACCUM_ALPHA_BITS_ARB 0x2021
#define WGL_DEPTH_BITS_ARB 0x2022
#define WGL_STENCIL_BITS_ARB 0x2023
#define WGL_AUX_BUFFERS_ARB 0x2024

#define WGL_NO_ACCELERATION_ARB 0x2025
#define WGL_GENERIC_ACCELERATION_ARB 0x2026
#define WGL_FULL_ACCELERATION_ARB 0x2027

#define WGL_SWAP_EXCHANGE_ARB 0x2028
#define WGL_SWAP_COPY_ARB 0x2029
#define WGL_SWAP_UNDEFINED_ARB 0x202A

#define WGL_TYPE_RGBA_ARB 0x202B
#define WGL_TYPE_COLORINDEX_ARB 0x202C
#endif

#ifndef LIB_FRAC_NO_KEYS
#define KEY_TAB VK_TAB
#define KEY_LEFTARROW VK_LEFT
#define KEY_RIGHTARROW VK_RIGHT
#define KEY_UPARROW VK_UP
#define KEY_DOWNARROW VK_DOWN
#define KEY_PAGEUP VK_PRIOR
#define KEY_PAGEDOWN VK_NEXT
#define KEY_HOME VK_HOME
#define KEY_END VK_END
#define KEY_INSERT VK_INSERT
#define KEY_DELETE VK_DELETE
#define KEY_BACKSPACE VK_BACK
#define KEY_SPACE VK_SPACE
#define KEY_ENTER VK_RETURN
#define KEY_ESCAPE VK_ESCAPE
#define KEY_APOSTROPHE VK_OEM_7
#define KEY_COMMA VK_OEM_COMMA
#define KEY_MINUS VK_OEM_MINUS
#define KEY_PERIOD VK_OEM_PERIOD
#define KEY_SLASH VK_OEM_2
#define KEY_SEMICOLON VK_OEM_1
#define KEY_EQUAL VK_OEM_PLUS
#define KEY_LEFTBRACKET VK_OEM_4
#define KEY_BACKSLASH VK_OEM_5
#define KEY_RIGHTBRACKET VK_OEM_6
#define KEY_GRAVEACCENT VK_OEM_3
#define KEY_CAPSLOCK VK_CAPITAL
#define KEY_SCROLLLOCK VK_SCROLL
#define KEY_NUMLOCK VK_NUMLOCK
#define KEY_PRINTSCREEN VK_SNAPSHOT
#define KEY_PAUSE VK_PAUSE
#define KEY_KEYPAD0 VK_NUMPAD0
#define KEY_KEYPAD1 VK_NUMPAD1
#define KEY_KEYPAD2 VK_NUMPAD2
#define KEY_KEYPAD3 VK_NUMPAD3
#define KEY_KEYPAD4 VK_NUMPAD4
#define KEY_KEYPAD5 VK_NUMPAD5
#define KEY_KEYPAD6 VK_NUMPAD6
#define KEY_KEYPAD7 VK_NUMPAD7
#define KEY_KEYPAD8 VK_NUMPAD8
#define KEY_KEYPAD9 VK_NUMPAD9
#define KEY_KEYPADDECIMAL VK_DECIMAL
#define KEY_KEYPADDIVIDE VK_DIVIDE
#define KEY_KEYPADMULTIPLY VK_MULTIPLY
#define KEY_KEYPADSUBTRACT VK_SUBTRACT
#define KEY_KEYPADADD VK_ADD
#define KEY_KEYPADENTER VK_RETURN
#define KEY_LEFTSHIFT VK_LSHIFT
#define KEY_LEFTCTRL VK_LCONTROL
#define KEY_LEFTALT VK_LMENU
#define KEY_LEFTSUPER VK_LWIN
#define KEY_RIGHTSHIFT VK_RSHIFT
#define KEY_RIGHTCTRL VK_RCONTROL
#define KEY_RIGHTALT VK_RMENU
#define KEY_RIGHTSUPER VK_RWIN
#define KEY_MENU VK_APPS
#define KEY_0 '0'
#define KEY_1 '1'
#define KEY_2 '2'
#define KEY_3 '3'
#define KEY_4 '4'
#define KEY_5 '5'
#define KEY_6 '6'
#define KEY_7 '7'
#define KEY_8 '8'
#define KEY_9 '9'
#define KEY_A 'A'
#define KEY_B 'B'
#define KEY_C 'C'
#define KEY_D 'D'
#define KEY_E 'E'
#define KEY_F 'F'
#define KEY_G 'G'
#define KEY_H 'H'
#define KEY_I 'I'
#define KEY_J 'J'
#define KEY_K 'K'
#define KEY_L 'L'
#define KEY_M 'M'
#define KEY_N 'N'
#define KEY_O 'O'
#define KEY_P 'P'
#define KEY_Q 'Q'
#define KEY_R 'R'
#define KEY_S 'S'
#define KEY_T 'T'
#define KEY_U 'U'
#define KEY_V 'V'
#define KEY_W 'W'
#define KEY_X 'X'
#define KEY_Y 'Y'
#define KEY_Z 'Z'
#define KEY_F1 VK_F1
#define KEY_F2 VK_F2
#define KEY_F3 VK_F3
#define KEY_F4 VK_F4
#define KEY_F5 VK_F5
#define KEY_F6 VK_F6
#define KEY_F7 VK_F7
#define KEY_F8 VK_F8
#define KEY_F9 VK_F9
#define KEY_F10 VK_F10
#define KEY_F11 VK_F11
#define KEY_F12 VK_F12
#endif

enum mouse_button_ {
    mouse_button_left,
    mouse_button_right,
    mouse_button_middle,
    mouse_button_x1,
    mouse_button_x2,
    mouse_button_count,
};

namespace libfrac {

#ifdef LIB_FRAC_GL
    typedef HGLRC WINAPI wglCreateContextAttribsARB_type(HDC hdc, HGLRC hShareContext, const int* attribList);
    extern wglCreateContextAttribsARB_type* wglCreateContextAttribsARB;

    typedef BOOL WINAPI wglChoosePixelFormatARB_type(HDC hdc, const int* piAttribIList, const FLOAT* pfAttribFList,
                                                     UINT nMaxFormats, int* piFormats, UINT* nNumFormats);
    extern wglChoosePixelFormatARB_type* wglChoosePixelFormatARB;
#endif

    struct LFNODISCARD error {
        DWORD m_error_code;
        const char* m_message = nullptr;

        error(DWORD t_code);
        static libfrac::error message(const char* t_message);

        operator DWORD() const { return m_error_code; }

        std::string get_string() noexcept;
    };

    struct GlPFN {};

    extern GlPFN g_gl_pfn;

    error init_gl();

    template <typename T> LRESULT CALLBACK libfrac_WndProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam) noexcept;

    struct Window {
        HWND m_hWnd = NULL;
        bool show() noexcept;
        bool redraw() noexcept;
        error destroy() noexcept;
        error notify() noexcept;
        error client_rect(RECT* r_rect) noexcept;
#ifdef VK_USE_PLATFORM_WIN32_KHR
        inline VkResult create_surface(VkInstance t_instance, VkSurfaceKHR* r_surface) noexcept {
            VkWin32SurfaceCreateInfoKHR create_info{.sType{VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR}};
            create_info.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
            create_info.hinstance = GetModuleHandleW(nullptr);
            create_info.hwnd = m_hWnd;
            create_info.pNext = NULL;
            create_info.flags = 0;

            return vkCreateWin32SurfaceKHR(t_instance, &create_info, nullptr, r_surface);
        }
#endif
    };

    struct GlContext {
        HGLRC m_glrc = NULL;
        error destroy();
    };

    struct GlDisplay {
        HDC m_dc = NULL;
        HWND m_hWnd = NULL;
        error destroy();
        error make_current(GlContext t_context = {0});
        error swap_buffers();
    };

    template <typename T> struct EventHandler {
        T* m_user_data;
        MSG m_msg;
        ATOM m_window_class;

        LFNODISCARD error destroy() noexcept {
            if (m_window_class == NULL)
                return 0;
            error err = 0;

            if (UnregisterClassW(MAKEINTATOM(m_window_class), nullptr) == 0) {
                err = GetLastError();
            }

            m_window_class = NULL;

            return err;
        }
        int wait() noexcept { return GetMessageW(&m_msg, NULL, NULL, NULL); }
        LFNODISCARD int poll() noexcept { return PeekMessageW(&m_msg, NULL, NULL, NULL, PM_REMOVE); }
        void dispatch() noexcept {
            TranslateMessage(&m_msg);
            DispatchMessageW(&m_msg);
        }
    };

    struct EventHandlerBuilder {
        HMODULE m_hInstance = GetModuleHandleW(0);
        HCURSOR m_hCursor = LoadCursorW(0, IDC_ARROW);
        LPCWSTR m_lpszClassName = L"Hello";
        UINT m_style = CS_OWNDC | CS_VREDRAW | CS_HREDRAW;

        template <typename T> error build(EventHandler<T>* r_event_handler) noexcept {
            WNDCLASSEXW wc{0};
            wc.cbSize = sizeof(wc);
            wc.style = m_style;
            wc.lpfnWndProc = libfrac_WndProc<T>;
            wc.hInstance = m_hInstance;
            wc.hCursor = m_hCursor;
            wc.lpszClassName = m_lpszClassName;

            ATOM window_class_atom = RegisterClassExW(&wc);
            if (window_class_atom == NULL) {
                r_event_handler->m_window_class = NULL;
                return GetLastError();
            }

            r_event_handler->m_window_class = window_class_atom;
            return 0;
        }
    };

    struct WindowBuilder {
        DWORD m_style = WS_OVERLAPPEDWINDOW;
        DWORD m_ex_style = WS_EX_ACCEPTFILES;
        LPCWSTR m_window_name = L"Simple Window";
        int m_x = CW_USEDEFAULT;
        int m_y = CW_USEDEFAULT;
        int m_width = CW_USEDEFAULT;
        int m_height = CW_USEDEFAULT;
        HWND m_parent = NULL;
        HMENU m_menu = NULL;
        HMODULE m_instance = GetModuleHandle(nullptr);
        LPVOID m_param = nullptr;

        template <typename T> LFNODISCARD error build(EventHandler<T>* t_event_handler, Window* r_window) noexcept {
            r_window->m_hWnd =
                CreateWindowExW(m_ex_style, MAKEINTATOM(t_event_handler->m_window_class), m_window_name, m_style, m_x,
                                m_y, m_width, m_height, m_parent, m_menu, m_instance, m_param);

            if (r_window->m_hWnd == 0) {
                return GetLastError();
            }

            SetLastError(0);
            if (SetWindowLongPtrW(r_window->m_hWnd, GWLP_USERDATA, (LONG_PTR)(void*)t_event_handler) == 0) {
                if (GetLastError() != 0) {
                    // TODO handle Destroy window
                    // DestroyWindow(r_window->m_window_handle); // Handle it too
                    return GetLastError();
                }
            }

            RAWINPUTDEVICE rw[] = {
                {HID_USAGE_PAGE_GENERIC, HID_USAGE_GENERIC_MOUSE, NULL, r_window->m_hWnd},
            };

            if (RegisterRawInputDevices(rw, 1, sizeof(RAWINPUTDEVICE)) == 0) {
                // Log warning
                __debugbreak();
            }

            return 0;
        }
    };

    struct GlContextBuilder {
        int m_major_version = 3;
        int m_minor_version = 0;
        error build(GlDisplay t_display, GlContext* r_context, GlContext t_share_context = {0}) noexcept;
    };

    struct Event {
        void mouse_move(int t_x, int t_y, libfrac::Window t_window) noexcept;
        void mouse_wheel(float t_dx, float t_dy, libfrac::Window t_window) noexcept;
        void mouse_button_down(int t_button, libfrac::Window t_window) noexcept;
        void mouse_button_up(int t_button, libfrac::Window t_window) noexcept;
        void key_down(int t_vk_code, int t_scan_code, libfrac::Window t_window) noexcept;
        void key_up(int t_vk_code, int t_scan_code, libfrac::Window t_window) noexcept;
        void close(libfrac::Window t_window) noexcept;
        void destroy(libfrac::Window t_window) noexcept;
        void paint(libfrac::Window t_window) noexcept;
        void resize(int t_width, int t_height, libfrac::Window t_window) noexcept;
        void file_drop(std::filesystem::path t_path, libfrac::Window t_window) noexcept;
        inline bool is_valid() noexcept { return true; }
        void notify(libfrac::Window t_window) noexcept;
        void character(int t_character, libfrac::Window t_window) noexcept;
        void focus(bool t_focus, libfrac::Window t_window) noexcept;

        void raw_mouse_delta(long t_dx, long t_dy, libfrac::Window t_window) noexcept;
    };

    struct GlDisplayBuilder {
        template <typename T>
        error build(EventHandler<T>* t_event_handler, WindowBuilder* t_window_builder, Window* r_window,
                    GlDisplay* r_display) noexcept;
    };

    template <typename T>
    error GlDisplayBuilder::build(EventHandler<T>* t_event_handler, WindowBuilder* t_window_builder, Window* r_window,
                                  GlDisplay* r_display) noexcept {
        error err = 0;
        if (err = t_window_builder->build(t_event_handler, r_window)) {
            return err;
        }

        HDC dc = GetDC(r_window->m_hWnd);
        if (dc == 0) {
            return error::message("GetDc failed.");
        }

        // clang-format off
		int pixel_attrib[] = {
			WGL_DRAW_TO_WINDOW_ARB,     GL_TRUE,
			WGL_SUPPORT_OPENGL_ARB,     GL_TRUE,
			WGL_DOUBLE_BUFFER_ARB,      GL_TRUE,
			WGL_ACCELERATION_ARB,       WGL_FULL_ACCELERATION_ARB,
			WGL_PIXEL_TYPE_ARB,         WGL_TYPE_RGBA_ARB,
			WGL_COLOR_BITS_ARB,         32,
			WGL_DEPTH_BITS_ARB,         24,
			WGL_STENCIL_BITS_ARB,       8,
			0
		};
        // clang-format on

        int pixel_format;
        UINT num_format;

        if (wglChoosePixelFormatARB(dc, pixel_attrib, NULL, 1, &pixel_format, &num_format) == 0) {
            return GetLastError();
        }

        if (num_format == 0) {
            return error::message("num_format == 0.");
        }

        PIXELFORMATDESCRIPTOR pfd;
        if (DescribePixelFormat(dc, pixel_format, sizeof(pfd), &pfd) == 0) {
            return GetLastError();
        }

        if (SetPixelFormat(dc, pixel_format, &pfd) == 0) {
            return GetLastError();
        }

        r_display->m_dc = dc;
        r_display->m_hWnd = r_window->m_hWnd;

        return 0;
    }

    template <typename T> LRESULT CALLBACK libfrac_WndProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam) noexcept {
        if (hWnd == 0) {
            return DefWindowProcW(hWnd, Msg, wParam, lParam);
        }

        EventHandler<T>* event_handler = (EventHandler<T>*)GetWindowLongPtrW(hWnd, GWLP_USERDATA);
        if (event_handler == nullptr) {
            return DefWindowProcW(hWnd, Msg, wParam, lParam);
        }

        T* user_data = event_handler->m_user_data;

        if (user_data->is_valid() == false) {
            return DefWindowProcW(hWnd, Msg, wParam, lParam);
        }

        switch (Msg) {
        case WM_INPUT: {
            HRAWINPUT hRawInput = (HRAWINPUT)lParam;
            RAWINPUT raw_input{};
            UINT raw_input_size = sizeof(RAWINPUT);
            UINT header_size = sizeof(RAWINPUTHEADER);

            UINT status = GetRawInputData(hRawInput, RID_INPUT, &raw_input, &raw_input_size, header_size);
            if (status == ~0 || status == 0) {
                break;
            }

            switch (raw_input.header.dwType) {
            case RIM_TYPEMOUSE:
                RAWMOUSE raw_mouse = raw_input.data.mouse;
                if ((raw_mouse.usFlags & MOUSE_MOVE_RELATIVE) == MOUSE_MOVE_RELATIVE) {
                    user_data->raw_mouse_delta(raw_mouse.lLastX, raw_mouse.lLastY, {hWnd});
                }

                break;
            default:
                break;
            }

            int input_code = GET_RAWINPUT_CODE_WPARAM(wParam);
            if (input_code == RIM_INPUT) {
            }

            if (input_code == RIM_INPUTSINK) {
            }
        } break;

        case WM_MOUSEMOVE:
            user_data->mouse_move(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), {hWnd});
            return 0;

        case WM_LBUTTONDOWN:
            user_data->mouse_button_down(mouse_button_left, {hWnd});
            return 0;

        case WM_LBUTTONUP:
            user_data->mouse_button_up(mouse_button_left, {hWnd});
            return 0;

        case WM_RBUTTONDOWN:
            user_data->mouse_button_down(mouse_button_right, {hWnd});
            return 0;

        case WM_RBUTTONUP:
            user_data->mouse_button_up(mouse_button_right, {hWnd});
            return 0;

        case WM_MBUTTONDOWN:
            user_data->mouse_button_down(mouse_button_middle, {hWnd});
            return 0;

        case WM_MBUTTONUP:
            user_data->mouse_button_up(mouse_button_middle, {hWnd});
            return 0;

        case WM_XBUTTONDOWN:
            user_data->mouse_button_down(mouse_button_x1 - 1 + HIWORD(wParam), {hWnd});
            return 0;

        case WM_XBUTTONUP:
            user_data->mouse_button_up(mouse_button_x1 - 1 + HIWORD(wParam), {hWnd});
            return 0;

        case WM_SYSKEYUP:
        case WM_SYSKEYDOWN:
        case WM_KEYUP:
        case WM_KEYDOWN: {
            WORD vkCode = LOWORD(wParam); // virtual-key code
            WORD keyFlags = HIWORD(lParam);
            WORD scanCode = LOBYTE(keyFlags); // scan code
            BOOL isExtendedKey =
                (keyFlags & KF_EXTENDED) == KF_EXTENDED; // extended-key flag, 1 if scancode has 0xE0 prefix
            if (isExtendedKey)
                scanCode = MAKEWORD(scanCode, 0xE0);
            BOOL wasKeyDown = (keyFlags & KF_REPEAT) == KF_REPEAT; // previous key-state flag, 1 on autorepeat
            WORD repeatCount =
                LOWORD(lParam); // repeat count, > 0 if several keydown messages was combined into one message
            BOOL isKeyReleased = (keyFlags & KF_UP) == KF_UP; // transition-state flag, 1 on keyup
            // if we want to distinguish these keys:
            switch (vkCode) {
            case VK_SHIFT:   // converts to VK_LSHIFT or VK_RSHIFT
            case VK_CONTROL: // converts to VK_LCONTROL or VK_RCONTROL
            case VK_MENU:    // converts to VK_LMENU or VK_RMENU
                vkCode = LOWORD(MapVirtualKeyW(scanCode, MAPVK_VSC_TO_VK_EX));
                break;
            }

            switch (Msg) {
            case WM_KEYDOWN:
                user_data->key_down(vkCode, scanCode, {hWnd});
                return 0;
            case WM_KEYUP:
                user_data->key_up(vkCode, scanCode, {hWnd});
                return 0;
            case WM_SYSKEYDOWN:
                user_data->key_down(vkCode, scanCode, {hWnd});
                break;
            case WM_SYSKEYUP:
                user_data->key_up(vkCode, scanCode, {hWnd});
                break;
            }
        }
            return 0;

        case WM_CHAR:
            // Skip surrogate pairs
            if ((IS_HIGH_SURROGATE(wParam) || IS_LOW_SURROGATE(wParam))) {
                return 0;
            }
            user_data->character(wParam, {hWnd});

            return 0;

        case WM_MOUSEWHEEL: {
            short md = HIWORD(wParam);
            user_data->mouse_wheel(0.0f, (float)(md / WHEEL_DELTA), {hWnd});
        }
            return 0;

        case WM_PAINT:
            user_data->paint({hWnd});
            return 0;

        case WM_ERASEBKGND:
            return 1;

        case WM_SIZE:
            user_data->resize(LOWORD(lParam), HIWORD(lParam), {hWnd});
            return 0;

        case WM_SETFOCUS:
            user_data->focus(true, {hWnd});
            return 0;

        case WM_KILLFOCUS:
            user_data->focus(false, {hWnd});
            return 0;

        case WM_CLOSE:
            user_data->close({hWnd});
            return 0;

        case WM_DESTROY:
            user_data->destroy({hWnd});
            return 0;

        case WM_DROPFILES: {
            HDROP hDrop = (HDROP)wParam;
            UINT count = DragQueryFileW(hDrop, 0xFFFFFFFF, 0, 0);

            for (UINT ii = 0; ii < count; ii++) {
                UINT file_name_size = DragQueryFileW(hDrop, ii, 0, 0) + 1;

                if (file_name_size > MAX_PATH) {
                    // TODO heap allocation
                    __debugbreak();
                } else {
                    WCHAR buffer[MAX_PATH];

                    if (DragQueryFileW(hDrop, ii, buffer, file_name_size) == 0) {
                        // ERR
                        __debugbreak();
                    } else {
                        try {
                            std::filesystem::path a(buffer);
                            user_data->file_drop(a, {hWnd});
                        } catch (...) {
                            // ??
                            __debugbreak();
                        }
                    }
                }
            }
        }
            return 0;

        case WM_APP:
            user_data->notify({hWnd});
            return 0;
        }

        return DefWindowProcW(hWnd, Msg, wParam, lParam);
    }

} // namespace libfrac

#ifdef LIB_FRAC_IMPL
#undef LIB_FRAC_IMPL

#include <system_error>

namespace libfrac {
    GlPFN g_gl_pfn;

    typedef HGLRC WINAPI wglCreateContextAttribsARB_type(HDC hdc, HGLRC hShareContext, const int* attribList);
    wglCreateContextAttribsARB_type* wglCreateContextAttribsARB;

    typedef BOOL WINAPI wglChoosePixelFormatARB_type(HDC hdc, const int* piAttribIList, const FLOAT* pfAttribFList,
                                                     UINT nMaxFormats, int* piFormats, UINT* nNumFormats);
    wglChoosePixelFormatARB_type* wglChoosePixelFormatARB;

    // TODO make cleanup more better
    error init_gl() {
        WNDCLASSW wc{};
        wc.style = CS_OWNDC;
        wc.lpfnWndProc = DefWindowProcW;
        wc.hInstance = GetModuleHandleW(0);
        wc.lpszClassName = L"DummyWgl";

        if (!RegisterClassW(&wc)) {
            return GetLastError();
        }

        HWND dummy_window = CreateWindowExW(0, wc.lpszClassName, L"DummyWindow", 0, CW_USEDEFAULT, CW_USEDEFAULT,
                                            CW_USEDEFAULT, CW_USEDEFAULT, 0, 0, wc.hInstance, 0);

        if (dummy_window == NULL) {
            DWORD err = GetLastError();
            UnregisterClassW(wc.lpszClassName, 0);
            return err;
        }

        HDC dummy_dc = GetDC(dummy_window);

        PIXELFORMATDESCRIPTOR pfd{};
        pfd.nSize = sizeof(pfd);
        pfd.nVersion = 1;
        pfd.iPixelType = PFD_TYPE_RGBA;
        pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
        pfd.cColorBits = 32;
        pfd.cAlphaBits = 8;
        pfd.iLayerType = PFD_MAIN_PLANE;
        pfd.cDepthBits = 24;
        pfd.cStencilBits = 8;

        int pixel_format = ChoosePixelFormat(dummy_dc, &pfd);
        if (!pixel_format) {
            DWORD err = GetLastError();
            return err;
        }

        if (!SetPixelFormat(dummy_dc, pixel_format, &pfd)) {
            DWORD err = GetLastError();
            return err;
        }

        HGLRC dummy_context = wglCreateContext(dummy_dc);
        if (!dummy_context) {
            DWORD err = GetLastError();
            return err;
        }

        if (!wglMakeCurrent(dummy_dc, dummy_context)) {
            DWORD err = GetLastError();
            return err;
        }

        wglCreateContextAttribsARB = (wglCreateContextAttribsARB_type*)wglGetProcAddress("wglCreateContextAttribsARB");
        wglChoosePixelFormatARB = (wglChoosePixelFormatARB_type*)wglGetProcAddress("wglChoosePixelFormatARB");

        wglMakeCurrent(dummy_dc, 0);
        wglDeleteContext(dummy_context);
        ReleaseDC(dummy_window, dummy_dc);
        DestroyWindow(dummy_window);
        UnregisterClassW(wc.lpszClassName, 0);
        return 0;
    }

    void Event::mouse_move(int t_x, int t_y, Window t_window) noexcept {
        (void)(t_x);
        (void)(t_y);
        (void)(t_window);
    }

    void Event::focus(bool t_focus, libfrac::Window t_window) noexcept {
        (void)(t_focus);
        (void)(t_window);
    }

    void Event::raw_mouse_delta(long t_dx, long t_dy, libfrac::Window t_window) noexcept {
        (void)(t_dx);
        (void)(t_dy);
        (void)(t_window);
    }

    void Event::mouse_wheel(float t_dx, float t_dy, libfrac::Window t_window) noexcept {
        (void)(t_dx);
        (void)(t_dy);
        (void)(t_window);
    }

    void Event::mouse_button_down(int t_button, libfrac::Window t_window) noexcept {
        (void)(t_button);
        (void)(t_window);
    }
    void Event::mouse_button_up(int t_button, libfrac::Window t_window) noexcept {
        (void)(t_button);
        (void)(t_window);
    }

    void Event::key_down(int t_vk_code, int t_scan_code, Window t_window) noexcept {
        (void)(t_vk_code);
        (void)(t_scan_code);
        (void)(t_window);
    }

    void Event::character(int t_character, libfrac::Window t_window) noexcept {
        (void)(t_character);
        (void)(t_window);
    }

    void Event::close(Window t_window) noexcept { (void)(t_window); }

    void Event::destroy(Window t_window) noexcept { (void)(t_window); }

    void Event::notify(Window t_window) noexcept { (void)(t_window); }

    void Event::resize(int t_width, int t_height, libfrac::Window t_window) noexcept {
        (void)(t_width);
        (void)(t_height);
        (void)(t_window);
    }

    void Event::file_drop(std::filesystem::path t_path, libfrac::Window t_window) noexcept {
        (void)(t_path);
        (void)(t_window);
    }

    void Event::paint(Window t_window) noexcept {
#ifdef _WIN32
        PAINTSTRUCT ps;
        BeginPaint(t_window.m_hWnd, &ps);
        EndPaint(t_window.m_hWnd, &ps);
#endif
    }

    bool Window::show() noexcept { return (bool)ShowWindowAsync(m_hWnd, SW_NORMAL); }

    bool Window::redraw() noexcept { return (bool)InvalidateRect(m_hWnd, 0, FALSE); }

    error Window::client_rect(RECT* r_rect) noexcept {
        if (GetClientRect(m_hWnd, r_rect) == 0) {
            return GetLastError();
        }
        return 0;
    }

    error Window::destroy() noexcept {
        error err = 0;
        if (m_hWnd != NULL) {
            if (DestroyWindow(m_hWnd) == 0)
                return GetLastError();
        }

        return err;
    }

    error Window::notify() noexcept {
        if (PostMessageW(m_hWnd, WM_APP, 0, 0) == 0) {
            return GetLastError();
        }

        return 0;
    }

    std::string get_error_string(int t_error) noexcept { return std::system_category().message(t_error); }

    error::error(DWORD t_code) { m_error_code = t_code; }

    error error::message(const char* t_message) {
        error err = 0;
        err.m_message = t_message;
        return err;
    }

    std::string error::get_string() noexcept {
        if (m_message)
            return std::string(m_message);
        else
            return std::system_category().message(m_error_code);
    }

    error GlContextBuilder::build(GlDisplay t_display, GlContext* r_context, GlContext t_share_context) noexcept {
        // clang-format off

		int gl_attrib[] = {
			WGL_CONTEXT_MAJOR_VERSION_ARB, m_major_version,
			WGL_CONTEXT_MINOR_VERSION_ARB, m_minor_version,
			WGL_CONTEXT_PROFILE_MASK_ARB,  WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
            WGL_CONTEXT_FLAGS_ARB, WGL_CONTEXT_DEBUG_BIT_ARB,  
			0,
		};

        // clang-format on

        r_context->m_glrc = wglCreateContextAttribsARB(t_display.m_dc, t_share_context.m_glrc, gl_attrib);

        if (r_context->m_glrc == 0) {
            return GetLastError();
        }

        return 0;

        //// clang-format off
        // int pixel_format_attribs[] = {
        //     WGL_DRAW_TO_WINDOW_ARB,     GL_TRUE,
        //     WGL_SUPPORT_OPENGL_ARB,     GL_TRUE,
        //     WGL_DOUBLE_BUFFER_ARB,      GL_TRUE,
        //     WGL_ACCELERATION_ARB,       WGL_FULL_ACCELERATION_ARB,
        //     WGL_PIXEL_TYPE_ARB,         WGL_TYPE_RGBA_ARB,
        //     WGL_COLOR_BITS_ARB,         32,
        //     WGL_DEPTH_BITS_ARB,         24,
        //     WGL_STENCIL_BITS_ARB,       8,
        //     0
        // };

        // int gl33_attribs[] = {
        //     WGL_CONTEXT_MAJOR_VERSION_ARB, 3,
        //     WGL_CONTEXT_MINOR_VERSION_ARB, 3,
        //     WGL_CONTEXT_PROFILE_MASK_ARB,  WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
        //     0,
        // };
        //// clang-format on

        // int pixel_format;
        // UINT num_formats;
        // wglChoosePixelFormatARB(t_display.m_dc, pixel_format_attribs, 0, 1, &pixel_format, &num_formats);
        // if (!num_formats) {
        //     return GetLastError();
        // }

        // PIXELFORMATDESCRIPTOR pfd;

        // if (DescribePixelFormat(t_display.m_dc, pixel_format, sizeof(pfd), &pfd) == 0) {
        //     return GetLastError();
        // }

        // if (!SetPixelFormat(t_display.m_dc, pixel_format, &pfd)) {
        //     return GetLastError();
        // }

        // HGLRC gl33_context = wglCreateContextAttribsARB(t_display.m_dc, 0, gl33_attribs);
        // if (!gl33_context) {
        //     return GetLastError();
        // }

        // r_context->m_glrc = gl33_context;

        // return 0;
    }

    error GlContext::destroy() {
        if (m_glrc == NULL) {
            return 0;
        }

        if (wglDeleteContext(m_glrc) == 0) {
            return GetLastError();
        }

        return 0;
    }

    error GlDisplay::swap_buffers() {
        if (SwapBuffers(m_dc) == 0)
            return GetLastError();

        return 0;
    }

    error GlDisplay::destroy() {
        if (ReleaseDC(m_hWnd, m_dc) == 0) {
            return error::message("Could not release DC.");
        };

        return 0;
    }

    error GlDisplay::make_current(GlContext t_context) {
        if (m_dc == 0)
            return 0;

        if (wglMakeCurrent(m_dc, t_context.m_glrc) == 0) {
            return GetLastError();
        }

        return 0;
    }

} // namespace libfrac

#endif // LIB_FRAC_IMPLIB_FRAC_IMPL
#endif // _WIN32
#endif // !LIB_FRAC_HPP_
