#include <iostream>
#include <spdlog/spdlog.h>
#include <json/json.h>
#include<opencv2/opencv.hpp>//OpenCV header to use VideoCapture class//

#include "cameracapturemanager.hpp"
//#include "usbservocontroller.hpp"
#include "pantilt.hpp"
#include "servocalibration.hpp"
#include "serial.hpp"



using namespace std;

int main() {


    try {
        
        // Set up the logger
        spdlog::set_level(spdlog::level::info);
        spdlog::set_pattern("[%^%l%$] %v");

        
        // Get and open a controller, passing in a calibration file.
        //USBServoController controller = USBServoController("cal.json");
        PanTilt controller = PanTilt(0, 2, "cal.json");
        controller.open("COM4");
          
        //std::vector<unsigned char> active_servos = {0,2};
        
        ServoProperties pan = ServoProperties(0);
        pan.acceleration = 15;
        pan.speed = 15;
        pan.disabled = false;
        
        ServoProperties tilt = ServoProperties(2);
        tilt.acceleration = 15;
        tilt.speed = 15;
        tilt.disabled = false;

        controller.sync(pan, tilt);

        if (controller.calibrate(WhichServo::BOTH, false)) {
            spdlog::info("Calibration Successful");
        }

        
        controller.setRelativePos(WhichServo::BOTH, FloatVec {-30.0, -30.0}, PositionUnits::DEGREES, true);
        controller.returnToHome(WhichServo::BOTH, true);
        controller.setRelativePos(WhichServo::BOTH, FloatVec {30.0, 30.0}, PositionUnits::DEGREES, true);
        controller.returnToHome(WhichServo::BOTH, true);
        /*
        std::vector<ServoProperties> active_props = {pan, tilt};
       
        controller.sync (active_servos, active_props);
        
        ServoProperties pan_props = controller.getChannelProperty(0);
        controller.calibrateServo(0, true);
        controller.calibrateServo(2, true);
        spdlog::info ("Calibration complete");
        */

        //ServoCalibration sv = ServoCalibration ("cal.json");
        //std::vector<double> calibration = sv.get(pan_props.channel, pan_props.acceleration, pan_props.speed);
        //if (calibration.empty()) {
            //;
            //calibration = controller.getChannelProperty(0).calibration;
            
            //sv.set(pan_props.channel, pan_props.acceleration, pan_props.speed, calibration);
        //}

        //std::string calibration_string = controller.calibrateServo(0);
        //std::vector<double> calibration = controller.getChannelProperty(0).calibration;
        
        //controller.setRelativePos (0, -20.0, PositionUnits::DEGREES, true);
        //controller.setRelativePos (0, 40.0, PositionUnits::DEGREES, true);
        
        //controller.setPositionMultiSync (std::vector<unsigned char>{0,2},  std::vector<int>{1000, 1000}); 
        //controller.setPositionMultiSync (std::vector<unsigned char>{0,2},  std::vector<int>{1500, 1500});
        


        /*
        controller.setPosition(5, 1500);
        sleep(1);
        int pos = controller.getPositionFromController(5);
        cout << "pos = " << pos << endl;
        sleep(1);
        controller.setAcceleration(5, 10);
        controller.setSpeed(5, 10);
        controller.setPosition(5, 2000);
        sleep(1);
        controller.setDisabled(5);
        
    
        CameraCaptureManager cm = CameraCaptureManager();
        cm.open(0);
        properties props = cm.getProperties();
        cout << cm.printProperties(props) << endl;
        cv::Mat frame;


        while (cm.read(frame)) {
            
            cv::imshow("Video Player", frame);//Showing the video//
            char c = (char)cv::waitKey(25);//Allowing 25 milliseconds frame processing time and initiating break condition//
            if (c == 27){ //If 'Esc' is entered break the loop//
                break;
            }
        }
        */
            
        return 0;
    }
    catch (const std::exception & e) {
        spdlog::error(e.what());
        spdlog::error("Program terminating");
    }
}