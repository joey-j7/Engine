#include "pch.h"
#include "Log.h"

#include "CustomLogSink.h"

namespace Engine {

	std::shared_ptr<spdlog::logger> Log::m_CoreLogger;
	std::shared_ptr<spdlog::logger> Log::m_ClientLogger;
	std::vector<ScreenMessage>		Log::m_ScreenLogger;

	void Log::Init()
	{
		spdlog::set_pattern("%^[%T] %n: %v%$");

		m_CoreLogger = CustomLogSink_mt("Engine");
		m_CoreLogger->set_level(spdlog::level::trace);

		m_ClientLogger = CustomLogSink_mt("Game");
		m_ClientLogger->set_level(spdlog::level::trace);
	}

	void Log::AddScreenMessage(std::string message, float fTimer)
	{
		m_ScreenLogger.push_back({ message, Color(1.0f), fTimer });

		if (m_ScreenLogger.size() > CB_MAX_LOG_MESSAGES)
			m_ScreenLogger.erase(m_ScreenLogger.begin());
	}
}