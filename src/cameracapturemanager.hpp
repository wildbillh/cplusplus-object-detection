// cameracapturemanager.hpp

#ifndef __CAMERACAPTUREMANAGER_HPP_INCLUDED__
#define __CAMERACAPTUREMANAGER_HPP_INCLUDED__


#include "capturemanager.hpp"
#include<iostream>
#include <unordered_map>
#include <sstream>
#include<opencv2/opencv.hpp>
using namespace std;

using properties = unordered_map<string, double>;
using propmapping = unordered_map<string, int>;

class CameraCaptureManager: public CaptureManager{
    public:
        CameraCaptureManager (void);
        ~CameraCaptureManager (void);
        int open (int, int = cv::CAP_DSHOW);
        bool read (cv::OutputArray&); 
        void setDefaultProperties ();
        void setProperties (properties);
        properties getProperties ();
        string decodeFourccValue (double);
        string printProperties (properties);
        
    protected:
        propmapping property_mapping;  
    private:
       
};

#endif