#include<opencv2/opencv.hpp>//OpenCV header to use VideoCapture class//
#include "cameracapturemanager.hpp"
#include "usbservocontroller.hpp"
#include "serial.hpp"

#include<iostream>

using namespace std;

int main() {

    try {
        


        USBServoController controller = USBServoController();
        controller.open("COM4");
        
        std::vector<unsigned char> active_servos = {4,5};
        ServoProperties pan = ServoProperties();
        pan.channel = 4;
        pan.disabled = false;
        ServoProperties tilt = ServoProperties();
        tilt.channel = 5;
        tilt.disabled = false;

        std::vector<ServoProperties> active_props = {pan, tilt};

        controller.sync (active_servos, active_props);
        sleep(1);

        for (unsigned char channel = 0; channel < 6; channel++) {
            cout << controller.getChannelProperty(channel).print() << endl;
        }
        
        controller.setPosition(5, 1000);
        sleep(2);

        cout << controller.getChannelProperty(5).print() << endl;

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