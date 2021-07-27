#include "pch.h"
#include "Log.h"

namespace Engine {
	std::mutex Log::m_Mutex;
	std::vector<ScreenMessage> Log::m_ScreenLogger;
	
	void Log::AddScreenMessage(const std::string& message, Color color, float fTimer)
	{	
		m_ScreenLogger.push_back({ message, color, fTimer });

		if (m_ScreenLogger.size() > CB_MAX_LOG_MESSAGES)
			m_ScreenLogger.erase(m_ScreenLogger.begin());
	}
}