#pragma once

#include <chrono>
#include <thread>

namespace utils {
//typedef chrono::high_resolution_clock Time;
//typedef chrono::milliseconds ms;
//typedef chrono::duration<double> fsec;
//static std::chrono::_V2::system_clock::time_point

void sleepSeconds (double);
void sleepMilliseconds (long);

class Timer {
    public: 
        Timer();
        void start();
        double seconds ();
        int milliseconds();

    private:
        std::chrono::time_point<std::chrono::high_resolution_clock> start_instance;
        //const std::chrono::time_point<std::chrono::system_clock> now =
        //std::chrono::system_clock::now();
};


} // namespace