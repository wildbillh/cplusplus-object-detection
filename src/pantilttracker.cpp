
#include "pantilttracker.hpp"

TrackerProperties::TrackerProperties (float hSlack, float vSlack, FloatOffset centerOffset, cv::Point frameDims) {
    horizontal_slack = hSlack;
    vertical_slack = vSlack;
    center_offset = FloatOffset(centerOffset);
    frame_dims = cv::Point(frameDims);
}

// ================================================================================================

PanTiltTracker::PanTiltTracker (Channel pan, Channel tilt, std::string calibrationFile, TrackerProperties trackerProps) 
    : PanTilt (pan, tilt, calibrationFile) {

    props = trackerProps;

    int x = props.frame_dims.x / 2 + (int)(std::get<0>(props.center_offset) * props.frame_dims.x / 2);
    int y = props.frame_dims.y / 2 + (int)(std::get<1>(props.center_offset) * props.frame_dims.y / 2);
    frame_center = cv::Point(x,y);
    
    x = frame_center.x - round(frame_center.x * props.horizontal_slack);
    y = frame_center.x + round(frame_center.x * props.horizontal_slack);
    horizontal_slack = std::make_tuple(x,y); 
    
    x = frame_center.y - round(frame_center.y * props.vertical_slack),
    y = frame_center.y + round(frame_center.y * props.vertical_slack),
    vertical_slack = std::make_tuple(x,y);

}

// ----------------------------------------------------------------------------------------------

bool PanTiltTracker::calculateCorrectionDegrees (cv::Point regionCenter, IntOffset &correction) {
    /**
     * Calcuate the degrees of correction to recenter the region of interest
     * @param regionCenter - x,y of center
     * @param correction - Tuple to be filled in
     * @returns bool - Returns true if a correction is needed. 
    */
    
    cout << "in calculateCorrectionDegrees with region center: " << regionCenter << endl;
    
    int x = regionCenter.x;
    int y = regionCenter.y;

    const auto [hs_min, hs_max] = horizontal_slack;
    const auto [vs_min, vs_max] = vertical_slack;
    bool ret_value = false;

    float h_correction = 0.0, v_correction = 0.0;
    
    if ( (x < hs_min) || (x > hs_max)) {
       h_correction = std::atan((x - frame_center.x) / (float)props.frame_dims.y) * 180.0 / M_PI; 
       ret_value = true; 
    }

    if (y < vs_min || y > vs_max) {
        spdlog::info("correcting y " + std::to_string(y));
        float val = (frame_center.y - y) / (float)props.frame_dims.y;
        spdlog::info("atan " + to_string(val));
        v_correction = std::atan( (frame_center.y - y) / (float)props.frame_dims.x) * 180.0 / M_PI;
        ret_value = true;
    }

    correction = make_tuple(h_correction, v_correction);
    return ret_value;

}

// --------------------------------------------------------------------------------------------

// Add calculation of movement time
//std::tuple<float, int> PanTilt::calculateMovementTime (int panDegrees, int tiltDegrees, int fps) {


std::tuple<float, int> PanTiltTracker::correct (cv::Point regionCenter, int fps) {


    cout << "correct: " << regionCenter << endl;

    IntOffset correction;
    if (calculateCorrectionDegrees(regionCenter, correction)) {
        auto [x_correct, y_correct] = correction;
        cout << "calculateCorrectDegrees returns " << x_correct << ", " << y_correct << endl;

        if (y_correct == 0.0) {
            setRelativePos(WhichServo::PAN, FloatVec{(float)x_correct});
        }
        else if (x_correct == 0.0) {
            setRelativePos(WhichServo::TILT, FloatVec{(float)y_correct});   
        }
        else {
            setRelativePos(WhichServo::BOTH, FloatVec{(float)x_correct, (float)y_correct});
        }

        
        return calculateMovementTime (x_correct, y_correct, fps);     
    }

    return std::make_tuple(0.0, 0);   

}