#ifndef __USBSERVOCONTROLLER_HPP_INCLUDED__
#define __USBSERVOCONTROLLER_HPP_INCLUDED__

#define CE_SERIAL_IMPLEMENTATION
#include <iostream>
#include <sstream>
#include <chrono>
#include <vector>
//#include "ceSerial.h"
#include "serial.hpp"
#include "utils.hpp"

using namespace std;


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
        ServoProperties ();
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
        int setSpeed (unsigned char, int);
        void setDisabled (unsigned char);
        void setEnabled (unsigned char);
        ServoProperties getChannelProperty (unsigned char); 
        static const int MAX_SERVOS = 6;
        std::vector<ServoProperties> properties;
    protected:
        std::vector <unsigned char>  active_servos;
        int number_of_active_servos;
        Serial serial;
};

#endif
