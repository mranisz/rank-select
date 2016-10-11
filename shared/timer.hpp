#ifndef TIMER_HPP
#define	TIMER_HPP

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
	void startTimer() {
            timer.start = high_resolution_clock::now();
        }
	void stopTimer() {
            timer.stop = high_resolution_clock::now();
        }
	double getElapsedTime() {
            nanoseconds time_span = duration_cast<nanoseconds>(timer.stop - timer.start);
            return (double)time_span.count() / 1000000000.0;
        }
};

#endif	/* TIMER_HPP */

