
#include "pantilt.hpp"

PanTilt::PanTilt (Channel _pan, Channel _tilt, std::string calibrationFile) : USBServoController(calibrationFile) {
    pan = _pan;
    tilt = _tilt;
    
}

// ----------------------------------------------------------------------------

ChannelVec PanTilt::getChannels (WhichServo whichServos) {
    /**
     * When sent the enum representing pan-tilt, returns a vector of channels
     * @param whichServos - enum
     * @returns - vector of channels
    */
  
    if (whichServos == WhichServo::PAN) {
        return std::vector<unsigned char> {pan};
    }
    else if (whichServos == WhichServo::TILT) {
        return std::vector<unsigned char> {tilt};   
    }

    return std::vector<unsigned char> {pan, tilt};
}

// ---------------------------------------------------------------------------------------

bool PanTilt::calibrate (WhichServo whichServos, bool force) {
    /**
     * Calibrates the desired servos
     * @param whichServos - PAN, TILT or BOTH
     * @param force - if true will recalibrate even if a file entry exists
    */
    
    // Get the list of channels to calibrate
    std::vector<unsigned char> channels = getChannels(whichServos);

    // Set up a list of return bools
    std::vector<bool> return_bools;

    for (auto channel : channels) {
        return_bools.push_back(calibrateServo(channel, force));
    }

    return utils::allTrue(return_bools);

}

// --------------------------------------------------------------------------------------

void PanTilt::sync (ServoProperties panProps, ServoProperties tiltProps) {

    /**
     * Sync the controller to the given properties
     * @param panProps - pan properties
     * @param tiltProps - tilt properties
    */

    // Set the active servos and properties vectors
    active_servos.clear();

    active_servos.push_back(panProps.channel);
    panProps.active = true;
    properties[panProps.channel] = panProps;
    syncProperty(panProps.channel);
    
    active_servos.push_back(tiltProps.channel);
    tiltProps.active = true;
    properties[tiltProps.channel] = tiltProps;
    syncProperty(tiltProps.channel);

}

// ------------------------------------------------------------------------------

IntVec PanTilt::setAcceleration (WhichServo whichServo, IntVec vals) {
    /**
     * Sets the acceleration of the given servos
     * @param whichServo - servos to set
     * @param - vector of values
     * @returns -vector of values
    */

    ChannelVec channels = getChannels(whichServo);
    IntVec return_vals;
    for (size_t i=0; i<channels.size(); i++) {
        return_vals.push_back(USBServoController::setAcceleration(channels[i], vals[i]));
    }

    return return_vals;
}

// -------------------------------------------------------------------------------

IntVec PanTilt::setSpeed (WhichServo whichServo, IntVec vals) {
    /**
     * Sets the speed of the given servos
     * @param whichServo - servos to set
     * @param - vector of values
     * @returns -vector of values
    */

    ChannelVec channels = getChannels(whichServo);
    IntVec return_vals;
    for (size_t i=0; i<channels.size(); i++) {
        return_vals.push_back(USBServoController::setSpeed(channels[i], vals[i]));
    }

    return return_vals;
}

// -------------------------------------------------------------------------------

IntVec PanTilt::setRelativePos( WhichServo whichServo, 
    FloatVec positions, 
    PositionUnits positionUnits, 
    bool sync,
    float timeout) {     
    
    /**
     * Sets the relative pos of pan and or tilt
     * @param whichServo - enum
     * @param positions - vector of relative positions
     * @param positionUnits - degrees or microseconds
     * @param sync - if true, block until complete
     * @param timeout - seconds to wait 
    */

   ChannelVec channels = getChannels(whichServo);
   return setRelativePosMulti(channels, positions, positionUnits, sync, timeout);

}

// ----------------------------------------------------------------------------------

IntVec PanTilt::returnToHome (WhichServo whichServo, bool sync, float timeout) {
    
    ChannelVec channels = getChannels(whichServo);
    return returnToHomeMulti(channels, sync, timeout);

}

// ------------------------------------------------------------------------------------

std::tuple<float, int> PanTilt::calculateMovementTime (int panDegrees, int tiltDegrees, int fps) {

/**
 * 
 * */     

    cout << "calculateMovementTime" << endl;
    float return_time = 0.0;

    if (panDegrees != 0) {
        return_time = USBServoController::calculateMovementTime(pan, panDegrees);
    }
    if (tiltDegrees != 0) {
        return_time += USBServoController::calculateMovementTime(tilt, tiltDegrees);
    }   

    return std::make_tuple(return_time, ceil(return_time * fps));

}

