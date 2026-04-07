/* SPDX-License-Identifier: BSD-2-Clause */
/*
 * position_pwm_stage.cpp - writes poisition to pwm from metadata
 */

#include "core/rpicam_app.hpp"
#include "post_processing_stages/post_processing_stage.hpp"

#include <fstream>
#include <filesystem>

class PositionPwmStage : public PostProcessingStage
{
public:
    PositionPwmStage(RPiCamApp *app) : PostProcessingStage(app) {}

    // disable harware pwm upon deconstruction
    ~PositionPwmStage() {
        if (std::filesystem::exists("/sys/class/pwm/pwmchip0/pwm2")) {
            pwm_write("/sys/class/pwm/pwmchip0/pwm2/enable", 0);
        }
        if (std::filesystem::exists("/sys/class/pwm/pwmchip0/pwm3")) {
            pwm_write("/sys/class/pwm/pwmchip0/pwm3/enable", 0);
        }
    }
    
    char const *Name() const override;

    void Read(boost::property_tree::ptree const &params) override;
    
    void Configure() override;
    
    bool Process(CompletedRequestPtr &completed_request) override;

private:
    // variables to interpret coordinates
    int frame_width_ = 1456;
    int frame_height_ = 1088;

    // pwm period
    int period_ = 100000;

    // define coordinate data structure
    #pragma pack(push, 1)
    struct Coordinates {
        float x;
        float y;
    };
    #pragma pack(pop)

    // function to write to hardware pwm
    void pwm_write(const std::string& path, int value) {
        std::ofstream f(path);
        f << value;
    }
};

#define NAME "position_pwm"

char const *PositionPwmStage::Name() const {
    return NAME;
}

void PositionPwmStage::Read(boost::property_tree::ptree const &params)
{
    // read parameters from JSON file
    period_ = params.get<int>("period", 100000);
    frame_width_ = params.get<int>("frame_width", 1456);
    frame_height_ = params.get<int>("frame_height", 1088);
}

void PositionPwmStage::Configure()
{
    // once upon starting a rpicam-app with this stage:
    // initialize the two hardware pwm channels (GPIO 18, 19) if they aren't initializes yet
    // (written for Raspberry Pi 5, may differ for other devices)
    if (!std::filesystem::exists("/sys/class/pwm/pwmchip0/pwm2"))
    {
        pwm_write("/sys/class/pwm/pwmchip0/export", 2);
    }
    if (!std::filesystem::exists("/sys/class/pwm/pwmchip0/pwm3"))
    {
        pwm_write("/sys/class/pwm/pwmchip0/export", 3);
    }
}

bool PositionPwmStage::Process(CompletedRequestPtr &completed_request)
{
    // create coordinate object
    Coordinates coor = {};

    // read the marker position (written by marker_tracking_cv_stage) from frame metadata
    completed_request->post_process_metadata.Get("marker_tracking.cx", coor.x);
    completed_request->post_process_metadata.Get("marker_tracking.cy", coor.y);

    // set hardware pwm channels with duty_cycle/period == coor/max_coor
    pwm_write("/sys/class/pwm/pwmchip0/pwm2/enable", 0);
    pwm_write("/sys/class/pwm/pwmchip0/pwm2/period", period_);
    pwm_write("/sys/class/pwm/pwmchip0/pwm2/duty_cycle", (int)(period_ * coor.x / frame_width_));
    pwm_write("/sys/class/pwm/pwmchip0/pwm2/enable", 1);

    pwm_write("/sys/class/pwm/pwmchip0/pwm3/enable", 0);
    pwm_write("/sys/class/pwm/pwmchip0/pwm3/period", period_);
    pwm_write("/sys/class/pwm/pwmchip0/pwm3/duty_cycle", (int)(period_ * coor.y / frame_height_));
    pwm_write("/sys/class/pwm/pwmchip0/pwm3/enable", 1);

    return false;
}

static PostProcessingStage *Create(RPiCamApp *app)
{
    return new PositionPwmStage(app);
}

static RegisterStage reg(NAME, &Create);