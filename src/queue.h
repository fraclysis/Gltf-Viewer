#include "pch.h"

#ifndef VIEWER_QUEUE_H
#define VIEWER_QUEUE_H

#include <atomic>

#define QUEUE_BUFFER_SIZE 256

namespace viewer {

    struct ThreadInfo {
        HANDLE m_thread_handle;
        void* m_queue;
        size_t m_logical_thread_id;
        DWORD m_thread_id;
        window::GlContext m_context;
    };

    struct alignas(64) GpuQueueWork {
        unsigned char* m_image_data;
        std::string* m_path; // For debug
        GLuint m_image;
        int m_x;
        int m_y;
        int m_channels;
    };

    struct alignas(64) FileQueueWork {
        std::string* m_path;
        GLuint m_image;
    };

    struct GpuTextureQueue {
        GpuQueueWork* m_buffer;
        std::atomic_int m_size;
        window::Window m_window;

        HANDLE m_semaphore;
        window::GlDisplay m_display;
        window::GlContext m_share_context;
        ThreadInfo m_thread;
        std::mutex m_add_guard = std::mutex();

        std::atomic_int m_index = std::atomic_int(0);
        std::atomic_int m_finished = std::atomic_int(0);

        std::atomic_bool m_ready = std::atomic_bool(false);
        std::atomic_bool m_successful = std::atomic_bool(false);
        std::atomic_bool m_awake = std::atomic_bool(false);
        std::atomic_bool m_quit = std::atomic_bool(false);

        GpuTextureQueue() = default;

        window::error init(window::GlDisplay t_display, window::GlContext t_not_current_context,
                            window::Window t_window);
        void free();
        void finish();
        void add_work(GpuQueueWork& t_work);
    };

    struct FileTextureQueue {
        FileQueueWork* m_buffer;
        std::atomic_int m_size;

        std::atomic_int m_index = std::atomic_int(0);
        std::atomic_int m_finished = std::atomic_int(0);

        HANDLE m_semaphore;
        int m_thread_count;
        ThreadInfo* m_threads;

        std::atomic_bool m_quit = std::atomic_bool(false);
        std::atomic_int m_failed_threads = std::atomic_int(0);
        std::atomic_int m_awake_threads = std::atomic_int(0);

        GpuTextureQueue* m_gpu_texture_queue;

        int init(int t_thread_count, GpuTextureQueue* t_gpu_texture_queue);
        void free();
        void finish();
        void add_work(FileQueueWork& t_work);
    };

} // namespace viewer

#endif // !VIEWER_QUEUE_H
