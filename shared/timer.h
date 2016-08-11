#ifndef _TIMER_H
#define _TIMER_H

#include <chrono>

using namespace std;
using namespace chrono;

typedef struct {
	high_resolution_clock::time_point start;
	high_resolution_clock::time_point stop;
} stopWatch;

class ChronoStopWatch {

private:
	stopWatch timer;
public:
	ChronoStopWatch() {};
	void startTimer();
	void stopTimer();
	double getElapsedTime();
};

#endif /* TIMER_H_ */
