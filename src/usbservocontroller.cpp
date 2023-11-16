
#include "usbservocontroller.hpp"



ServoProperties::ServoProperties (unsigned char servo, int rangeDegrees) {

	/**
	 * Initialize a default servo property class
	 * @ param servo - the channel to set up. Defaults to 99
	 * @ param rangeDegrees - The range of degrees that the min and max microseconds represent. Default to 120
	*/

	channel = servo;
 	min = 992;
    max = 2000;
    home = 1500;
	pos = 1500;
	speed = 200;
    acceleration = 0;
	disabled = true;
	active = false;
	range_degrees = rangeDegrees;
	microseconds_per_degree = (max - min ) / range_degrees;
}

// -----------------------------------------------------------------------------------

string ServoProperties::print () {

	/**
	 * Build a string representing the state of the servo properties
	*/
	std::stringstream s;
	s << "channel: " << (channel == 99 ? "unset" : std::to_string(channel)) << endl;
	s << "min: " << min << endl;
	s << "max: " << max << endl;
	s << "home: " << home << endl;
	s << "pos: " << pos << endl;
	s << "acceleration: " << acceleration << endl;
	s << "speed: " << speed << endl;
	s << "disabled: " << (disabled ? "true" : "false") << endl;
	s << "active: " << (active ? "true" : "false") << endl;
	s << "range (degrees): " << range_degrees << endl;

	return s.str();

}

// ----------------------------------------------------------------------------------

USBServoController::USBServoController () {
	/**
	 * Sets up the default properties for the number of possible servos
	*/
	
	for (int i=0; i<USBServoController::MAX_SERVOS; i++) {
		properties.push_back(ServoProperties());
	}
	
}

// -----------------------------------------------------------------------------------

USBServoController::~USBServoController () {
	/**
	 * Disable all active servos before the class is destroyed
	*/
	
	for (unsigned char servo : active_servos) {
		setDisabled(servo);
	}

	close();	
}

// -------------------------------------------------------------------------------

void USBServoController::close () {
	/**
	 * Close the serial port used to communicate with the controller
	*/
	if (serial.isOpen()) {
		serial.close();
	}
	
}


// --------------------------------------------------------------------------------

void USBServoController::open (string port) {

	/**
	 * Open the given string representing a virtual COM port
	 * 
	*/

	serial = Serial();
	// if this call fails, an exception is thrown
	serial.open(port);

}

// -----------------------------------------------------------------------------------------

void USBServoController::sync (std::vector <unsigned char> activeServos) {
	
	// Store the active servos in the member var
	active_servos = activeServos;
	

	// For each active servo property, set to active and enable if needed.
	for (auto channel : active_servos) {
		properties[channel].active = true;
		syncProperty (channel);
	}
}

// -----------------------------------------------------------------------------------------

void USBServoController::sync (std::vector<unsigned char> activeServos, std::vector<ServoProperties> activeProperties) {
	
	/**
	 * Send any commands necessary to sync the controller to the current local settings
	 * for all active channels
	*/

	// Store the active servos in the member var
	active_servos = activeServos;

	// For each active servo, set the given properties
	for (int i=0; i<active_servos.size(); i++) {
		
		unsigned char channel = activeServos[i];
		// Replace the stored stored property with the new one
		properties[channel] = activeProperties[i];
		
		// Send the property info to the controller
		properties[channel].active = true;
		syncProperty (channel);	
	}
}

// ---------------------------------------------------------------------------------------

void USBServoController::syncProperty (unsigned char channel) {

	/**
	 * Sync a particular channels settings with the controller
	*/

	// Get the stored properties for the channel
	ServoProperties prop = properties[channel];

	// If the servo is disabled, we sync the acceleration and speed vals on the controller
	if (prop.disabled) {
		setAcceleration(channel, prop.acceleration);
		setSpeed (channel, prop.speed);
		setDisabled (channel);
	}
	else {
		// setEnabled sync all of the properties automatically
		setEnabled (channel);
	}
}


// --------------------------------------------------------------------------------------------

bool USBServoController::writeCommand (unsigned char code, unsigned char channel, string description) {

	/**
	 * Write a command to a particular channel.
	 * This method is used typically to get info from the controller 
	 * since it doesn't pass any data.
	 * @param code byte value recognized by the controller
	 * @param channel - channel to send to 
	 * @param description - description of the command to be used for troubleshooting
	 * @returns - true if successful 
	 * @throws - runtime_error if port is not open
	*/
    
	unsigned char command[] = {code, channel};
    
    if (!serial.write(command, sizeof command))
    {
        spdlog::error("error writing: " + description);
        return false;
    }

    return true;
}

// -----------------------------------------------------------------------------------------------

bool USBServoController::writeCommand (unsigned char code, unsigned char channel, int target, string description) {

	/**
	 * Write a command to a particular channel.
	 * Passes data to the controller
	 * @param code byte value recognized by the controller
	 * @param channel - channel to send to 
	 * @param target - the data to pass. This value is chopped into 2 bytes
	 * @param description - description of the command to be used for troubleshooting
	 * @returns - true if successful
	 * @throws - runtime_error if port is not open
	*/
    
    unsigned char command[] = {code, channel, (unsigned char)(target & 0x7F), (unsigned char)(target >> 7 & 0x7F)};

    if (!serial.write(command, sizeof command))
    {
        spdlog::error("error writing: " + description);
        return false;
    }

    return true;
}


// ----------------------------------------------------------------------------------------------

int USBServoController::setAcceleration (unsigned char channel, int val) {

	/**
	 * Sets the acceleration value in the settings and controller
	 * @param channel - channel to write to 
	 * @param val - acceleration value 0 - 255
	 * @returns - the value given on success or -1 on failure
	*/
	
	spdlog::debug ("Setting acceleration of channel " + to_string((int)channel) + " to " + to_string(val));

	if (writeCommand(0x89, channel, val, "setAcceleration") ) {
		properties[channel].acceleration = val;
		return val;
	}

	return -1;

}

// ------------------------------------------------------------------------

int USBServoController::setPosition (unsigned char channel, int position) {

	/**
	 * Sets the position value in the settings and controller in a non blocking way
	 * @param channel - channel to write to 
	 * @param val - acceleration value 0 - 255
	 * @returns - the value given on success or -1 on failure
	*/

	int new_pos = position;
	// Make sure we don't try to reach a position outside of the property boundaries
	if (new_pos != 0) {
		if (new_pos < properties[channel].min) {
			new_pos = properties[channel].min;
		}
		else if (new_pos > properties[channel].max) {
			new_pos = properties[channel].max;
		}
	}

	spdlog::debug ("Setting position of channel " + to_string((int)channel) + " to " + to_string(new_pos));

	// Convert to quarter ms
	int quarter_ms = new_pos * 4;
	
	if (writeCommand(0x84, channel, quarter_ms, "setPosition")) {
		properties[channel].pos = position;
		return position;
	}

	return -1;

}

// ----------------------------------------------------------------------------------------------------

std::vector<int> USBServoController::setPositionMulti ( 
	std::vector<unsigned char> channels, 
	std::vector<int> positions) {

	/**
	 * Sets the position value in the settings and controller for multiple channels. Non blocking
	 * @param channels - channels to write to 
	 * @param positions - positions to set
	 * @returns - vector of positions 
	*/

	std::vector<int> returned_pos_list;

	if (channels.size() != positions.size()) {
		cerr << "Mismatched data sent to setPositionMulti" << endl;
	}
	else {
		for (int i=0; i<channels.size(); i++) {
			returned_pos_list.push_back(setPosition(channels[i], positions[i]));
		}	
	}

	return returned_pos_list;
	

}

// --------------------------------------------------------------------------------------------

int USBServoController::setPositionSync (unsigned char channel, int position, float timeout) {

	/**
	 * Sets the position value in the settings and controller, but blocks until the position is achieved
	 * @param channel - channel to write to 
	 * @param val - acceleration value 0 - 255
	 * @param timeout - how long to wait for the position to be acheived in seconds 
	 * @returns - the value given on success or -1 on failure
	*/

	utils::Timer timer = utils::Timer();
	int pos = setPosition(channel, position);
	while (getPositionFromController(channel) != pos) {
		utils::sleepMilliseconds(1);
		if (timer.seconds() > timeout) {
			cerr << "Warning: timeout occurred befre setPositionSynch() completion" << endl;
			break;
		}
	}
	return position;
}


// -------------------------------------------------------------------------------------------

std::vector<int> USBServoController::setPositionMultiSync (
	std::vector<unsigned char> channels,
	std::vector<int> positions,
	float timeout) {

	/**
	 * Sets the position value in the settings and controller for multiple channels, but blocks until 
	 * the position is achieved
	 * @param channels - channels to write to 
	 * @param positions - positions to set
	 * @param timeout - how long to wait for the position to be acheived in seconds 
	 * @returns - the values given on success or -1 on failure
	*/

	int number_of_channels = channels.size();

	setPositionMulti (channels, positions);
	bool status [number_of_channels] = {false}; 
	auto timer = utils::Timer();

	while (!utils::allTrue(status, number_of_channels))  {
		
		for (int i=0; i<number_of_channels; i++) {
			if (!status[i]) {
				if (getPositionFromController(channels[i]) == positions[i]) {
					status[i] = true;
				}
			}
		}
		if (timer.seconds() >= timeout) {
			spdlog::warn("Timeout in setPostionMultiSync");
			break;
		}

	} 

	return positions;
}


int USBServoController::setRelativePos (unsigned char channel, float val, PositionUnits units, bool sync) {

	/**
	 * Sets the position value in the settings and controller as a delta to the current position
	 * @param channel - channel to write to 
	 * @param val - delta of position
	 * @param units - microseconds or degrees 
	 * @returns - the final position calculated on success or -1 on failure
	*/

	int new_pos = calculateRelativePosition (channel, val, units);
	if (sync) {
		 return setPositionSync (channel, new_pos);
	}
	return setPosition (channel, new_pos);
}


// -------------------------------------------------------------------------------------------

int USBServoController::setSpeed (unsigned char channel, int val) {

	/**
	 * Sets the speed value in the settings and controller
	 * @param channel - channel to write to 
	 * @param val - speed value 0 - 255
	 * @returns - the value given on success or -1 on failure
	*/
	
	
	spdlog::debug ("Setting speed of channel " + to_string((int)channel) + " to " + to_string(val));
	
	if (writeCommand(0x87, channel, val, "setSpeed")) {
		properties[channel].speed = val;
		return val;
	}

	return -1;

}

// ------------------------------------------------------------------------

int USBServoController::getPositionFromController (unsigned char channel) {

	/**
	 * Gets the position value from the controller
	 * @param channel - channel to write to 
	 * @returns - the position on success or -1 on failure
	*/
	
	if (writeCommand(0x90, channel, "getPosition")) {	
		unsigned char response[2];
		if (serial.read(response, 2)) {
			return (response[0] + 256*response[1]) / 4;
		}
	}

	return -1;
}

// ---------------------------------------------------------------------------

void USBServoController::setDisabled (unsigned char channel) {

	/**
	 * Disables a channel (set position to 0)
	 * @param channel - channel to disable
	*/
	spdlog::debug("disabling channel " + to_string((int)channel));
	setPosition(channel, 0);
	properties[channel].disabled = true;
}

// ---------------------------------------------------------------------------

void USBServoController::setEnabled (unsigned char channel) {

	/**
	 * Enables a channel. Sets the controller to the settings values. Blocks until complete
	 * @param channel - channel to disable
	*/
	
	spdlog::debug("enabling channel " + to_string((int)channel));
	ServoProperties prop = properties[channel];
	setAcceleration (channel, prop.acceleration);
	setSpeed (channel, prop.speed);
	setPositionSync (channel, prop.pos);
	prop.disabled = false;
}

ServoProperties USBServoController::getChannelProperty (unsigned char channel) {

	/** 
	 * Gets the servo properties class for a channel
	 * @param channel - channel to get the properties for
	 * @returns ServoProperties class
	*/

	return properties[channel];
}

// ------------------------------------------------------------------------------------

int USBServoController::calculateRelativePosition (unsigned char channel, float val, PositionUnits units) {

	/**
	 * Calculates a new position based on the current value and the given delta
	 * @param channel = channel to calculate
	 * @param val - delta to calculate
	 * @param units - microseconds or degrees
	 * @returns - position in microseconds
	*/

	int diff_ms = 0;
	if (units == PositionUnits::MICROSECONDS) {
		diff_ms = (int)val;
	}
	else if (units == PositionUnits::DEGREES) {
		diff_ms = (int) (val * properties[channel].microseconds_per_degree);
	}

	return properties[channel].pos + diff_ms; 
}