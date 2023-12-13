#include "servocalibration.hpp"


ServoCalibration::ServoCalibration (std::string fileName) {
    /**
     * Constructor initializes members
     * @param fileName - file to read and write to 
    */
    filename = fileName;
    root = Json::nullValue;
    
}

// ----------------------------------------------------------------------------------------------

std::string ServoCalibration::buildCalibrationString (unsigned char channel, int acceleration, int speed) {
    /**
     * Builds a calibration string of the format: "channel-acceleration-speed"
     * @param channel 
     * @param acceleration
     * @param speed
     * @returns the calibration string
    */
    
    return std::to_string((int)channel) + "-" + std::to_string(acceleration) + "-" + std::to_string(speed);
}

// ----------------------------------------------------------------------------------------------

std::string ServoCalibration::printJsonValues () {
    /**
     * Converts the root JSON to a string
     * @returns - Readable string of the JSON
    */

    Json::StreamWriterBuilder builder;
    return Json::writeString(builder, root);

}

// ---------------------------------------------------------------------------------------------

std::vector<double> ServoCalibration::get (unsigned char channel, int acceleration, int speed) {
    /**
     * Get the calibration values from a stored JSON file. If file not found or 
     * calibration key not found return an empty vector.
     * @param channel
     * @param acceleration
     * @param speed
     * @returns vector of doubles
     * @throws if the string read from the file fails to parse into valid JSON
    */
    
    std::vector<double> cal_values;

    // If we can't open the file, just return the empty vector
    if (!readJsonFromFile()) {
        spdlog::warn ("file: " + filename + " could not be opened");
        return cal_values;
    }

    // Get the key we're looking for
    std::string calibration_string = buildCalibrationString(channel, acceleration, speed);
    
    
    // See if the json contains the values. If not return empty vector. If so populate the return vector.
    Json::Value vals = root[calibration_string];
    if (vals == Json::nullValue) {
        spdlog::info("Key: " + calibration_string + " not found");
    }
    else {
        for (Json::Value::ArrayIndex i=0; i<vals.size(); i++) {
            cal_values.push_back(vals[i].asDouble());
        }
    }

    return cal_values;
    
}

// --------------------------------------------------------------------------------------------------------

void ServoCalibration::set (unsigned char channel, int acceleration, int speed, std::vector<double> cal_values) {
    /**
     * @param channel
     * @param acceleration
     * @param speed
     * @param cal_values - vector of doubles to input into the stored JSON
    */
    
    // Build the key for the values we are inserting
    std::string calibration_string = buildCalibrationString (channel, acceleration, speed);
    
    // Parse the file and populate the root member
    readJsonFromFile ();

    // Build the Json array with our vector values
    Json::Value cal_array = Json::arrayValue;
    for (size_t i=0; i<cal_values.size(); i++) {
        cal_array.append(cal_values[i]);
    }

    // Set the key with the Json array
    root[calibration_string] = cal_array;

    // Write the new JSON to the file
    writeJsonToFile();

}

// --------------------------------------------------------------------------------------------------

bool ServoCalibration::readJsonFromFile () {
    /**
     * Read JSON from a file and set the returned info in the root member
     * @returns true if file is read and parsed
     * @throws runtime exception if file is read and parse fails
    */

    std::ifstream f;
    f.open(filename);
    
    // If the file is not open return false
    if (!f.is_open()) {
        return false;
    }
    
    // Parse the file contentts into JSON
    Json::CharReaderBuilder builder;
    JSONCPP_STRING errs;

    // Throw a runtime error if parse fails
    if (!parseFromStream(builder, f, &root, &errs)) {
        spdlog::error(errs);
        f.close();
        throw std::runtime_error ("Error parsing JSON from file: " + filename);      
    }

    f.close();
    return true;

};

// ----------------------------------------------------------------------------------------------------

void ServoCalibration::writeJsonToFile () {
    /**
     * Write the contents of the root Json to the file
    */

    Json::StreamWriterBuilder builder;
    const std::unique_ptr<Json::StreamWriter> writer(builder.newStreamWriter());
    
    std::ofstream f;
    f.open(filename, std::ofstream::out | std::ofstream::trunc);
    if (!f.is_open()) {
        spdlog::error("Error opening: " + filename);
        return;
    }
    
    writer->write(root, &f);
    f.close();
    return;
};