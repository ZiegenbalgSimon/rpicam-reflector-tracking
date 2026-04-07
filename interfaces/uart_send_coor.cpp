#include <iostream>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <filesystem>

// define coordinate data structure
#pragma pack(push, 1)
struct Coordinates {
    uint16_t x;
    uint16_t y;
};
#pragma pack(pop)

// variable for uart synchronization
const uint8_t SIGNAL_BYTE = 0xAA;

// fuction to send data
void send_coor(int fd, Coordinates &coor)
{
    uint8_t send_buffer[1 + sizeof(Coordinates)];

    // package the data in the format used by the receiver
    send_buffer[0] = SIGNAL_BYTE;
    send_buffer[1] = coor.x & 0xFF;
    send_buffer[2] = (coor.x >> 8) & 0xFF;
    send_buffer[3] = coor.y & 0xFF;
    send_buffer[4] = (coor.y >> 8) & 0xFF;

    // send data
    write(fd, send_buffer, sizeof(send_buffer));
}

int main() {
    // variable for coordinates
    Coordinates coor = {};

    // request input
    int input = 0;
    std::cout << "Enter coordinates to send (0-65535)." << std::endl;
    std::cout << "X: ";
    std::cin >> input;

    if(input < 0 || input > 65535) {
        std::cerr << "Invalid input. X set to 0." << std::endl;
        coor.x = 0;
    }
    else {
        coor.x = input;
    }
    
    input = 0;
    std::cout << "Y: ";
    std::cin >> input;

    if(input < 0 || input > 65535) {
        std::cerr << "Invalid input. Y set to 0." << std::endl;
        coor.y = 0;
    }
    else {
        coor.y = input;
    }

    // initialize the uart connection
    // (depending on the system to work on different Raspberry Pis)
    int fd;
    if (std::filesystem::exists("/dev/ttyAMA0")) {
        fd = open("/dev/ttyAMA0", O_RDWR | O_NOCTTY);
    }
    else {
        fd = open("/dev/serial0", O_RDWR | O_NOCTTY);
    }

    if (fd < 0) {
        std::cerr << "Error opening UART. Terminating program.";
        return 1;
    }

    // configure serial port
    struct termios options;
    tcgetattr(fd, &options);

	options.c_cflag = B115200 | CS8 | CLOCAL | CREAD;
	options.c_iflag = IGNPAR;
	options.c_oflag = 0;
	options.c_lflag = 0;

	tcsetattr(fd, TCSANOW, &options);
    
    // send coordinates
    send_coor(fd, coor);

    // terminate the program
    close(fd);
    return 0;
}
