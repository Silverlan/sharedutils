module;

#include <spdlog/spdlog.h>
#include <spdlog/details/os.h>
#include <spdlog/details/null_mutex.h>
#include <spdlog/details/console_globals.h>
#include <spdlog/pattern_formatter.h>
#include <spdlog/sinks/sink.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>

export module spdlog;
import std;

export namespace spdlog
{
    using spdlog::logger;
    using spdlog::critical;
    using spdlog::debug;
    using spdlog::error;
    using spdlog::info;
    using spdlog::trace;
    using spdlog::warn;

    using spdlog::color_mode;

    using spdlog::log;
    using spdlog::should_log;
    using spdlog::memory_buf_t;
    using spdlog::custom_flag_formatter;
    using spdlog::set_default_logger;
    using spdlog::drop;
    using spdlog::shutdown;
    using spdlog::sink_ptr;
    using spdlog::sinks_init_list;

    using spdlog::get;
    using spdlog::get_level;
    using spdlog::flush_on;
    using spdlog::register_logger;
    using spdlog::set_pattern;

    using spdlog::formatter;
    using spdlog::pattern_formatter;
    using spdlog::set_default_logger;
    using spdlog::format_string_t;

    namespace level {
        using spdlog::level::level_enum;
        using spdlog::level::off;
        using spdlog::level::to_short_c_str;
    }

    namespace sinks {
        using spdlog::sinks::sink;
        using spdlog::sinks::basic_file_sink_mt;
        using spdlog::sinks::stdout_color_sink_mt;
    }

    namespace details {
        using spdlog::details::log_msg;
        using spdlog::details::make_unique;
        using spdlog::details::console_mutex;
        using spdlog::details::console_nullmutex;
#ifdef _WIN32
        // Workaround for msvc compiler error
        inline const char *default_eol = "\r\n";
#else
        inline const char *default_eol = spdlog::details::os::default_eol;
#endif
        namespace os {
            using spdlog::details::os::in_terminal;
            using spdlog::details::os::is_color_terminal;
        }
    }
} // namespace spdlog
