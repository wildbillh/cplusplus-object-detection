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
typedef unsigned char Channel;
typedef std::vector<unsigned char> ChannelVec;
typedef std::vector<int> IntVec;
typedef std::vector<float> FloatVec;
typedef std::vector<double> DoubleVec;

enum class PositionUnits {MICROSECONDS, DEGREES};

/**
 * Struct like class which holds the current setting for a particular 
 * channel. 
*/
class ServoProperties {
    public:
        Channel channel;
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
        DoubleVec calibration;
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
        void syncProperty (Channel);
        void sync (ChannelVec);
        void sync (ChannelVec, std::vector<ServoProperties>);


        bool writeCommand (unsigned char, Channel, string);
        bool writeCommand (unsigned char, Channel, int, string);
        int getPositionFromController (Channel);
        int setAcceleration (Channel, int);
        
        int setPosition (Channel, int);
        IntVec setPositionMulti (ChannelVec, IntVec); 
        int setPositionSync (Channel, int, float = 3.0);
        IntVec setPositionMultiSync (ChannelVec, IntVec, float = 3.0);
        int setRelativePos (Channel, float, PositionUnits units = PositionUnits::DEGREES, bool sync = false);
        IntVec setRelativePosMulti (ChannelVec, FloatVec, PositionUnits = PositionUnits::DEGREES, bool = false, float = 3.0);

        int setSpeed (Channel, int);
        int returnToHome (Channel, bool = false, float = 3.0);
        IntVec returnToHomeMulti (ChannelVec, bool = false, float = 3.0);
        void setDisabled (Channel);
        void setEnabled (Channel);
        ServoProperties getChannelProperty (Channel); 
        int calculateRelativePosition (Channel, float, PositionUnits);
        static const int MAX_SERVOS = 6;
        bool calibrateServo (Channel, bool = false);
        float calculateMovementTime (Channel, int);
        std::vector<ServoProperties> properties;
    protected:
        ChannelVec  active_servos;
        int number_of_active_servos;
        Serial serial;
        std::string calibration_file;
};


