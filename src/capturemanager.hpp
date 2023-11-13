// capturemanager.hpp

#ifndef __CAPTUREMANAGER_HPP_INCLUDED__
#define __CAPTUREMANAGER_HPP_INCLUDED__

#include<iostream>
#include<opencv2/opencv.hpp>
using namespace std;

class CaptureManager {
    public:
        CaptureManager (void);
        ~CaptureManager (void);
        int open(int, int api=cv::CAP_DSHOW);
        void close(void);
        
    protected:
        cv::VideoCapture* cap; 
};

#endif