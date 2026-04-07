#include <iostream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>

int main() {
    // variables for ethernet connection
    int sockfd;
    struct sockaddr_in local_addr, remote_addr;
    int port = 8080;

    // variables for message content
    uint8_t message = 0;
    const uint8_t START_FLAG = 0xAA;
    const uint8_t STOP_FLAG = 0xFF;

    // request input
    int input = 0;
    std::cout << "Enter a byte to send (0-255, 170 for START, 255 for STOP): ";
    std::cin >> input;

    if(input < 0 || input > 255) {
        std::cerr << "Invalid input. Sending 0..." << std::endl;
        message = 0;
    }
    else {
        message = static_cast<uint8_t>(input);
    }

    // initialize socket (UDP)
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    // configure the socket
    memset(&local_addr, 0, sizeof(local_addr));
    memset(&remote_addr, 0, sizeof(remote_addr));
    remote_addr.sin_family = AF_INET;
    remote_addr.sin_port = htons(port);

    // create (dummy) connection (to get local ip address)
    inet_pton(AF_INET, "192.168.1.1", &remote_addr.sin_addr);
    connect(sockfd, (struct sockaddr*)&remote_addr, sizeof(remote_addr));

    // read out local ip address
    socklen_t socklen = sizeof(local_addr);
    getsockname(sockfd, (struct sockaddr*)&local_addr, &socklen);

    // choose remote ip address depending on local ip address
    in_addr addr1, addr2;
    inet_pton(AF_INET, "192.168.1.1", &addr1);
    inet_pton(AF_INET, "192.168.1.2", &addr2);

    if (local_addr.sin_addr.s_addr == addr1.s_addr) {
        remote_addr.sin_addr = addr2;
    }
    else {
        remote_addr.sin_addr = addr1;
    }

    // send byte
    sendto(sockfd, &message, 1, 0, (struct sockaddr*)&remote_addr, sizeof(remote_addr));

    // read out remote ip address for cout
    char remote_ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &remote_addr.sin_addr, remote_ip, sizeof(remote_ip));

    if (message == START_FLAG) {
        std::cout << "START";
    }
    else if (message == STOP_FLAG) {
        std::cout << "STOP";
    }
    else {
        std::cout << static_cast<int>(message);
    }
    std::cout << " sent to " << remote_ip << std::endl;

    // terminate the program
    close(sockfd);
    return 0;
}