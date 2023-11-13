#include "cameracapturemanager.hpp"


CameraCaptureManager::CameraCaptureManager (void) {
}

// -------------------------------------------------------------------------

CameraCaptureManager::~CameraCaptureManager () {

}

// -------------------------------------------------------------------------

int CameraCaptureManager:: open (int source, int api) {
    // Call the base class to open the camera
    CaptureManager::open(source, api);

    // Grab a single frame as a test
    cv::Mat frame;
    if (!cap->read(frame)) {
        throw ("Issue getting first frame");
    }
  
    frame.release();

    // Set the default properties
    setDefaultProperties();
    return 0;
}

// -------------------------------------------------------------------------

bool CameraCaptureManager:: read (cv::OutputArray& frame) {
    if (!cap) {
        return false;
    }

    return cap->read(frame);
}

// -------------------------------------------------------------------------

void CameraCaptureManager::setDefaultProperties () {
    
    property_mapping["width"] = cv::CAP_PROP_FRAME_WIDTH;
    property_mapping["height"] = cv::CAP_PROP_FRAME_HEIGHT;
    property_mapping["fps"] = cv::CAP_PROP_FPS;
    property_mapping["fourcc"] = cv::CAP_PROP_FOURCC;

    properties default_props;
    default_props["fourcc"] = cv::VideoWriter::fourcc('M', 'J', 'P', 'G');
    default_props["fps"] = 30.0;
    default_props["width"] = 1600.0;
    default_props["height"] = 896.0;
    
    setProperties(default_props);

}

// -------------------------------------------------------------------------

properties CameraCaptureManager::getProperties () {

    properties props;
    propmapping::iterator it = property_mapping.begin(); 
    while (it != property_mapping.end()) {

        double val = cap->get(it->second);
        props[it->first] = val;
        it++;
    }

    return props;  
}

// -------------------------------------------------------------------------

void CameraCaptureManager::setProperties (properties props) {
    //! Set the camera properties from the given properties map
    
    
    // Iterate through the passed properties
    properties::iterator it = props.begin();
    while (it != props.end()) {
        string key = it->first;
        // Look for the same key in the property_mapping map
        if (property_mapping.find(key) == property_mapping.end()) {
            cerr << "Unexpected property: " << key << " found";
        }
        else {
            // The property key matches one in the property map
                   
            // Don't set the property if the value is zero
            if (props[key] != 0.0) {
                cap->set(property_mapping[key], it->second);
                //cout << property_mapping[key] << ", " << it->second << endl;
            } 
        }
        it++; 
    }
}

// -------------------------------------------------------------------------

string CameraCaptureManager::decodeFourccValue (double val) {
    //! Decode a double into a readable 4 character string

    int fourcc = (int)val;
    return cv::format("%c%c%c%c", fourcc & 255, (fourcc >> 8) & 255, (fourcc >> 16) & 255, (fourcc >> 24) & 255);  
}

// -------------------------------------------------------------------------

string CameraCaptureManager::printProperties(properties props) {
    //! Convert a properties map to a printable string

    ostringstream oss;
    for (auto it = props.begin(); it != props.end(); it++) {
        if (it->first.compare("fourcc") == 0) {
            oss << "fourcc: " << string(decodeFourccValue(it->second)) << endl;;           
        }
        else {
            oss << it->first << " : " << it->second << endl;
        }
    }
    return oss.str();
}
