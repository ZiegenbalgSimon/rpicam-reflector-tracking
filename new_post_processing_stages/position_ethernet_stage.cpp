/* SPDX-License-Identifier: BSD-2-Clause */
/*
 * position_ethernet_stage.cpp - sends position through ethernet from metadata
 */

#include "core/rpicam_app.hpp"
#include "post_processing_stages/post_processing_stage.hpp"

#include <unistd.h>
#include <cstring>
#include <arpa/inet.h>
#include <fcntl.h>
#include <signal.h>

class PositionEthernetStage : public PostProcessingStage
{
public:
    PositionEthernetStage(RPiCamApp *app) : PostProcessingStage(app), sockfd_(-1), remote_ip_("192.168.1.2"), port_(8080) {}

    // close ethernet socket upon deconstruction
    ~PositionEthernetStage() { if (sockfd_ >= 0) close(sockfd_); }
    
    char const *Name() const override;

    void Read(boost::property_tree::ptree const &params) override;
    
    void Configure() override;
    
    bool Process(CompletedRequestPtr &completed_request) override;

private:
    // variables for ethernet connection
    int sockfd_;
    struct sockaddr_in local_addr_, remote_addr_;
    std::string remote_ip_;
    int port_;

    // variable to interpret incoming data
    static constexpr uint8_t STOP_FLAG = 0xFF;

    // struct for coordinates
    #pragma pack(push, 1)
    struct Coordinates
    {
        uint16_t x;
        uint16_t y;
    };
    #pragma pack(pop)
};

#define NAME "position_ethernet"

char const *PositionEthernetStage::Name() const {
    return NAME;
}

void PositionEthernetStage::Read(boost::property_tree::ptree const &params)
{
    // read parameters from JSON file
    remote_ip_ = params.get<std::string>("ip", "192.168.1.2");
    port_ = params.get<int>("port", 8080);
}

void PositionEthernetStage::Configure()
{
    // once upon starting a rpicam-app with this stage:
    // initialize socket (UDP), exists until the rpicam-app is terminated
    sockfd_ = socket(AF_INET, SOCK_DGRAM, 0);

    // configure the socket
    fcntl(sockfd_, F_SETFL, O_NONBLOCK);  // set non-blocking

    // configure local port for receiving
    memset(&local_addr_, 0, sizeof(local_addr_));
    local_addr_.sin_family = AF_INET;
    local_addr_.sin_port = htons(port_);
    local_addr_.sin_addr.s_addr = INADDR_ANY;

    bind(sockfd_, (struct sockaddr*)&local_addr_, sizeof(local_addr_));

    // configure remote port for sending
    memset(&remote_addr_, 0, sizeof(remote_addr_));
    remote_addr_.sin_family = AF_INET;
    remote_addr_.sin_port = htons(port_);

    inet_pton(AF_INET, remote_ip_.c_str(), &remote_addr_.sin_addr);
}

bool PositionEthernetStage::Process(CompletedRequestPtr &completed_request)
{
    // variable to store incoming data
    uint8_t ethernet_in = 0;

    // if a stop flag is received, send a signal to terminate the running rpicam-app
    if (recvfrom(sockfd_, &ethernet_in, sizeof(ethernet_in), 0, nullptr, nullptr) > 0) {
        if (ethernet_in == STOP_FLAG) {
            raise(SIGTERM);
        }
    }
    
    // read the marker position (written by marker_tracking_cv_stage) from frame metadata
    float cx = 0.0f, cy = 0.0f;

    completed_request->post_process_metadata.Get("marker_tracking.cx", cx);
    completed_request->post_process_metadata.Get("marker_tracking.cy", cy);

    // multiply position by 10 to improve resolution when transforming float to int
    cx *= 10.0f;
    cy *= 10.0f;

    // transfer coordinates to predefined struct for transmission (float to uint16_t)
    Coordinates coor = {};
    coor.x = htons(static_cast<uint16_t>(cx));
    coor.y = htons(static_cast<uint16_t>(cy));

    // send coordinates
    sendto(sockfd_, &coor, sizeof(coor), 0, (struct sockaddr *)&remote_addr_, sizeof(remote_addr_));

    return false;
}

static PostProcessingStage *Create(RPiCamApp *app)
{
    return new PositionEthernetStage(app);
}

static RegisterStage reg(NAME, &Create);
