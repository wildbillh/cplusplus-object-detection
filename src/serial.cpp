
#include "serial.hpp"

Serial::Serial () {
    fd = -1;
}

// ---------------------------------------------------------------------------
Serial::~Serial () {
    Serial::close();
}

// ---------------------------------------------------------------------------

bool Serial::open (std::string port) {

    fd = ::open(port.c_str(), O_RDWR | O_NOCTTY);
    if (fd == -1)
    {
        std::cerr << "Could not open port: " << port << std::endl;
        return false;
    }
    #ifdef _WIN32
        if (_setmode(fd, _O_BINARY) == -1) {
            std::cerr << "Warning: issue setting mode" << std::endl;
        }
    #else
        struct termios options;
        tcgetattr(fd, &options);
        options.c_iflag &= ~(INLCR | IGNCR | ICRNL | IXON | IXOFF);
        options.c_oflag &= ~(ONLCR | OCRNL);
        options.c_lflag &= ~(ECHO | ECHONL | ICANON | ISIG | IEXTEN);
        tcsetattr(fd, TCSANOW, &options);
    #endif
    
    return true;
}

// ---------------------------------------------------------------------------

void Serial::close () {

    if (fd != -1) {
        cout << "closing port" << endl;
        ::close(fd);
        fd = -1;
    }
}


// ---------------------------------------------------------------------------------------------

bool Serial::write (unsigned char * command, int size) {
    if (fd == -1) {
        cerr << "port is not open" << endl;
        return false;
    }
   
    return (::write(fd, command, size) != -1);
    
} 

bool Serial::read (unsigned char * response, int size) {
    
    //unsigned char resp [2];
    if (::read(fd, response, size) != size) {
        cerr << "Error reading" << endl;
        return false;
    }

    return true;
}


// ----------------------------------------------------------------------------------------------

bool Serial::isOpen () {
    return (fd != -1);
}