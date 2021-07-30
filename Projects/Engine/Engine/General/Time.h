#pragma once

#include <chrono>
#include <time.h>

class Time
{
public:
	static std::time_t Get()
	{
		return std::chrono::system_clock::to_time_t(
			std::chrono::system_clock::now()
		);
	}

	static std::tm GetTM()
	{
		/* Get time */
		std::time_t t = Get();

#ifdef _WIN32
		std::tm tm;
		localtime_s(&tm, &t);
#else
		std::tm tm;
		localtime_r(&t, &tm);
#endif

		return tm;
	}
	
	static std::string GetFormattedString(const std::string& Format)
	{
		/* Get time */
		std::tm tm = GetTM();

		std::string F = Format;
		// auto Second = F.find("%S");

		// Do seconds separately as padding isn't normally applied
		/*if (Second != std::string::npos)
		{
			std::string sec = std::to_string(tm.tm_sec);
			if (sec.length() < 2) sec = "0" + sec;

			while (Second != std::string::npos)
			{
				F.replace(Second, 2, sec);
				Second = F.find("%S");
			}
		}*/
		
		std::string Buffer;
		Buffer.resize(F.size());

		int32_t Length = strftime(&Buffer[0], Buffer.size(), F.c_str(), &tm);
		
		while (Length == 0) {
			Buffer.resize(Buffer.size() * 2);
			Length = strftime(&Buffer[0], Buffer.size(), F.c_str(), &tm);
		}

		Buffer.resize(Length);
		
		return Buffer;
	}
};