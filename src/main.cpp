#include <iostream>
#include <spdlog/spdlog.h>
#include <json/json.h>
#include<opencv2/opencv.hpp>//OpenCV header to use VideoCapture class//

#include "cameracapturemanager.hpp"
//#include "usbservocontroller.hpp"
//#include "pantilt.hpp"
#include "pantilttracker.hpp"
#include "servocalibration.hpp"
#include "serial.hpp"
#include <thread>



using namespace std;

int main() {


    try {
        cout << "threads: " << std::thread::hardware_concurrency() << endl;
        
        // Set up the logger
        spdlog::set_level(spdlog::level::info);
        spdlog::set_pattern("[%^%l%$] %v");

        
        // Get and open a controller, passing in a calibration file.
        //USBServoController controller = USBServoController("cal.json");

        TrackerProperties tracker_props = TrackerProperties (0.03, 0.04, FloatOffset(0.0, 0.0), cv::Point(1600,896));
        PanTiltTracker controller = PanTiltTracker(0, 2, "cal.json", tracker_props);
        controller.open("COM4");
          
        //std::vector<unsigned char> active_servos = {0,2};
        
        ServoProperties pan = ServoProperties(0);
        pan.acceleration = 5;
        pan.speed = 15;
        pan.disabled = false;
        
        ServoProperties tilt = ServoProperties(2);
        tilt.acceleration = 5;
        tilt.speed = 15;
        tilt.disabled = false;

        controller.sync(pan, tilt);

        if (controller.calibrate(WhichServo::BOTH, false)) {
            spdlog::info("Calibration Successful");
        }
    
        CameraCaptureManager cm = CameraCaptureManager();
        cm.open(0);
        properties props = cm.getProperties();
        cout << cm.printProperties(props) << endl;
        cv::Mat frame;

        cv::dnn::Net net = cv::dnn::readNetFromDarknet ("dnn_model/yolov4-tiny.cfg", "dnn_model/yolov4-tiny.weights");
        net.setPreferableBackend(cv::dnn::DNN_BACKEND_OPENCV);
        net.setPreferableTarget(cv::dnn::DNN_TARGET_CPU);
        
        cv::dnn::DetectionModel model = cv::dnn::DetectionModel(net);
        model.setInputParams(1.0/255, cv::Size(320,320));
        IntVec class_ids;
        FloatVec confidences;
        std::vector<cv::Rect> boxes;

        //cv::Point center = cv::Point(1600 / 2, 896 / 2);
        //cv::Point center = cv::Point(400, 300);
        IntOffset correction;

       
        int skipFrames = 0;
        
        while (cm.read(frame)) {

            model.detect(frame, class_ids, confidences, boxes);
            
            // Draw a rect for the best candidate where class_id == 0
            for (size_t i=0; i<class_ids.size(); i++) {   
                //cout << class_ids[i] << endl;     
                if (class_ids[i] == 66) {
                    auto center = boxes[i].tl() + cv::Point(boxes[i].width / 2, boxes[i].height /2);
                    //cout << center << endl;
                    if (true) {//(skipFrames == 0) {
                        auto [seconds, frames_to_skip] = controller.correct(center);
                        skipFrames = frames_to_skip; 
                        cout << "seconds: " << seconds << ", skipframes: " << skipFrames << endl;         
                    }
                    else {
                        skipFrames--;
                    }

                    cv::drawMarker(frame, center, cv::Scalar(255,0,0), cv::MARKER_CROSS, 200, 3);
                    cv::rectangle(frame, boxes[i], cv::Scalar(255,0,0), 2, cv::LINE_8);
                    break;
                }         
            }
            
            cv::drawMarker(frame, cv::Point(800, 448), cv::Scalar(255,255,0), cv::MARKER_CROSS, 200, 4);
            cv::imshow("Video Player", frame);//Showing the video//
            char c = (char)cv::waitKey(25);//Allowing 25 milliseconds frame processing time and initiating break condition//
            if (c == 27){ //If 'Esc' is entered break the loop//
                break;
            }
        }
        
        controller.returnToHome(WhichServo::BOTH, true);
        return 0;
    }
    catch (const std::exception & e) {
        spdlog::error(e.what());
        spdlog::error("Program terminating");
    }
}