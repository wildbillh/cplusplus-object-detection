#include "capturemanager.hpp"

CaptureManager::CaptureManager () {
    cap = nullptr;
}

// -----------------------------------------------------------------------

CaptureManager::~CaptureManager () {
    close();
}

// -----------------------------------------------------------------------

int CaptureManager::open (int source, int api /*=CAP_DSHOW*/) {
    //! Attempt to open the camera. Throws an exception if open fails
    
    cap = new cv::VideoCapture(source, api);
    if (!cap->isOpened()) {
      throw ("No video stream detected");
    }
 
    return 0;
}

// -----------------------------------------------------------------------

void CaptureManager::close (void) {
    if (cap) {
        cout << "releasing" << endl;
        cap->release();
        delete cap;
    }
    else {
        cout << "not allocated" << endl;
    }
}


