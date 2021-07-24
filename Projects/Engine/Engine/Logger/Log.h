#pragma once

#include "Engine/Core.h"
#include "Engine/Timer.h"

#include <chrono>
#include <string>

#include "spdlog/spdlog.h"
#include "Engine/General/Math.h"

#ifdef CB_PLATFORM_ANDROID
#include <android/log.h>
#include "spdlog/sinks/android_sink.h"
#endif

namespace Engine
{
	struct Engine_API ScreenMessage
	{
		std::string Message = "";
		Color Color;
		Timer Time = Timer(1.0f);
	};

	class Engine_API Log
	{
	public:
		enum Type
		{
			E_TRACE,
			E_INFO,
			E_WARN,
			E_ERROR,
			E_FATAL
		};

		template<typename... Args>
		static void Add(Log::Type type, bool bIsCore, float fTimer, const char* fmt, const Args &... args);

		inline static std::vector<ScreenMessage>& GetScreenLogger() { return m_ScreenLogger; }

	private:
		static void AddScreenMessage(const std::string& message, Color color, float fTimer);
		
		static inline Type Level = 
			#ifdef CB_DEBUG
				E_TRACE
			#else
				E_WARN
			#endif
		;

		static std::vector<ScreenMessage> m_ScreenLogger;
	};

	template<typename... Args>
	void Log::Add(Log::Type type, bool isCore, float timer, const char* fmt, const Args &... args)
	{
#ifndef CB_DIST
		if (type < Level)
			return;
		
		Color col = Color(1.0f);
		static const std::string typeNames[] = { "TRACE", "INFO", "WARN", "ERROR", "FATAL" };

		switch (type)
		{
		case E_FATAL:
			col = Color(0.862745098f, 0.0784313725f, 0.0784313725f, 1.0f);
			break;
		case E_ERROR:
			col = Color(0.941176471f, 0.352941176f, 0.352941176f, 1.0f);
			break;
		case E_WARN:
			col = Color(0.980392157f, 0.901960784f, 0.352941176f, 1.0f);
			break;
		case E_INFO:
			col = Color(0.584313725f, 0.701960784f, 0.941176471f, 1.0f);
			break;
		case E_TRACE:
		default:
			break;
		}

		/* Get time */
		std::time_t t = std::chrono::system_clock::to_time_t(
			std::chrono::system_clock::now()
		);

#ifdef _WIN32
		std::tm tm;
		localtime_s(&tm, &t);
#else
		std::tm tm;
		localtime_r(&t, &tm);
#endif

		/* Format time */
		std::string hour = std::to_string(tm.tm_hour);
		std::string min = std::to_string(tm.tm_min);
		std::string sec = std::to_string(tm.tm_sec);

		if (hour.length() < 2) hour = "0" + hour;
		if (min.length() < 2) min = "0" + min;
		if (sec.length() < 2) sec = "0" + sec;

		/* Format message */
		std::string message = "";
		std::string name = ((isCore) ? "CORE" : "USER");
		std::string time = hour + ":" + min + ":" + sec;

		fmt::memory_buffer formatted;

		try
		{
			fmt::format_to(formatted, fmt, args...);
			size_t s = formatted.size();

			if (s == 0)
				return;
			
			if (s == formatted.capacity())
				formatted.resize(s + 1);

			formatted.data()[s] = '\0';
		}
		catch (...) {}
		
		/* Compose and add to screen */
		message = "[" + time + "][" + name + "][" + typeNames[type] + "] ";
		message += formatted.data();
		AddScreenMessage(message, col, timer);

		const char* msg = message.c_str();

#ifdef CB_PLATFORM_WINDOWS
		message += '\n';
		message += '\0';
		msg = message.c_str();
		OutputDebugString(msg);
#else
		__android_log_write(
			type == E_FATAL ? ANDROID_LOG_FATAL :
			type == E_ERROR ? ANDROID_LOG_ERROR :
			type == E_WARN ? ANDROID_LOG_WARN :
			type == E_INFO ? ANDROID_LOG_INFO :
			type == E_TRACE ? ANDROID_LOG_VERBOSE :
			ANDROID_LOG_UNKNOWN,
			"Engine",
			msg
		);
#endif
#endif
	}
}

// Core log macros
#define CB_CORE_TRACE(...)			::Engine::Log::Add(::Engine::Log::Type::E_TRACE,true, 2.0f, __VA_ARGS__)
#define CB_CORE_INFO(...)			::Engine::Log::Add(::Engine::Log::Type::E_INFO,	true, 5.0f, __VA_ARGS__)
#define CB_CORE_WARN(...)			::Engine::Log::Add(::Engine::Log::Type::E_WARN,	true, 5.0f, __VA_ARGS__)
#define CB_CORE_ERROR(...)			::Engine::Log::Add(::Engine::Log::Type::E_ERROR,true, 5.0f, __VA_ARGS__)
#define CB_CORE_FATAL(...)			::Engine::Log::Add(::Engine::Log::Type::E_FATAL,true, 5.0f, __VA_ARGS__)

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
#define CB_INFO(...)				::Engine::Log::Add(::Engine::Log::Type::E_INFO,	false, 5.0f, __VA_ARGS__)
#define CB_WARN(...)				::Engine::Log::Add(::Engine::Log::Type::E_WARN,	false, 5.0f, __VA_ARGS__)
#define CB_ERROR(...)				::Engine::Log::Add(::Engine::Log::Type::E_ERROR,false, 5.0f, __VA_ARGS__)
#define CB_FATAL(...)				::Engine::Log::Add(::Engine::Log::Type::E_FATAL,false, 5.0f, __VA_ARGS__)

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