#ifndef __USBSERVOCONTROLLER_HPP_INCLUDED__
#define __USBSERVOCONTROLLER_HPP_INCLUDED__

#define CE_SERIAL_IMPLEMENTATION
#include <algorithm>
#include <iostream>
#include <string>
#include <string_view>
#include <sstream>
#include <chrono>
#include <vector>
#include <format>
//#include "ceSerial.h"
#include "serial.hpp"
#include "utils.hpp"
#include <spdlog/spdlog.h>

using namespace std;

enum class PositionUnits {MICROSECONDS, DEGREES};


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
        ServoProperties (unsigned char = 99, int = 120);
        string print ();
};



class USBServoController {
    public:
        USBServoController ();
        ~USBServoController ();
        void close ();
        bool open (string);
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
        void setDisabled (unsigned char);
        void setEnabled (unsigned char);
        ServoProperties getChannelProperty (unsigned char); 

        int calculateRelativePosition (unsigned char, float, PositionUnits);
        static const int MAX_SERVOS = 6;
        std::vector<ServoProperties> properties;
    protected:
        std::vector <unsigned char>  active_servos;
        int number_of_active_servos;
        Serial serial;
};

#endif
