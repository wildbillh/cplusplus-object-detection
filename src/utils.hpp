#pragma once

#include <chrono>
#include <thread>
#include <fstream>
#include <iostream>

#include <json/json.h>

namespace utils {

void sleepSeconds (double);
void sleepMilliseconds (long);
bool allTrue (bool[], int);
Json::Value readJsonFromFile (std::string filename);

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