#pragma once

#include "../Core.h"

#include "Engine/Math.h"
#include "Engine/Timer.h"

#include <vector>
#include <string>

#include "spdlog/spdlog.h"
#include "spdlog/fmt/ostr.h"

class engine_sink;

namespace Engine {
	struct ScreenMessage
	{
		std::string Message = "";
		Color Color;
		Timer Time = Timer(1.0f);
	};

	class Engine_API Log
	{
		friend class engine_sink;

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

		static void AddScreenMessage(const std::string& message, Color color, float fTimer);
		inline static std::vector<ScreenMessage>& GetScreenLogger() { return m_ScreenLogger; }
	private:
		static std::shared_ptr<spdlog::logger> m_CoreLogger;
		static std::shared_ptr<spdlog::logger> m_ClientLogger;

		static std::vector<ScreenMessage> m_ScreenLogger;
	};

	template<typename... Args>
	void Log::Add(Log::Type type, bool bIsCore, float fTimer, const char *fmt, const Args &... args)
	{
		spdlog::logger* pLogger = bIsCore ? m_CoreLogger.get() : m_ClientLogger.get();

		switch (type)
		{
		case E_TRACE:
			pLogger->trace(fmt, args...);
			break;
		case E_INFO:
			pLogger->info(fmt, args...);
			break;
		case E_WARN:
			pLogger->warn(fmt, args...);
			break;
		case E_ERROR:
			pLogger->error(fmt, args...);
			break;
		case E_FATAL:
			pLogger->critical(fmt, args...);
			break;
		default:
			break;
		}

#ifndef CB_DIST
		ScreenMessage& msg = m_ScreenLogger.back();
		msg.Time.Init(fTimer);
#endif
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