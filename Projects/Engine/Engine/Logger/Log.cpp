#include "pch.h"
#include "Log.h"

#include "Engine/Logger/Sinks/engine_sink.h"

namespace Engine {

	std::shared_ptr<spdlog::logger> Log::m_CoreLogger;
	std::shared_ptr<spdlog::logger> Log::m_ClientLogger;
	std::vector<ScreenMessage>		Log::m_ScreenLogger;

	void Log::Init()
	{
		spdlog::set_pattern("%^[%T] %n: %v%$");

		if (!m_CoreLogger.get())
		{
			m_CoreLogger = EngineLogSink("Engine");
			m_CoreLogger->set_level(spdlog::level::trace);
		}

		if (!m_CoreLogger.get())
		{
			m_ClientLogger = EngineLogSink("Game");
			m_ClientLogger->set_level(spdlog::level::trace);
		}
	}

	void Log::AddScreenMessage(const std::string& message, Color color, float fTimer)
	{
		m_ScreenLogger.push_back({ message, color, fTimer });

		if (m_ScreenLogger.size() > CB_MAX_LOG_MESSAGES)
			m_ScreenLogger.erase(m_ScreenLogger.begin());
	}
}