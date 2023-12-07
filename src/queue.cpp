#include "pch.h"

#include "queue.h"

#include "renderer.h"

namespace viewer {

    static size_t g_last_logical_thread_id = 0;

    static DWORD WINAPI gpu_texture_proc(LPVOID lpThreadParameter) {
        ThreadInfo info = *(ThreadInfo*)lpThreadParameter;
        GpuTextureQueue& queue = *(GpuTextureQueue*)info.m_queue;
        gt_thread_id = info.m_logical_thread_id;

        window::error err = 0;
        window::GlContextBuilder cb;

        if (err = cb.build(queue.m_display, &info.m_context, queue.m_share_context)) {
            queue.m_ready.store(true, std::memory_order_release);
            return 1;
        }

        if (err = queue.m_display.make_current(info.m_context)) {
            if (!info.m_context.destroy()) {
                info.m_context = {0};
            }

            queue.m_ready.store(true, std::memory_order_release);
            return 1;
        }

        queue.m_successful.store(true, std::memory_order_release);
        queue.m_ready.store(true, std::memory_order_release);
        queue.m_awake.store(true, std::memory_order_release);

        log_renderer_api_info();

        glEnable(GL_TEXTURE_2D);

        while (!queue.m_quit.load(std::memory_order_acquire)) {
            int size = queue.m_size.load(std::memory_order_acquire);
            int i = queue.m_index.load(std::memory_order_acquire);
            if (size > i) {

                if (queue.m_index.compare_exchange_weak(i, i + 1)) {
                    GpuQueueWork& work = queue.m_buffer[i];
#if 1
                    GL(glBindTexture(GL_TEXTURE_2D, work.m_image));

                    GLenum gl_format;

                    switch (work.m_channels) {
                    case 1:
                        gl_format = GL_R;
                        break;
                    case 2:
                        gl_format = GL_RG;
                        break;
                    case 3:
                        gl_format = GL_RGB;
                        break;
                    case 4:
                        gl_format = GL_RGBA;
                        break;
                    }

                    GL(glTexImage2D(GL_TEXTURE_2D, 0, gl_format, work.m_x, work.m_y, 0, gl_format, GL_UNSIGNED_BYTE,
                                    work.m_image_data));

                    GL(glGenerateMipmap(GL_TEXTURE_2D));
                    GL(glBindTexture(GL_TEXTURE_2D, 0));
                    GL(glFinish());
#endif
                    stbi_image_free(work.m_image_data);
                    log_info("[%d]%s", i, work.m_path->c_str());
                    log_flush();

                    queue.m_finished.fetch_add(1, std::memory_order_release);
                    log_lf_c(queue.m_window.notify());
                }
            } else {
                queue.m_awake.store(false, std::memory_order_release);
                DWORD res = WaitForSingleObjectEx(queue.m_semaphore, INFINITE, FALSE);
                queue.m_awake.store(true, std::memory_order_release);

                // if (res == WAIT_FAILED) {
                //	// TODO error handling
                //	queue.m_quit.store(true, std::memory_order_release);
                // }
            }
        }

        if (err = info.m_context.destroy()) {
            log_lf(err);
        }

        return 0;
    }

    static DWORD WINAPI file_texture_proc(LPVOID lpThreadParameter) {
        ThreadInfo info = *(ThreadInfo*)lpThreadParameter;
        gt_thread_id = info.m_logical_thread_id;
        FileTextureQueue& queue = *(FileTextureQueue*)info.m_queue;

        while (!queue.m_quit.load(std::memory_order_acquire)) {
            int size = queue.m_size.load(std::memory_order_acquire);
            int i = queue.m_index.load(std::memory_order_acquire);
            if (size > i) {
                if (queue.m_index.compare_exchange_weak(i, i + 1)) {

                    FileQueueWork& work = queue.m_buffer[i];
                    GpuQueueWork out_work;

                    if (work.m_path != nullptr) {
                        out_work.m_image = work.m_image;

                        out_work.m_image_data =
                            stbi_load(work.m_path->c_str(), &out_work.m_x, &out_work.m_y, &out_work.m_channels, 0);

                        out_work.m_path = work.m_path;
                        if (out_work.m_image_data) {
                            queue.m_gpu_texture_queue->add_work(out_work);
                        }
                        log_info("[%dL]Loaded: %s", info.m_thread_id, work.m_path->c_str());
                        log_flush();
                    } else {
                        log_error("Work path is null.");
                        log_flush();
                    }

                    queue.m_finished.fetch_add(1, std::memory_order_release);
                }
            } else {
                queue.m_awake_threads.fetch_sub(1, std::memory_order_release);
                WaitForSingleObjectEx(queue.m_semaphore, INFINITE, FALSE);
                queue.m_awake_threads.fetch_add(1, std::memory_order_release);
            }
        }

        return 0;
    }

    window::error GpuTextureQueue::init(window::GlDisplay t_display, window::GlContext t_not_current_context,
                                         window::Window t_window) {
        m_display = t_display;
        m_share_context = t_not_current_context;
        m_window = t_window;

        m_buffer = new GpuQueueWork[QUEUE_BUFFER_SIZE];

        m_semaphore = CreateSemaphoreExW(NULL, 0, 1, NULL, NULL, SEMAPHORE_ALL_ACCESS);
        if (m_semaphore == NULL) {
            return GetLastError();
        }
        std::atomic_thread_fence(std::memory_order_release);

        m_thread.m_queue = this;

        m_thread.m_logical_thread_id = g_last_logical_thread_id + 1;
        g_last_logical_thread_id = m_thread.m_logical_thread_id;

        m_thread.m_thread_handle =
            CreateThread(NULL, NULL, gpu_texture_proc, (void*)&m_thread, NULL, &m_thread.m_thread_id);
        if (m_thread.m_thread_handle == NULL) {
            return GetLastError();
        }

        while (!m_ready.load(std::memory_order_acquire))
            ;

        return 0;
    }

    void GpuTextureQueue::free() {
        finish();

        if (m_thread.m_thread_handle) {
            m_quit.store(true, std::memory_order_release);
            BOOL wake_ret = ReleaseSemaphore(m_semaphore, 1, 0);

            DWORD res1 = WaitForSingleObjectEx(m_thread.m_thread_handle, 1000, FALSE);
            BOOL res2 = 0;
            switch (res1) {
            case WAIT_OBJECT_0:
                break;
            default:
                res2 = TerminateThread(m_thread.m_thread_handle, 1);
                break;
            }

            BOOL res3 = CloseHandle(m_thread.m_thread_handle);

            log_debug("Wait: %dL", res1);
            log_debug("Terminate: %d", res2);
            log_debug("Close: %d", res3);
            m_thread.m_thread_handle = NULL;
        }

        if (m_semaphore) {
            CloseHandle(m_semaphore);
            m_semaphore = NULL;
        }

        delete[] m_buffer;
        m_buffer = nullptr;
    }

    void GpuTextureQueue::finish() {
        std::lock_guard g(m_add_guard);
        while (m_size.load(std::memory_order_acquire) > m_finished.load(std::memory_order_acquire))
            ;

        m_size.store(0, std::memory_order_release);
        m_finished.store(0, std::memory_order_release);
        m_index.store(0, std::memory_order_release);
    }

    void GpuTextureQueue::add_work(GpuQueueWork& t_work) {
        std::lock_guard g(m_add_guard);

        int size = m_size.load(std::memory_order_acquire);
        if (QUEUE_BUFFER_SIZE > size) {
            m_buffer[size] = t_work;
            m_size.fetch_add(1, std::memory_order_release);
        } else {
            finish();
            int new_size = m_size.load(std::memory_order_acquire);
            if (QUEUE_BUFFER_SIZE > size) {
                m_buffer[new_size] = t_work;
                m_size.fetch_add(1, std::memory_order_release);
            } else {
                // TODO error ?
            }
        }

        BOOL ret = ReleaseSemaphore(m_semaphore, 1, 0);
    }

    int FileTextureQueue::init(int t_thread_count, GpuTextureQueue* t_gpu_texture_queue) {
        m_gpu_texture_queue = t_gpu_texture_queue;
        m_buffer = new FileQueueWork[QUEUE_BUFFER_SIZE];

        m_semaphore = CreateSemaphoreExW(NULL, 0, t_thread_count, NULL, NULL, SEMAPHORE_ALL_ACCESS);
        if (m_semaphore == NULL) {
            return 0;
        }

        m_thread_count = 0;
        m_threads = new ThreadInfo[t_thread_count]{};
        for (int i = 0; i < t_thread_count; i++) {
            auto& thread = m_threads[m_thread_count];
            thread.m_logical_thread_id = g_last_logical_thread_id + 1;
            thread.m_queue = this;

            thread.m_thread_handle = CreateThread(NULL, NULL, file_texture_proc, &thread, NULL, &thread.m_thread_id);
            if (thread.m_thread_handle != NULL) {
                g_last_logical_thread_id = thread.m_logical_thread_id;
                m_thread_count++;
            }
        }

        return m_thread_count;
    }

    void FileTextureQueue::free() {
        for (size_t i = 0; i < m_thread_count; i++) {
            if (m_threads[i].m_thread_handle) {
                CloseHandle(m_threads[i].m_thread_handle);
                m_threads[i].m_thread_handle = NULL;
            }
        }

        if (m_threads) {
            delete[] m_threads;
            m_threads = nullptr;
        }

        if (m_semaphore) {
            CloseHandle(m_semaphore);
            m_semaphore = NULL;
        }

        delete[] m_buffer;
        m_buffer = nullptr;
    }

    void FileTextureQueue::finish() {
        while (m_size.load(std::memory_order_acquire) > m_finished.load(std::memory_order_acquire))
            ;

        m_size.store(0, std::memory_order_release);
        m_finished.store(0, std::memory_order_release);
        m_index.store(0, std::memory_order_release);
    }

    void FileTextureQueue::add_work(FileQueueWork& t_work) {
        int size = m_size.load(std::memory_order_acquire);
        if (QUEUE_BUFFER_SIZE > size) {
            m_buffer[size] = t_work;
            m_size.fetch_add(1, std::memory_order_release);
        } else {
            finish();
            int new_size = m_size.load(std::memory_order_acquire);
            if (QUEUE_BUFFER_SIZE > size) {
                m_buffer[new_size] = t_work;
                m_size.fetch_add(1, std::memory_order_release);
            } else {
                log_warn("Texture reader: freeing image buffer.\n");
                log_error("Texture reader: Could not add work to queue. Work size "
                          "%d, Work index %d, Finished works %d\n",
                          m_size.load(), m_index.load(), m_finished.load());
                log_flush();
            }
        }

        BOOL ret = ReleaseSemaphore(m_semaphore, 1, 0);
    }

} // namespace viewer