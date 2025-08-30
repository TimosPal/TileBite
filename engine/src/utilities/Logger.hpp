#ifndef LOGGER_HPP
#define LOGGER_HPP

#ifdef _MSC_VER
#define LOG_UNIMPLEMENTED do { LOG_WARNING("Not implemented " __FUNCSIG__); } while(0)
#else
#define LOG_UNIMPLEMENTED do { LOG_WARNING("Not implemented " __PRETTY_FUNCTION__); } while(0)
#endif

#if !defined(DEBUG_MODE) && 0 // Logging enabled in release mode

// No-op in release mode
#define LOG_INFO(msg, ...) ((void)0)
#define LOG_ERROR(msg, ...) ((void)0)
#define LOG_WARNING(msg, ...) ((void)0)
#define LOG_CRITICAL(msg, ...) ((void)0)

#else

#include <spdlog/spdlog.h>

#define LOG_INFO(msg, ...) Logger::getInstance().logInfo(msg, ##__VA_ARGS__)
#define LOG_ERROR(msg, ...) Logger::getInstance().logError(msg, ##__VA_ARGS__)
#define LOG_WARNING(msg, ...) Logger::getInstance().logWarning(msg, ##__VA_ARGS__)
#define LOG_CRITICAL(msg, ...) Logger::getInstance().logCritical(msg, ##__VA_ARGS__)

namespace TileBite {

class Logger {
public:

    static Logger& getInstance()
    {
        static Logger instance;
        return instance;
    }

    template <typename... Args>
    inline void logInfo(std::format_string<Args...> message, Args&&... args)
    {
        std::string formatted_message = std::format(message, std::forward<Args>(args)...);
        spdlog::info(formatted_message);
    }

    template <typename... Args>
    inline void logError(std::format_string<Args...> message, Args&&... args)
    {
        std::string formatted_message = std::format(message, std::forward<Args>(args)...);
        spdlog::error(formatted_message);
    }

    template <typename... Args>
    inline void logWarning(std::format_string<Args...> message, Args&&... args)
    {
        std::string formatted_message = std::format(message, std::forward<Args>(args)...);
        spdlog::warn(formatted_message);
    }

    template <typename... Args>
    inline void logCritical(std::format_string<Args...> message, Args&&... args)
    {
        std::string formatted_message = std::format(message, std::forward<Args>(args)...);
        spdlog::critical(formatted_message);
    }

private:

    Logger()
    {
        // Hardcode default logging pattern.
        spdlog::set_pattern("%^[%l]%$ %v");
    }

};

} // TileBite

#endif // DEBUG_MODE

#endif // !LOGGER_HPP