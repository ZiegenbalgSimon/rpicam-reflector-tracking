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

    // initialize socket (UDP)
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    // configure the socket (blocking, receive from any ip addr)
    memset(&local_addr, 0, sizeof(local_addr));
    local_addr.sin_family = AF_INET;
    local_addr.sin_port = htons(port);
    local_addr.sin_addr.s_addr = INADDR_ANY;
    bind(sockfd, (struct sockaddr*)&local_addr, sizeof(local_addr));

    std::cout << "Waiting to receive start signal..." << std::endl;

    // wait until the start flag is received (from any ip addr)
    while (true){
        if (recvfrom(sockfd, &received, sizeof(received), 0, nullptr, nullptr) > 0) {
            if (received == START_FLAG) {
                std::cout << "Start signal received" << std::endl;
                break;
            }
        }
    }

    // terminate the program
    close(sockfd);
    return 0;
}