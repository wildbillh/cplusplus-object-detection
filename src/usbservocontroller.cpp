
#include "usbservocontroller.hpp"

ServoProperties::ServoProperties (unsigned char servo, int rangeDegrees) {
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

	return s.str();

}



USBServoController::USBServoController () {
	for (int i=0; i<USBServoController::MAX_SERVOS; i++) {
		properties.push_back(ServoProperties());
	}
	//active_servos = {};
	//number_of_active_servos = 0;
	//cout << sizeof (active_servos) << endl;
}

USBServoController::~USBServoController () {
	// Disable all active servos
	for (unsigned char servo : active_servos) {
		cout << "disable " << (int)servo << endl;
		setDisabled(servo);
	}

	close();	
}

void USBServoController::close () {
	if (serial.isOpen()) {
		serial.close();
	}
	
}


// --------------------------------------------------------------------------------

bool USBServoController::open (string port) {

	serial = Serial();
	if (serial.open(port)) {
	}
	else {
		printf("Error.\n");
		return false;
	}

    return true;
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
    
    unsigned char command[] = {code, channel};
    
    if (!serial.write(command, sizeof command))
    {
        cerr << "error writing: " << description << endl;
        return false;
    }

    return true;
}



// -----------------------------------------------------------------------------------------------

bool USBServoController::writeCommand (unsigned char code, unsigned char channel, int target, string description) {

    
    unsigned char command[] = {code, channel, (unsigned char)(target & 0x7F), (unsigned char)(target >> 7 & 0x7F)};

    if (!serial.write(command, sizeof command))
    {
        cerr << "error writing: " << description << endl;
        return false;
    }

    return true;
}


// ----------------------------------------------------------------------------------------------

int USBServoController::setAcceleration (unsigned char channel, int val) {
	
	if (writeCommand(0x89, channel, val, "setAcceleration") ) {
		properties[channel].acceleration = val;
		return val;
	}

	return -1;

}

// ------------------------------------------------------------------------

int USBServoController::setPosition (unsigned char channel, int position) {

	int quarter_ms = position * 4;
	
	if (writeCommand(0x84, channel, quarter_ms, "setPosition")) {
		properties[channel].pos = position;
		return position;
	}

	return -1;

}

std::vector<int> USBServoController::setPositionMulti ( 
	std::vector<unsigned char> channels, 
	std::vector<int> positions) {

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

	int new_pos = calculateRelativePosition (channel, val, units);
	if (sync) {
		 return setPositionSync (channel, new_pos);
	}
	return setPosition (channel, new_pos);
}


// -------------------------------------------------------------------------------------------


int USBServoController::setSpeed (unsigned char channel, int val) {
	
	if (writeCommand(0x87, channel, val, "setSpeed")) {
		properties[channel].speed = val;
		return val;
	}

	return -1;

}

// ------------------------------------------------------------------------

int USBServoController::getPositionFromController (unsigned char channel) {
	
	
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
	spdlog::debug("disabling channel " + to_string((int)channel));
	setPosition(channel, 0);
	properties[channel].disabled = true;
}

// ---------------------------------------------------------------------------

void USBServoController::setEnabled (unsigned char channel) {
	ServoProperties prop = properties[channel];
	setAcceleration (channel, prop.acceleration);
	setSpeed (channel, prop.speed);
	setPositionSync (channel, prop.pos);
	prop.disabled = false;
}

ServoProperties USBServoController::getChannelProperty (unsigned char channel) {
	return properties[channel];
}

// ------------------------------------------------------------------------------------

int USBServoController::calculateRelativePosition (unsigned char channel, float val, PositionUnits units) {

	int diff_ms = 0;
	if (units == PositionUnits::MICROSECONDS) {
		diff_ms = (int)val;
	}
	else if (units == PositionUnits::DEGREES) {
		diff_ms = (int) (val * properties[channel].microseconds_per_degree);
	}

	return properties[channel].pos + diff_ms; 
}