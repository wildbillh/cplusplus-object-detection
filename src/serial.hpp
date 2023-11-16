
#pragma once

#include <fcntl.h>
#include <sys/stat.h>
#include <stdio.h>
#include <unistd.h>
#include <iostream>
#include <stdexcept>
#include <spdlog/spdlog.h>

#ifdef _WIN32
    #define O_NOCTTY 0
#else
    #include <termios.h>
#endif

using namespace std;

/**
 * Class which supplies methods to open, close, read and write to a virtual com port.
*/
class Serial {

    public:
        Serial ();
        ~Serial ();
        void open (std::string);
        void close();
        bool read (unsigned char *, int);
        bool write (unsigned char *, int);
        bool isOpen ();
    protected:
        int fd;
        string port;
};