#pragma once

#include "pantilt.hpp"
#include <opencv2/opencv.hpp>
#include <math.h>

typedef std::tuple<float,float> FloatOffset;
typedef std::tuple<int,int> IntOffset;

class TrackerProperties {
    public:
        TrackerProperties (float=0.02, float=0.02, FloatOffset=FloatOffset(0.0, 0.0), cv::Point=cv::Point(1600,896));
        float horizontal_slack;
        float vertical_slack;
        FloatOffset center_offset;
        cv::Point frame_dims;
       
};


class PanTiltTracker : public PanTilt {

    public:
        PanTiltTracker (Channel, Channel, std::string, TrackerProperties = TrackerProperties());
        TrackerProperties props;
        IntOffset horizontal_slack;
        IntOffset vertical_slack;
        cv::Point frame_center;
        bool calculateCorrectionDegrees (cv::Point, IntOffset &);
        std::tuple<float, int> correct (cv::Point, int = 30);
};