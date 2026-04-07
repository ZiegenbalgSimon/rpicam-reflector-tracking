/* SPDX-License-Identifier: BSD-2-Clause */
/*
 * position_uart_stage.cpp - writes poisition to uart from metadata
 */

#include "core/rpicam_app.hpp"
#include "post_processing_stages/post_processing_stage.hpp"

#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <filesystem>

class PositionUartStage : public PostProcessingStage
{
public:
    PositionUartStage(RPiCamApp *app) : PostProcessingStage(app), fd_(-1) {}

    // close uart upon deconstruction
    ~PositionUartStage() { if(fd_ >= 0) close(fd_); }
    
    char const *Name() const override;

    void Read(boost::property_tree::ptree const &params) override;
    
    void Configure() override;
    
    bool Process(CompletedRequestPtr &completed_request) override;

private:
    // file descriptor for uart connection
    int fd_;

    // variable for uart synchronization
    static constexpr uint8_t SIGNAL_BYTE = 0xAA;

    // struct for coordinates
    #pragma pack(push, 1)
    struct Coordinates
    {
        uint16_t x;
        uint16_t y;
    };
    #pragma pack(pop)

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
};

#define NAME "position_uart"

char const *PositionUartStage::Name() const {
    return NAME;
}

void PositionUartStage::Read(boost::property_tree::ptree const &params)
{
}

void PositionUartStage::Configure()
{
    // once upon starting a rpicam-app with this stage:
    // initialize the uart connection, exists until the rpicam-app is terminated
    // (depending on the system to work on different Raspberry Pis)
    if (std::filesystem::exists("/dev/ttyAMA0")) {
        fd_ = open("/dev/ttyAMA0", O_RDWR | O_NOCTTY);
    }
    else {
        fd_ = open("/dev/serial0", O_RDWR | O_NOCTTY);
    }

    // configure serial port
    struct termios options;
    tcgetattr(fd_, &options);

	options.c_cflag = B115200 | CS8 | CLOCAL | CREAD;
	options.c_iflag = IGNPAR;
	options.c_oflag = 0;
	options.c_lflag = 0;

	tcsetattr(fd_, TCSANOW, &options);
}

bool PositionUartStage::Process(CompletedRequestPtr &completed_request)
{
    // read the marker position (written by marker_tracking_cv_stage) from frame metadata
    float cx = 0.0f, cy = 0.0f;

    completed_request->post_process_metadata.Get("marker_tracking.cx", cx);
    completed_request->post_process_metadata.Get("marker_tracking.cy", cy);

    // multiply position by 10 to improve resolution when transforming float to int
    cx *= 10.0f;
    cy *= 10.0f;

    // transfer coordinates to predefined struct for transmission (float to uint16_t)
    Coordinates coor = {};
    coor.x = static_cast<uint16_t>(cx);
    coor.y = static_cast<uint16_t>(cy);

    // send coordinates
    send_coor(fd_, coor);

    return false;
}

static PostProcessingStage *Create(RPiCamApp *app)
{
    return new PositionUartStage(app);
}

static RegisterStage reg(NAME, &Create);