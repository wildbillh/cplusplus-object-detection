#include <iostream>
#include <spdlog/spdlog.h>
#include<opencv2/opencv.hpp>//OpenCV header to use VideoCapture class//

#include "cameracapturemanager.hpp"
#include "usbservocontroller.hpp"
#include "serial.hpp"



using namespace std;

int main() {


    try {
        
        spdlog::set_level(spdlog::level::debug);
        spdlog::set_pattern("[%^%l%$] %v");
        spdlog::info("hello there");
        USBServoController controller = USBServoController();
        controller.open("COM4");
          
        std::vector<unsigned char> active_servos = {0,2};
        
        ServoProperties pan = ServoProperties(0);
        pan.speed = 15;
        pan.disabled = false;
        
        ServoProperties tilt = ServoProperties(2);
        tilt.speed = 15;
        tilt.disabled = false;

        
        std::vector<ServoProperties> active_props = {pan, tilt};

       
        controller.sync (active_servos, active_props);
        
        
        controller.setRelativePos (0, -20.0, PositionUnits::DEGREES, true);
        controller.setRelativePos (0, 40.0, PositionUnits::DEGREES, true);
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
    catch (const char* msg) {
        cerr << "Exception: " << msg << endl;
    }
}