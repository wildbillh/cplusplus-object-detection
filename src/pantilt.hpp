#pragma once


#include "usbservocontroller.hpp"

enum class WhichServo {PAN, TILT, BOTH};

class TiltPan: USBServoController {

    public:
        PanTilt (unsigned char _pan, unsigned char _tilt, std::string calibrationFile) : USBServoController(calibrationFile);
    protected:
        unsigned char pan;
        unsigned char tilt;
};