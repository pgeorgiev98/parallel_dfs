#ifndef CHRONOMETER_H_INCLUDED
#define CHRONOMETER_H_INCLUDED

#include <chrono>

class Chronometer
{
public:
	void start()
	{
		startPoint = std::chrono::steady_clock::now();
	}

	int milliseconds() const
	{
		auto now = std::chrono::steady_clock::now();
		return std::chrono::duration_cast<std::chrono::milliseconds>(
				now - startPoint).count();
	}

private:
	std::chrono::time_point<std::chrono::steady_clock> startPoint;
};

#endif
