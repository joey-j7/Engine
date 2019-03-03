//
// Copyright(c) 2016 spdlog
// Distributed under the MIT License (http://opensource.org/licenses/MIT)
//

#pragma once

#ifndef SPDLOG_H
#include "spdlog/spdlog.h"
#endif

#include "spdlog/common.h"
#include "spdlog/details/console_globals.h"
#include "spdlog/details/null_mutex.h"
#include "spdlog/sinks/sink.h"

#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>

#include "Engine/Logger/Log.h"

class Engine_API engine_sink : public spdlog::sinks::sink
{
public:
	engine_sink() {}

	~engine_sink() override
	{
		this->flush();
	}

	engine_sink(const engine_sink &other) = delete;
	engine_sink &operator=(const engine_sink &other) = delete;

	void log(const spdlog::details::log_msg &msg) final override
	{
#ifndef CB_DIST
		fmt::memory_buffer formatted;
		this->formatter_->format(msg, formatted);

		Color col = Color(1.0f);

		switch (msg.level)
		{
		case spdlog::level::critical:
			col = Color(0.862745098f, 0.0784313725f, 0.0784313725f, 1.0f);
			break;
		case spdlog::level::err:
			col = Color(0.941176471f, 0.352941176f, 0.352941176f, 1.0f);
			break;
		case spdlog::level::warn:
			col = Color(0.980392157f, 0.901960784f, 0.352941176f, 1.0f);
			break;
		case spdlog::level::info:
			col = Color(0.584313725f, 0.701960784f, 0.941176471f, 1.0f);
			break;
		case spdlog::level::trace:
		default:
			break;
		}

		Engine::Log::AddScreenMessage(std::string(formatted.data()), col, 1.0f);
#endif
	}

	void flush() final override	{}

	void set_pattern(const std::string &pattern) override final
	{
		formatter_ = std::unique_ptr<spdlog::formatter>(new spdlog::pattern_formatter(pattern));
	}

	void set_formatter(std::unique_ptr<spdlog::formatter> sink_formatter) override final
	{
		formatter_ = std::move(sink_formatter);
	}
};

template<typename Factory = spdlog::default_factory>
inline std::shared_ptr<spdlog::logger> EngineLogSink(const std::string &logger_name)
{
	return Factory::template create<engine_sink>(logger_name);
}