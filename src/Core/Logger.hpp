#ifndef DATAGRAPHTOOL_CORE_LOGGER_H__
#define DATAGRAPHTOOL_CORE_LOGGER_H__

#include <spdlog/logger.h>
#include <fmt/color.h>
#include <fmt/format.h>

#include <string>
#include <unordered_map>
#include <fstream>

#include <json/json.hpp>

namespace DataGraph
{
class Logger
{
public:
	static constexpr const char* LogDir = "logs";
	static constexpr const char* LogConfigPath = "configs/logs.json";
	static constexpr const char* LogExt = "log";
	static constexpr const uint32_t QueueSize = 16384;
	static constexpr const uint8_t Workers = 2;

	int init();

#if defined(_DEBUG) && !defined(FORMHANDLER_TESTS)
	template <typename... Args>
	void log(const std::string_view& loggerName, spdlog::format_string_t<Args...> fmt, Args&&... args)
	{
		auto loger = loggers.find(loggerName);
		if (loger->second.enabled)
		{
			loger->second.logger->debug(fmt, std::forward<Args>(args)...);
		}
	}

	template <typename T>
	void log(const std::string_view& loggerName, const T& msg)
	{
		auto loger = loggers.find(loggerName);
		if (loger->second.enabled)
		{
			loger->second.logger->debug(msg);
		}
	}

	template <typename... Args>
	void error(const std::string_view& loggerName, spdlog::format_string_t<Args...> fmt, Args&&... args)
	{
		auto loger = loggers.find(loggerName);
		if (loger->second.enabled)
		{
			loger->second.logger->error(fmt, std::forward<Args>(args)...);
		}
	}

	template <typename T>
	void error(const std::string_view& loggerName, const T& msg)
	{
		auto loger = loggers.find(loggerName);
		if (loger->second.enabled)
		{
			loger->second.logger->error(msg);
		}
	}

#else
	template <typename... Args>
	void log(const std::string_view& loggerName, spdlog::format_string_t<Args...> fmt, Args&&... args)
	{}

	template <typename Args>
	void log(const std::string_view& loggerName, const Args& msg)
	{}

	template <typename... Args>
	void error(const std::string_view& loggerName, spdlog::format_string_t<Args...> fmt, Args&&... args)
	{}

	template <typename Args>
	void error(const std::string_view& loggerName, const Args& msg)
	{}
#endif	// _DEBUG

	int createLogger(const std::string_view& name, bool enabled = true);

	void enableLogger(const std::string_view& name);
	void disableLogger(const std::string_view& name);

	void flush(const std::string_view& name);
	void flushAll();

	~Logger();

private:
	struct LoggerData
	{
		bool enabled;
		std::shared_ptr<spdlog::logger> logger;
	};

private:
	std::unordered_map<std::string_view, LoggerData> loggers;

	nlohmann::json logConfig;
};
};	// namespace DataGraph

#endif
