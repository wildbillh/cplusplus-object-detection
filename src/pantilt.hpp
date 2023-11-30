#pragma once

#include "utils.hpp"
#include "usbservocontroller.hpp"

enum class WhichServo {PAN, TILT, BOTH};

class PanTilt: public USBServoController {

    public:
        PanTilt (Channel, Channel, std::string);
        ChannelVec getChannels (WhichServo);
        bool calibrate (WhichServo, bool);
        void sync (ServoProperties, ServoProperties);
        IntVec setAcceleration (WhichServo, IntVec);
        IntVec setSpeed (WhichServo, IntVec);
        IntVec setRelativePos(WhichServo, FloatVec, PositionUnits = PositionUnits::DEGREES, bool = false, float = 3.0);
        IntVec returnToHome (WhichServo, bool = false, float = 3.0);
        std::tuple<float, int> calculateMovementTime (int, int, int = 30);
    protected:
        Channel pan;
        Channel tilt;
};