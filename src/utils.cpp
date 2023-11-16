#include "utils.hpp"


void utils::sleepSeconds (double seconds) {
    
    /**
     * Sleep for the designated number of seconds
     * @param seconds - seconds to sleep
    */

    std::this_thread::sleep_for(std::chrono::microseconds((int)(seconds * 1000000)));
}

// --------------------------------------------------------------------------------------

void utils::sleepMilliseconds (long milliseconds) {

    /** 
     * Sleep for the designated number of milliseconds
     * @param milliseconds - time to sleep
    */

    std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));   
}


// --------------------------------------------------------------------------------------

bool utils::allTrue (bool source[], int size) {

    /**
     * Returns true if all of the booleans in an array are true
     * @param source - array to test
     * @param size - size of array
     * @returns - true if all are true
    */

    for (int i=0; i<size; i++) {
        if (!source[i]) {
            return false;
        }
    }
    return true;
}

// --------------------------------------------------------------------------------------

utils::Timer::Timer () {

    /**
     * Initialize and start the timer
    */

    start();
}

// --------------------------------------------------------------------------------------

void utils::Timer::start () {

    /**
     * Explicitly start or restart the timer
    */

    start_instance = std::chrono::system_clock::now();
}

// --------------------------------------------------------------------------------------

double utils::Timer::seconds () {

    /**
     * Gets the seconds since the timer was started
     * @returns seconds expired
    */

    std::chrono::time_point end_instance = std::chrono::high_resolution_clock::now();
    return std::chrono::duration<double> (end_instance - start_instance).count();
}

// --------------------------------------------------------------------------------------

int utils::Timer::milliseconds () {

    /**
     * Gets the milliseconds since the timer was started
     * @returns milliseconds expired
    */

    std::chrono::time_point end_instance = std::chrono::high_resolution_clock::now();
    return std::chrono::duration_cast<std::chrono::milliseconds> (end_instance - start_instance).count();
}


    



