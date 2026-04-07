// udp_receiver.cpp
#include <iostream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>

// define coordinate data structure
#pragma pack(push, 1)
struct Coordinates {
    uint16_t x;
    uint16_t y;
};
#pragma pack(pop)

int main() {
    // variables for ethernet connection
    int sockfd;
    struct sockaddr_in local_addr;
    int port = 8080;

    // variable for coordinates
    Coordinates received = {};

    // initialize socket (UDP)
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    // configure the socket (blocking, receive from any ip addr)
    memset(&local_addr, 0, sizeof(local_addr));
    local_addr.sin_family = AF_INET;
    local_addr.sin_port = htons(port);
    local_addr.sin_addr.s_addr = INADDR_ANY;
    bind(sockfd, (struct sockaddr*)&local_addr, sizeof(local_addr));

    std::cout << "Waiting to receive coordinates..." << std::endl;

    // decode received coordinates and print to cout
    while (true) {
        ssize_t recv_size = recvfrom(sockfd, &received, sizeof(received), 0, nullptr, nullptr);
        if (recv_size == sizeof(received)) {
            received.x = ntohs(received.x);
            received.y = ntohs(received.y);

            std::cout << "(" << received.x << ", " << received.y << ")" << std::endl;
        }
        else {
            std::cerr << "Error receiving coordinates" << std::endl; 
        }
    }

    // terminate the program
    close(sockfd);
    return 0;
}