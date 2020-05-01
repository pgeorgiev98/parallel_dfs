#ifndef LOG_H_INCLUDED
#define LOG_H_INCLUDED

#include <iostream>
#include <sstream>

class Logger
{
public:
	enum class LogLevel
	{
		Error, Info, Debug,
	};

	Logger(LogLevel level) : level(level), addSpace(true), addNewLine(true) {}

	~Logger()
	{
		if (line.empty())
			return;
		std::cout << line;
		if (addNewLine)
			std::cout << std::endl;
	}

	template <typename T>
	Logger &operator<<(const T &v)
	{
		if (level > maxLevel)
			return *this;

		if (!line.empty() && addSpace)
			line += ' ';
		std::ostringstream s;
		s << v;
		line += s.str();
		return *this;
	}

	Logger &noSpace()
	{
		addSpace = false;
		return *this;
	}

	Logger &noNewLine()
	{
		addNewLine = false;
		return *this;
	}

	static void setMaximumLogLevel(LogLevel level)
	{
		maxLevel = level;
	}

private:
	std::string line;
	LogLevel level;
	bool addSpace;
	bool addNewLine;
	static LogLevel maxLevel;
};

inline Logger logError()
{
	return Logger(Logger::LogLevel::Error);
}

inline Logger logInfo()
{
	return Logger(Logger::LogLevel::Info);
}

inline Logger logDebug()
{
	return Logger(Logger::LogLevel::Debug);
}

#endif
