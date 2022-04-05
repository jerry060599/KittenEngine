#include <exception>
#include "../includes/modules/Timer.h"

namespace Kitten {
	Timer::Timer() {
		reset();
	}

	void Timer::start(const char* tag) {
		entry zero{};
		auto itr = entries.insert(std::make_pair(tag, zero));
		if (itr.first->second.inFence) throw new std::exception("Tag already started");
		itr.first->second.inFence = true;
		itr.first->second.lastPoint = high_resolution_clock::now();
	}

	double Timer::end(const char* tag) {
		auto time = high_resolution_clock::now();
		entry& e = entries[tag];

		if (!e.inFence) throw new std::exception("Tag not started");
		double delta = duration_cast<duration<double>>(time - e.lastPoint).count();
		e.dist.accu(delta);
		e.inFence = false;
		return delta;
	}

	void Timer::reset() {
		entries.clear();
	}

	inline void sprintTime(char buff[128], double time) {
		if (time > 60 * 60 * 3)
			snprintf(buff, 128, "%.2f hr", time / 3600);
		else if (time > 60 * 15)
			snprintf(buff, 128, "%.3f min", time / 60);
		else if (time > 10)
			snprintf(buff, 128, "%.2f sec", time);
		else if (time > 2e-3)
			snprintf(buff, 128, "%.1f ms", time * 1e3);
		else if (time > 2e-6)
			snprintf(buff, 128, "%.1f us", time * 1e6);
		else
			snprintf(buff, 128, "%.1f ns", time * 1e9);
	}

	void Timer::printTimes() {
		for (auto p : entries) {
			printf("\"%s\" ", p.first);

			char buff[128];
			sprintTime(buff, p.second.dist.mean());
			printf("avg: %s, ", buff);

			sprintTime(buff, p.second.dist.sd());
			printf("sd: %s, count: %d, ", buff, p.second.dist.num);

			sprintTime(buff, p.second.dist.X);
			printf("tot: %s\n", buff);
		}
	}
}