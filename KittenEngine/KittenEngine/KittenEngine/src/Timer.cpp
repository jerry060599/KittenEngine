#include "../includes/modules/Timer.h"

namespace Kitten {
	Timer::Timer() {
		reset();
	}

	double Timer::totTime() {
		return totSecs;
	}

	double Timer::time(const char* tag) {
		auto time = high_resolution_clock::now();
		double delta = duration_cast<duration<double>>(time - lastPoint).count();
		lastPoint = time;

		totSecs += delta;
		deltaTimes.push_back(delta);
		times.push_back(totSecs);
		tags.push_back(tag);
		return totSecs;
	}

	void Timer::reset() {
		deltaTimes.clear();
		times.clear();
		tags.clear();
		lastPoint = high_resolution_clock::now();
	}

	void Timer::printTimes() {
		for (size_t i = 0; i < times.size(); i++) {
			double time = times[i];
			if (tags[i] == nullptr)
				printf("Tag%03zd timed @ %.2f sec", i, time);
			else
				printf("%s timed @ %.2f sec", tags[i], time);

			if (time < 10) printf(" (%.4f ms)", 1000 * time);

			if (i > 0) {
				time = deltaTimes[i];
				printf(" delta=%.2f sec", time);
				if (time < 10) printf(" (%.4f ms)", 1000 * time);
			}
			printf("\n");
		}
		printf("Total: %.2f sec", totSecs);
		if (totSecs < 10) printf(" (%.4f ms)", 1000 * totSecs);
		printf("\n");
	}
}