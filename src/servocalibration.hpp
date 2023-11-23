#pragma once

#include <chrono>
#include <thread>
#include <fstream>
#include <iostream>

#include <json/json.h>
#include <spdlog/spdlog.h>

class ServoCalibration {

    public:
        ServoCalibration (std::string);
        std::string buildCalibrationString (unsigned char, int, int);
        std::string printJsonValues ();
        std::vector<double> get (unsigned char, int, int);
        void set (unsigned char, int, int, std::vector<double>);
        bool readJsonFromFile ();
        void writeJsonToFile ();
    protected:
        std::string filename;
        Json::Value root;
};