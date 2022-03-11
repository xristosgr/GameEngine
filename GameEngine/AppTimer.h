#pragma once

#include <iostream>
#include <chrono>

class AppTimer
{
public:

	AppTimer();
	double GetMilisecondsElapsed();
	int GetSecondsElapsed();
	void Restart();
	bool Stop();
	bool Start();
	void StartSeconds();
	float GetMilliseconds();

private:

	bool isRunning = false;
	std::chrono::time_point<std::chrono::steady_clock> start;
	std::chrono::time_point<std::chrono::steady_clock> stop;
};
