#pragma once

#include <chrono>
#include <thread>

namespace utils {

void sleepSeconds (double);
void sleepMilliseconds (long);
bool allTrue (bool[], int);

/**
 * Class for getting elapsed time between to set points.
 * Uses the high resolution clock.
*/
class Timer {
    public: 
        Timer();
        void start();
        double seconds ();
        int milliseconds();

    private:
        std::chrono::time_point<std::chrono::high_resolution_clock> start_instance;
};


} // namespace