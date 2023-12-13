
#include "usbservocontroller.hpp"



ServoProperties::ServoProperties (Channel servo, int rangeDegrees) {

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
	target_pos = pos;
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
	stringstream s;
	s << "channel: " << (channel == 99 ? "unset" : to_string(channel)) << endl;
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

USBServoController::USBServoController (string calibrationFile) {
	/**
	 * Sets up the default properties for the number of possible servos
	*/
	
	calibration_file = calibrationFile;

	for (int i=0; i<USBServoController::MAX_SERVOS; i++) {
		properties.push_back(ServoProperties());
	}

#ifdef THREADED
	position_thread = jthread();
#endif 	
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

void USBServoController::sync (ChannelVec activeServos) {
	
	// Store the active servos in the member var
	active_servos = activeServos;
	

	// For each active servo property, set to active and enable if needed.
	for (auto channel : active_servos) {
		properties[channel].active = true;
		syncProperty (channel);
	}
}

// -----------------------------------------------------------------------------------------

void USBServoController::sync (ChannelVec activeServos, vector<ServoProperties> activeProperties) {
	
	/**
	 * Send any commands necessary to sync the controller to the current local settings
	 * for all active channels
	*/

	// Store the active servos in the member var
	active_servos = activeServos;

	// For each active servo, set the given properties
	for (size_t i=0; i<active_servos.size(); i++) {
		
		unsigned char channel = activeServos[i];
		// Replace the stored stored property with the new one
		properties[channel] = activeProperties[i];
		
		// Send the property info to the controller
		properties[channel].active = true;
		syncProperty (channel);	
	}
}

// ---------------------------------------------------------------------------------------

void USBServoController::syncProperty (Channel channel) {

	/**
	 * Sync a particular channels settings with the controller
	*/

	// Get the stored properties for the channel
	ServoProperties *prop = &(properties[channel]);

	// If the servo is disabled, we sync the acceleration and speed vals on the controller
	if (prop->disabled) {
		setAcceleration(channel, prop->acceleration);
		setSpeed (channel, prop->speed);
		setDisabled (channel);
	}
	else {
		// setEnabled sync all of the properties automatically
		setEnabled (channel);
	}
}


// --------------------------------------------------------------------------------------------

bool USBServoController::writeCommand (unsigned char code, Channel channel, string description) {

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

bool USBServoController::writeCommand (unsigned char code, Channel channel, int target, string description) {

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

#ifdef THREADED  
    const lock_guard <mutex> lock (write_mutex);
#endif

	unsigned char command[] = {code, channel, (unsigned char)(target & 0x7F), (unsigned char)(target >> 7 & 0x7F)};

	if (!serial.write(command, sizeof command))
    {
        spdlog::error("error writing: " + description);
        return false;
    }

    return true;
}


// ----------------------------------------------------------------------------------------------

int USBServoController::setAcceleration (Channel channel, int val) {

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

int USBServoController::setPosition (Channel channel, int position) {

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
		properties[channel].target_pos = position;
		return position;
	}

	return -1;

}

// ----------------------------------------------------------------------------------------------------

IntVec USBServoController::setPositionMulti ( 
	ChannelVec channels, 
	IntVec positions) {

	/**
	 * Sets the position value in the settings and controller for multiple channels. Non blocking
	 * @param channels - channels to write to 
	 * @param positions - positions to set
	 * @returns - vector of positions 
	*/

	IntVec returned_pos_list;

	if (channels.size() != positions.size()) {
		cerr << "Mismatched data sent to setPositionMulti" << endl;
	}
	else {
		for (size_t i=0; i<channels.size(); i++) {
			returned_pos_list.push_back(setPosition(channels[i], positions[i]));
		}	
	}

	return returned_pos_list;
	
}

// --------------------------------------------------------------------------------------------

#ifdef THREADED
void USBServoController::setPositionThreaded (vector<pair<Channel, int>> pos_pairs, 
	 bool &done, float timeout) {

	/**
	 * Sets the position value in the settings and controller and spawns a thread to monitor the position
	 * progress. Done is set to true when the target position is acheived.
	 * @param pos_pairs - Vector containing pairs of channel/position 
	 * @param done - Reference value set to true when done.
	 * @param timeout - how long to wait for the position to be acheived in seconds 
	 * @returns - Vector of positions
	*/

	// Capture the scope
	auto that = this;
	
	// This bool will be modified by the thread when it completes
	done = false;

	// Define the lambda
	function<void ()> f = [pos_pairs, &done, timeout, that] () {

		// When all of the bools in the status vector are true or timeout occurs, return.
		vector<bool> status;
		// Get a vector to hold the target servo positions
		IntVec target_positions;

		size_t size = pos_pairs.size();

		// for each channel set the position and set a value in the status vector to false
		for (size_t i=0; i<size; i++) {
			target_positions.push_back(that->setPosition(pos_pairs[i].first, pos_pairs[i].second));
			status.push_back(false);
		}

		// Start a timer
		utils::Timer timer = utils::Timer();

		// Keep looping until all servos are complete
		while (!utils::allTrue(status)) {

			// Get the position of each servo and set the status vector if it's complete. 
			for (size_t i=0; i<size; i++) {
				if (!status[i]) {
					if (that->getPositionFromController(pos_pairs[i].first) == target_positions[i]) {
						status[i] = true;
					}
				}
			}

			// if timeout occurs, break
			if (timer.seconds() > timeout) {
				spdlog::warn("timeout occurred befre setPositionSynch() completion");
				break;
			}
		}
		
		// Set the final status. This value should be checked by the user
		done = true;
	};
	
	// Execute the thread
	position_thread = jthread(move(jthread(f)));
	return;
}

// --------------------------------------------------------------------------------------------

void USBServoController::setPositionThreaded (Channel channel, int position, bool &done, float timeout) {

	/**
	 * Sets the position value in the settings and controller, but blocks until the position is achieved
	 * @param channel - channel to write to 
	 * @param val - acceleration value 0 - 255
	 * @param timeout - how long to wait for the position to be acheived in seconds 
	 * @returns - the value given on success or -1 on failure
	*/

	auto that = this;
	done = false;

	// Declare a lambda to set the position and loop until there
	function<void ()> f = [channel, position, &done, timeout, that] () {
		utils::Timer timer = utils::Timer();
		int pos = that->setPosition(channel, position);
		int interim_pos;
		while ( (interim_pos = that->getPositionFromController(channel)) != pos) {
			utils::sleepMilliseconds(1);
			if (timer.seconds() > timeout) {
				spdlog::warn("timeout occurred befre setPositionSynch() completion");
				break;
			}
		}
		done = true;
	
		that->properties[channel].pos = pos;	
	};

	// Execute the lambda in the thread
	position_thread = jthread(move(jthread(f)));
	
	return;
}
#endif 
// --------------------------------------------------------------------------------------------

int USBServoController::setPositionSync (Channel channel, int position, float timeout) {

	/**
	 * Sets the position value in the settings and controller, but blocks until the position is achieved
	 * @param channel - channel to write to 
	 * @param val - acceleration value 0 - 255
	 * @param timeout - how long to wait for the position to be acheived in seconds 
	 * @returns - the value given on success or -1 on failure
	*/

	
	utils::Timer timer = utils::Timer();
	int pos = setPosition(channel, position);
	int interim_pos;
	while ( (interim_pos = getPositionFromController(channel)) != pos) {
		utils::sleepMilliseconds(5);
		if (timer.seconds() > timeout) {
			spdlog::warn("timeout occurred befre setPositionSynch() completion");
			break;
		}
	}
	properties[channel].pos = pos;
	
	return pos;
}


// -------------------------------------------------------------------------------------------

IntVec USBServoController::setPositionMultiSync (
	ChannelVec channels,
	IntVec positions,
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

// -------------------------------------------------------------------------------------------

int USBServoController::setRelativePos (Channel channel, float val, PositionUnits units, bool sync) {

	/**
	 * Sets the position value in the settings and controller as a delta to the current position
	 * @param channel - channel to write to 
	 * @param val - delta of position
	 * @param units - microseconds or degrees 
	 * @returns - the final position calculated on success or -1 on failure
	*/

	spdlog::debug("setRelativePos - channel: " + to_string((int)channel) + " pos: " + to_string(val));

	int new_pos = calculateRelativePosition (channel, val, units);
	if (sync) {
		 return setPositionSync (channel, new_pos);
	}
	return setPosition (channel, new_pos);
}

// --------------------------------------------------------------------------------------

IntVec USBServoController::setRelativePosMulti (ChannelVec channels, FloatVec positions, 
	PositionUnits units, bool sync, float timeout) {

	/**
	 * Set the relative position of multiple servos
	 * @param channels
	 * @param positions
	 * @param units
	 * @param sync - if true, block
	 * @param timeout - wait time
	*/

	// Get a vector of the new absolute positions
	IntVec abs_positions;
	for (size_t i=0; i<channels.size(); i++) {
		abs_positions.push_back(calculateRelativePosition(channels[i], positions[i], units));
	}

	if (sync) {
		return setPositionMultiSync(channels, abs_positions, timeout);
	}

	return setPositionMulti(channels, abs_positions);

}


// -------------------------------------------------------------------------------------------

int USBServoController::setSpeed (Channel channel, int val) {

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

// ----------------------------------------------------------------------------------------------


int USBServoController::returnToHome (Channel channel, bool sync, float timeout) {
	/**
 	* Set the channel to the defined home position.
	* @param channel - channel to set
	* @param sync - Set to true for blocking behavior
	* @param timeout - How long to wait for blocking call to complete
	* @returns position set
	*/

	int pos = properties[channel].home;
	if (sync) {
		return setPositionSync (channel, pos, timeout);
	}

	return setPosition (channel, pos);
}

// -------------------------------------------------------------------------------------------

vector<int> USBServoController::returnToHomeMulti (ChannelVec channels, bool sync, float timeout) {
	/**
 	* Set the channel to the defined home position.
	* @param channels - vector of channels to set
	* @param sync - Set to true for blocking behavior
	* @param timeout - How long to wait for blocking call to complete
	* @returns - vector of positions set
	*/

	// Get a list of home positions
	vector<int> positions;
	for (size_t i=0; i<channels.size(); i++) {
		positions.push_back(properties[channels[i]].home);
	}

	if (sync) {
		return setPositionMultiSync (channels, positions, timeout);
	}

	return setPositionMulti (channels, positions);
	
}


// ------------------------------------------------------------------------

int USBServoController::getPositionFromController (Channel channel) {

	/**
	 * Gets the position value from the controller
	 * @param channel - channel to write to 
	 * @returns - the position on success or -1 on failure
	*/
#ifdef THREADED	
	const lock_guard <mutex> lock (read_mutex);
#endif

	if (writeCommand(0x90, channel, "getPosition")) {	
		unsigned char response[2];
		if (serial.read(response, 2)) {
			return (response[0] + 256*response[1]) / 4;
		}
	}

	return -1;
}

// ---------------------------------------------------------------------------

void USBServoController::setDisabled (Channel channel) {

	/**
	 * Disables a channel (set position to 0)
	 * @param channel - channel to disable
	*/
	spdlog::debug("disabling channel " + to_string((int)channel));
	setPosition(channel, 0);
	properties[channel].disabled = true;
}

// ---------------------------------------------------------------------------

void USBServoController::setEnabled (Channel channel) {

	/**
	 * Enables a channel. Sets the controller to the settings values. Blocks until complete
	 * @param channel - channel to disable
	*/
	
	spdlog::info("enabling channel " + to_string((int)channel));
	ServoProperties *prop = &(properties[channel]);
	setAcceleration (channel, prop->acceleration);
	setSpeed (channel, prop->speed);
	setPositionSync (channel, prop->pos);
	//utils::sleepMilliseconds(1);
	prop->disabled = false;
}

ServoProperties USBServoController::getChannelProperty (Channel channel) {

	/** 
	 * Gets the servo properties class for a channel
	 * @param channel - channel to get the properties for
	 * @returns ServoProperties class
	*/

	return properties[channel];
}

// ------------------------------------------------------------------------------------

int USBServoController::calculateRelativePosition (Channel channel, float val, PositionUnits units) {

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

// ---------------------------------------------------------------------------------------

bool USBServoController::calibrateServo (Channel channel, bool force) {
	/**
	 * Finds a stored or builds a new calibration vector for the given channel, acc, speed
	 * @param channel 
	 * @param force - recalibrate even if found in file
	 * @returns vector containing the number of seconds to move from 0 to 45 degrees by ones
	*/

	
	// Get a pointer to the properties
	ServoProperties *props = &(properties[channel]);
	// Clear the calibration
	props->calibration.clear();


	// If there is a calibration file, attempt to get the calibration from the file
	if (!calibration_file.empty() && !force) {
		spdlog::info ("Attempt to get calibration from file");
		ServoCalibration sc = ServoCalibration (calibration_file);
		vector<double> cal_vals = sc.get(channel, props->acceleration, props->speed);
		if (!cal_vals.empty()) {
			props->calibration = cal_vals;
			return true;
		}
	}

	// To get here means we need to do a calibration
	spdlog::info("Building calibration for channel: " + to_string((int)channel));
	
	// Move to the center (home) position
	returnToHome (channel, true);
	
	utils::Timer timer = utils::Timer();

	int pos;
	// Calibrate from 0-45 degrees
	for (int i=0; i<46; i++) {
		timer.start();
		pos = i;
		// set every other pos to a negative value
		if (pos % 2) {
			pos = -pos;
		}
		
		// Move the servo and capture the time needed to complete the move
		setRelativePos (channel, pos, PositionUnits::DEGREES, true);
		props->calibration.push_back(timer.seconds());
		spdlog::debug(timer.seconds());
	}

	// Move to center position before exiting
	returnToHome(channel, true);

	// if filename, We need to write the new calibration to the stored json and then the file 
	if (!calibration_file.empty()) {

		ServoCalibration sc = ServoCalibration (calibration_file);
		sc.set(channel, props->acceleration, props->speed, props->calibration);

	}
	return true;
}

// --------------------------------------------------------------------------------------------------

	float USBServoController::calculateMovementTime (Channel channel, int degrees) {

        /**
		 * Retreive the stored time and number of frames needed to move each servo the specified degrees
		 * 
		*/
       
        int deg = abs(degrees) ? abs(degrees) < 45 : 45;
        float seconds = properties[channel].calibration[deg];
        //int frames_to_skip = ceil(fps * seconds);

        return seconds;
	} 