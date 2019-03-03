#pragma once

#ifdef CB_PLATFORM_WINDOWS
#ifdef _WIN32
#include "spdlog/sinks/wincolor_sink.h"

#define TEMPLATE_TYPE template<typename OutHandle, typename ConsoleMutex>
#define SINK_TYPE spdlog::sinks::wincolor_sink<OutHandle, ConsoleMutex>
#else
#include "spdlog/sinks/ansicolor_sink.h"

#define TEMPLATE_TYPE template<typename TargetStream, class ConsoleMutex>
#define SINK_TYPE spdlog::sinks::ansicolor_sink<TargetStream, ConsoleMutex>
#endif

#elif CB_PLATFORM_ANDROID
#include "spdlog/sinks/android_sink.h"

#define TEMPLATE_TYPE template<typename Mutex>
#define SINK_TYPE spdlog::sinks::android_sink<Mutex>
#endif

TEMPLATE_TYPE
class CustomLogSink : public SINK_TYPE
{
public:
	void log(const spdlog::details::log_msg &msg) final override
	{
		SINK_TYPE::log(msg);

#ifndef CB_DIST
		fmt::memory_buffer formatted;
		this->formatter_->format(msg, formatted);
		
		std::string st = std::string(formatted.data());
		::Engine::Log::AddScreenMessage(st, 1.0f);
#endif
	}
};

#include "spdlog/details/null_mutex.h"
#include <mutex>

#ifdef CB_PLATFORM_WINDOWS
template<typename Factory = spdlog::default_factory>
inline std::shared_ptr<spdlog::logger> CustomLogSink_mt(const std::string &logger_name)
{
	return Factory::template create<CustomLogSink<spdlog::details::console_stdout, spdlog::details::console_mutex>>(logger_name);
}

template<typename Factory = spdlog::default_factory>
inline std::shared_ptr<spdlog::logger> CustomLogSink_st(const std::string &logger_name)
{
	return Factory::template create<CustomLogSink<spdlog::details::console_stdout, spdlog::details::console_nullmutex>>(logger_name);
}

template<typename Factory = spdlog::default_factory>
inline std::shared_ptr<spdlog::logger> CustomErrSink_mt(const std::string &logger_name)
{
	return Factory::template create<CustomLogSink<spdlog::details::console_stderr, spdlog::details::console_mutex>>(logger_name);
}

template<typename Factory = spdlog::default_factory>
inline std::shared_ptr<spdlog::logger> CustomErrSink_st(const std::string &logger_name)
{
	return Factory::template create<CustomLogSink<spdlog::details::console_stderr, spdlog::details::console_nullmutex>>(logger_name);
}
#elif CB_PLATFORM_ANDROID
template<typename Factory = spdlog::default_factory>
inline std::shared_ptr<spdlog::logger> CustomLogSink_mt(const std::string &logger_name)
{
    return Factory::template create<CustomLogSink<std::mutex>>(logger_name);
}

template<typename Factory = spdlog::default_factory>
inline std::shared_ptr<spdlog::logger> CustomLogSink_st(const std::string &logger_name, const std::string &tag = "spdlog")
{
	return Factory::template create<CustomLogSink<spdlog::details::null_mutex>>(logger_name, tag);
}
#endif