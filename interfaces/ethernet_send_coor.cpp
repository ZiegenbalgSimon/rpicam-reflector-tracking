#include <iostream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>

// struct for coordinates
#pragma pack(push, 1)
struct Coordinates
{
    uint16_t x;
    uint16_t y;
};
#pragma pack(pop)

int main() {
    // variables for ethernet connection
    int sockfd;
    struct sockaddr_in local_addr, remote_addr;
    int port = 8080;

    // variable for coordinates
    Coordinates coor = {};

    // request input
    int input = 0;
    std::cout << "Enter coordinates to send (0-65535)." << std::endl;
    std::cout << "X: ";
    std::cin >> input;

    if(input < 0 || input > 65535) {
        std::cerr << "Invalid input. X set to 0." << std::endl;
        coor.x = htons(0);
    }
    else {
        coor.x = htons(input);
    }
    
    input = 0;
    std::cout << "Y: ";
    std::cin >> input;

    if(input < 0 || input > 65535) {
        std::cerr << "Invalid input. Y set to 0." << std::endl;
        coor.y = htons(0);
    }
    else {
        coor.y = htons(input);
    }

    // initialize socket (UDP)
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    // configure the socket
    memset(&local_addr, 0, sizeof(local_addr));
    memset(&remote_addr, 0, sizeof(remote_addr));
    remote_addr.sin_family = AF_INET;
    remote_addr.sin_port = htons(port);

    // create connection (to get local ip address)
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

    // send coordinates
    sendto(sockfd, &coor, sizeof(coor), 0, (struct sockaddr *)&remote_addr, sizeof(remote_addr));

    // read out remote ip address for cout
    char remote_ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &remote_addr.sin_addr, remote_ip, sizeof(remote_ip));

    std::cout << "(" << ntohs(coor.x) << ", " << ntohs(coor.y) << ") sent to " << remote_ip << std::endl;

    // terminate the program
    close(sockfd);
    return 0;
}