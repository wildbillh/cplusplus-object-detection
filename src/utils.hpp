#pragma once

#include <chrono>
#include <thread>
#include <fstream>
#include <iostream>

#include <json/json.h>
#include <spdlog/spdlog.h>

namespace utils {

void sleepSeconds (double);
void sleepMilliseconds (long);
bool allTrue (std::vector<bool> source);
bool allTrue (bool[], int);
Json::Value readJsonFromFile (std::string filename);
void writeJsonToFile (std::string, Json::Value);

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