#ifndef VIEWER_LOG_H
#define VIEWER_LOG_H

#include <mutex>
#include <stdio.h>
#include <string.h>

#ifdef _WIN32
#define __FILENAME__ (strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__)
#else
#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)
#endif

extern FILE* g_log_out;
extern std::mutex g_log_mutex;
extern thread_local size_t gt_thread_id;

// clang-format off

#define log_lock(x) do { std::lock_guard macro_guard(g_log_mutex); x; } while(0)

#define log_trace(...) log_lock(log_prefix("\033[37mTRACE\033[0m", __FILENAME__, __LINE__); fprintf_s(g_log_out, __VA_ARGS__))

#define log_debug(...) log_lock(log_prefix("\033[96mDEBUG\033[0m", __FILENAME__, __LINE__); fprintf_s(g_log_out, __VA_ARGS__))

#define log_info(...) log_lock(log_prefix("\033[92mINFO\033[0m", __FILENAME__, __LINE__); fprintf_s(g_log_out, __VA_ARGS__))

#define log_warn(...) log_lock(log_prefix("\033[33mWARN\033[0m", __FILENAME__, __LINE__); fprintf_s(g_log_out, __VA_ARGS__))

#define log_error(...) log_lock(log_prefix("\033[91mERROR\033[0m", __FILENAME__, __LINE__); fprintf_s(g_log_out, __VA_ARGS__)); log_flush()

#define log_fatal(...) log_lock(log_prefix("\033[95mFATAL\033[0m", __FILENAME__, __LINE__); fprintf_s(g_log_out, __VA_ARGS__)); log_flush()

#define log_flush() fflush(g_log_out)

#define log_lf(x)                                                                                                      \
    log_error("%s", x.get_string().c_str());                                                                           \
    log_flush();

#define log_lf_c(x)                                                                                                    \
    if (x) {                                                                                                           \
        log_error("%s", x.get_string().c_str());                                                                       \
        log_flush();                                                                                                   \
    }

// clang-format on

void log_open();

void log_close();

void log_prefix(const char* t_log_level, const char* t_file_path, int t_line);

#ifdef _DEBUG
#define GL(stmt)                                                                                                       \
    do {                                                                                                               \
        stmt;                                                                                                          \
        CheckOpenGLError(#stmt, __FILE__, __LINE__);                                                                   \
    } while (0)
#else
#define GL(stmt) stmt
#endif

void CheckOpenGLError(const char* stmt, const char* fname, int line);

#endif // VIEWER_LOG_H
