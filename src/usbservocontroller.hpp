#pragma once


#include <algorithm>
#include <iostream>
#include <string>
#include <sstream>
#include <chrono>
#include <vector>

#include "serial.hpp"
#include "utils.hpp"
#include "servocalibration.hpp"
#include <spdlog/spdlog.h>

using namespace std;

enum class PositionUnits {MICROSECONDS, DEGREES};

/**
 * Struct like class which holds the current setting for a particular 
 * channel. 
*/
class ServoProperties {
    public:
        unsigned char channel;
        int min;
        int max;
        int home;
        int pos;
        int speed;
        int acceleration;
        bool disabled;
        bool active;
        int range_degrees;
	    float microseconds_per_degree; 
        std::vector<double> calibration;
        ServoProperties (unsigned char = 99, int = 120);
        string print ();
    
};


/**
 * Class for sending and receiving commands to a USB Servo controller
*/
class USBServoController {
    public:
        USBServoController (std::string = std::string());
        ~USBServoController ();
        void close ();
        void open (string);
        void syncProperty (unsigned char);
        void sync (std::vector<unsigned char>);
        void sync (std::vector<unsigned char>, std::vector<ServoProperties>);


        bool writeCommand (unsigned char, unsigned char, string);
        bool writeCommand (unsigned char, unsigned char, int, string);
        int getPositionFromController (unsigned char);
        int setAcceleration (unsigned char, int);
        int setPosition (unsigned char, int);
        std::vector<int> setPositionMulti (std::vector<unsigned char>, std::vector<int>); 

        int setPositionSync (unsigned char, int, float = 3.0);
        std::vector<int> setPositionMultiSync (std::vector<unsigned char>, std::vector<int>, float = 3.0);
        int setRelativePos (unsigned char, float, PositionUnits units = PositionUnits::DEGREES, bool sync = false);
        
        int setSpeed (unsigned char, int);
        int returnToHome (unsigned char, bool = false, float = 3.0);
        std::vector<int> returnToHomeMulti (std::vector<unsigned char>, bool = false, float = 3.0);
        void setDisabled (unsigned char);
        void setEnabled (unsigned char);
        ServoProperties getChannelProperty (unsigned char); 

        int calculateRelativePosition (unsigned char, float, PositionUnits);
        static const int MAX_SERVOS = 6;
        std::vector<ServoProperties> properties;
        bool calibrateServo (unsigned char, bool = false);
    protected:
        std::vector <unsigned char>  active_servos;
        int number_of_active_servos;
        Serial serial;
        std::string calibration_file;
};


