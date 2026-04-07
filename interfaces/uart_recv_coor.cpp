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

// fuction to receive data
bool receive_coor(int fd, Coordinates &coor)
{
    // varable to hold individual incoming bytes
    uint8_t sync_byte = 0;

    // wait until the signal byte comes in (synchronization)
    while (true)
    {
        if (read(fd, &sync_byte, 1) != 1)
            return false;

        if (sync_byte == SIGNAL_BYTE)
            break;
    }

    // receive bytes in the size of Coordinates struct
    uint8_t received_bytes[sizeof(Coordinates)];
    int received_count = 0;
    while (received_count < sizeof(received_bytes))
    {
        int newly_received_count = read(fd, received_bytes + received_count, sizeof(received_bytes) - received_count);
        if (newly_received_count <= 0)
            return false;
        received_count += newly_received_count;
    }

    // decode the received bytes
    coor.x = received_bytes[0] | (received_bytes[1] << 8);
    coor.y = received_bytes[2] | (received_bytes[3] << 8);

    return true;
}

int main() {
    // variable for coordinates
    Coordinates received = {};
    
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

    std::cout << "Waiting to receive coordinates..." << std::endl;

    // decode received coordinates and print to cout
    while(true){
    if (receive_coor(fd, received))
        {
            std::cout << "(" << received.x << ", " << received.y << ")" << std::endl;
        }
    }

    // terminate the program
    close(fd);
    return 0;
}