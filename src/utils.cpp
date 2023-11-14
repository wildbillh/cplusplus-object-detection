#include "utils.hpp"


void utils::sleepSeconds (double seconds) {
    std::this_thread::sleep_for(std::chrono::microseconds((int)(seconds * 1000000)));
}

void utils::sleepMilliseconds (long milliseconds) {
    std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));   
}

bool utils::allTrue (bool source[], int size) {
    for (int i=0; i<size; i++) {
        if (!source[i]) {
            return false;
        }
    }
    return true;
}

utils::Timer::Timer () {
    start();
}

void utils::Timer::start () {
    start_instance = std::chrono::system_clock::now();
}

double utils::Timer::seconds () {

    std::chrono::time_point end_instance = std::chrono::high_resolution_clock::now();
    return std::chrono::duration<double> (end_instance - start_instance).count();
}

int utils::Timer::milliseconds () {
    std::chrono::time_point end_instance = std::chrono::high_resolution_clock::now();
    return std::chrono::duration_cast<std::chrono::milliseconds> (end_instance - start_instance).count();
}


    



