
#include "serial.hpp"


Serial::Serial () {
    
    /**
     * Initialize the file descriptor to the closed state
    */
    
    fd = -1;
}

// ---------------------------------------------------------------------------
Serial::~Serial () {
    
    /**
     * Automatically close the port on delete.
    */
    
    Serial::close();
}

// ---------------------------------------------------------------------------

void Serial::open (std::string _port) {

    /**
     * Open the virtual port.
     * @param _port - String representing the com port
     * @throws runtime_error - if the port cannot be opened
    */

    port = _port;
    fd = ::open(port.c_str(), O_RDWR | O_NOCTTY);
    if (fd == -1)
    {
        throw std::runtime_error ("Could not open port " + port);
        
    }
    #ifdef _WIN32
        if (_setmode(fd, _O_BINARY) == -1) {
            spdlog::warn ("Unable to set binary mode");
        }
    #else
        struct termios options;
        tcgetattr(fd, &options);
        options.c_iflag &= ~(INLCR | IGNCR | ICRNL | IXON | IXOFF);
        options.c_oflag &= ~(ONLCR | OCRNL);
        options.c_lflag &= ~(ECHO | ECHONL | ICANON | ISIG | IEXTEN);
        tcsetattr(fd, TCSANOW, &options);
    #endif
    
    spdlog::debug("Port " + port + " successfully opened");
    
    return;
}

// ---------------------------------------------------------------------------

void Serial::close () {
    
    /**
     * Close the port if open and set fd to -1
    */

    if (fd != -1) {
        spdlog::debug("closing port " + port);
        ::close(fd);
        fd = -1;
    }
}


// ---------------------------------------------------------------------------------------------

bool Serial::write (unsigned char * command, int size) {
    
    /**
     * Write a command to the com port
     * @param command - bytes to write
     * @param size - size to write
     * @throws runtime_error - if port is not previously opened.
     * @returns - true if successful 
    */

    if (fd == -1) {
        throw std::runtime_error ("Attempt to write to unopen port");
    }
   
    return (::write(fd, command, size) != -1);
    
} 

// -----------------------------------------------------------------------------------------------

bool Serial::read (unsigned char * response, int size) {

    /**
     * Read a response from the port
     * @param response - byte buffer
     * @param size - size of buffer
     * @throws runtime_error if port is not open
     * @returns true if successful
    */
    
    if (fd == -1) {
        throw std::runtime_error ("Attempt to read from unopen port");
    }

    if (::read(fd, response, size) != size) {
        cerr << "Error reading" << endl;
        return false;
    }

    return true;
}


// ----------------------------------------------------------------------------------------------

bool Serial::isOpen () {
    
    /**
     * Weak check to see if the port is open
     * @returns true if open
    */
    
    return (fd != -1);
}