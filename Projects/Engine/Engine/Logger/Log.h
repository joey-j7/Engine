#pragma once

#include "../Core.h"

#include "Engine/Math.h"
#include "Engine/Timer.h"

#include <vector>
#include <string>

#include "spdlog/spdlog.h"
#include "spdlog/fmt/ostr.h"

#include "CustomLogSink.h"

namespace Engine {
	struct ScreenMessage
	{
		std::string Message = "";
		Color Color;
		Timer Time = Timer(1.0f);
	};

	class Engine_API Log
	{
#ifdef CB_PLATFORM_WINDOWS
		friend class CustomLogSink<spdlog::details::console_stdout, spdlog::details::console_mutex>;
		friend class CustomLogSink<spdlog::details::console_stdout, spdlog::details::console_nullmutex>;
		friend class CustomLogSink<spdlog::details::console_stderr, spdlog::details::console_mutex>;
		friend class CustomLogSink<spdlog::details::console_stderr, spdlog::details::console_nullmutex>;
#elif CB_PLATFORM_ANDROID
		friend class CustomLogSink<std::mutex>;
		friend class CustomLogSink<spdlog::details::null_mutex>;
#endif

	public:
		enum Type
		{
			E_TRACE,
			E_INFO,
			E_WARN,
			E_ERROR,
			E_FATAL
		};

		static void Init();

		template<typename... Args>
		static void Add(Log::Type type, bool bIsCore, float fTimer, const char *fmt, const Args &... args);

		inline static std::vector<ScreenMessage>& GetScreenLogger() { return m_ScreenLogger; }
	private:
		static void AddScreenMessage(std::string message, float fTimer);

		static std::shared_ptr<spdlog::logger> m_CoreLogger;
		static std::shared_ptr<spdlog::logger> m_ClientLogger;

		static std::vector<ScreenMessage> m_ScreenLogger;
	};

	template<typename... Args>
	void Log::Add(Log::Type type, bool bIsCore, float fTimer, const char *fmt, const Args &... args)
	{
		spdlog::logger* pLogger = bIsCore ? m_CoreLogger.get() : m_ClientLogger.get();
		Color color = Color(1.0f);

		switch (type)
		{
		case E_TRACE:
			pLogger->trace(fmt, args...);
			break;
		case E_INFO:
			pLogger->info(fmt, args...);
			color = Color(0.584313725f, 0.701960784f, 0.941176471f, 1.0f);
			break;
		case E_WARN:
			pLogger->warn(fmt, args...);
			color = Color(0.980392157f, 0.901960784f, 0.352941176f, 1.0f);
			break;
		case E_ERROR:
			pLogger->error(fmt, args...);
			color = Color(0.941176471f, 0.352941176f, 0.352941176f, 1.0f);
			break;
		case E_FATAL:
			pLogger->critical(fmt, args...);
			color = Color(0.862745098f, 0.0784313725f, 0.0784313725f, 1.0f);
			break;
		default:
			break;
		}

		ScreenMessage& msg = m_ScreenLogger.back();
		msg.Time.Init(fTimer);
		msg.Color = color;
	}
}

// Core log macros
#define CB_CORE_TRACE(...)			::Engine::Log::Add(::Engine::Log::Type::E_TRACE,true, 2.0f, __VA_ARGS__)
#define CB_CORE_INFO(...)			::Engine::Log::Add(::Engine::Log::Type::E_INFO,	true, 2.0f, __VA_ARGS__)
#define CB_CORE_WARN(...)			::Engine::Log::Add(::Engine::Log::Type::E_WARN,	true, 2.0f, __VA_ARGS__)
#define CB_CORE_ERROR(...)			::Engine::Log::Add(::Engine::Log::Type::E_ERROR,true, 2.0f, __VA_ARGS__)
#define CB_CORE_FATAL(...)			::Engine::Log::Add(::Engine::Log::Type::E_FATAL,true, 2.0f, __VA_ARGS__)

#define CB_CORE_TRACE_T(t, ...)		::Engine::Log::Add(::Engine::Log::Type::E_TRACE,true, t, __VA_ARGS__)
#define CB_CORE_INFO_T(t, ...)		::Engine::Log::Add(::Engine::Log::Type::E_INFO,	true, t, __VA_ARGS__)
#define CB_CORE_WARN_T(t, ...)		::Engine::Log::Add(::Engine::Log::Type::E_WARN,	true, t, __VA_ARGS__)
#define CB_CORE_ERROR_T(t, ...)		::Engine::Log::Add(::Engine::Log::Type::E_ERROR,true, t, __VA_ARGS__)
#define CB_CORE_FATAL_T(t, ...)		::Engine::Log::Add(::Engine::Log::Type::E_FATAL,true, t, __VA_ARGS__)

#define CB_CORE_TRACE_I(...)		::Engine::Log::Add(::Engine::Log::Type::E_TRACE,true, 0.0f, __VA_ARGS__)
#define CB_CORE_INFO_I(...)			::Engine::Log::Add(::Engine::Log::Type::E_INFO,	true, 0.0f, __VA_ARGS__)
#define CB_CORE_WARN_I(...)			::Engine::Log::Add(::Engine::Log::Type::E_WARN,	true, 0.0f, __VA_ARGS__)
#define CB_CORE_ERROR_I(...)		::Engine::Log::Add(::Engine::Log::Type::E_ERROR,true, 0.0f, __VA_ARGS__)
#define CB_CORE_FATAL_I(...)		::Engine::Log::Add(::Engine::Log::Type::E_FATAL,true, 0.0f, __VA_ARGS__)

// Client log macros
#define CB_TRACE(...)				::Engine::Log::Add(::Engine::Log::Type::E_TRACE,false, 1.0f, __VA_ARGS__)
#define CB_INFO(...)				::Engine::Log::Add(::Engine::Log::Type::E_INFO,	false, 1.0f, __VA_ARGS__)
#define CB_WARN(...)				::Engine::Log::Add(::Engine::Log::Type::E_WARN,	false, 1.0f, __VA_ARGS__)
#define CB_ERROR(...)				::Engine::Log::Add(::Engine::Log::Type::E_ERROR,false, 1.0f, __VA_ARGS__)
#define CB_FATAL(...)				::Engine::Log::Add(::Engine::Log::Type::E_FATAL,false, 1.0f, __VA_ARGS__)

#define CB_TRACE_T(t, ...)			::Engine::Log::Add(::Engine::Log::Type::E_TRACE,false, t, __VA_ARGS__)
#define CB_INFO_T(t, ...)			::Engine::Log::Add(::Engine::Log::Type::E_INFO,	false, t, __VA_ARGS__)
#define CB_WARN_T(t, ...)			::Engine::Log::Add(::Engine::Log::Type::E_WARN,	false, t, __VA_ARGS__)
#define CB_ERROR_T(t, ...)			::Engine::Log::Add(::Engine::Log::Type::E_ERROR,false, t, __VA_ARGS__)
#define CB_FATAL_T(t, ...)			::Engine::Log::Add(::Engine::Log::Type::E_FATAL,false, t, __VA_ARGS__)

#define CB_TRACE_I(...)				::Engine::Log::Add(::Engine::Log::Type::E_TRACE,false, 0.0f, __VA_ARGS__)
#define CB_INFO_I(...)				::Engine::Log::Add(::Engine::Log::Type::E_INFO,	false, 0.0f, __VA_ARGS__)
#define CB_WARN_I(...)				::Engine::Log::Add(::Engine::Log::Type::E_WARN,	false, 0.0f, __VA_ARGS__)
#define CB_ERROR_I(...)				::Engine::Log::Add(::Engine::Log::Type::E_ERROR,false, 0.0f, __VA_ARGS__)
#define CB_FATAL_I(...)				::Engine::Log::Add(::Engine::Log::Type::E_FATAL,false, 0.0f, __VA_ARGS__)