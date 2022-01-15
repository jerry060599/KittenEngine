#pragma once

#include <string>
#include <vector>
#include <chrono>

namespace Kitten {
	using namespace std::chrono;

	class Timer {
	public:
		steady_clock::time_point lastPoint;
		std::vector<double> deltaTimes;
		std::vector<double> times;
		std::vector<const char*> tags;
	private:
		double totSecs = 0;

	public:
		Timer();
		double totTime();
		void printTimes();
		double time(const char* tag = nullptr);
		void reset();
	};
}