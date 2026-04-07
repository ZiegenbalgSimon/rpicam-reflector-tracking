#include <iostream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>

int main() {
    // variables for ethernet connection
    int sockfd;
    struct sockaddr_in local_addr;
    int port = 8080;

    // variables to process incoming data
    uint8_t received = 0;
    const uint8_t START_FLAG = 0xAA;
    const uint8_t STOP_FLAG = 0xFF;

    // initialize socket (UDP)
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    // configure the socket (blocking, receive from any ip addr)
    memset(&local_addr, 0, sizeof(local_addr));
    local_addr.sin_family = AF_INET;
    local_addr.sin_port = htons(port);
    local_addr.sin_addr.s_addr = INADDR_ANY;
    bind(sockfd, (struct sockaddr*)&local_addr, sizeof(local_addr));

    std::cout << "Waiting to receive byte..." << std::endl;

    // wait until a byte is received (spezial handling of start and stop flag)
    ssize_t recv_size = recvfrom(sockfd, &received, sizeof(received), 0, nullptr, nullptr);

    if (recv_size == sizeof(received)) {
        std::cout << "Received: ";
        if (received == START_FLAG) {
            std::cout << "START" << std::endl;
        }
        else if (received == STOP_FLAG) {
            std::cout << "STOP" << std::endl;
        }
        else {
            std::cout << static_cast<int>(received) << std::endl;
        }
    }
    else {
        std::cerr << "Error receiving byte" << std::endl;
    }

    // terminate the program
    close(sockfd);
    return 0;
}