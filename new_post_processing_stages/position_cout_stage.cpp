/* SPDX-License-Identifier: BSD-2-Clause */
/*
 * position_cout_stage.cpp - writes poisition to cout from metadata
 */

#include "core/rpicam_app.hpp"
#include "post_processing_stages/post_processing_stage.hpp"

class PositionCoutStage : public PostProcessingStage
{
public:
    PositionCoutStage(RPiCamApp *app) : PostProcessingStage(app) {}

    char const *Name() const override;

    void Read(boost::property_tree::ptree const &params) override;
    
    void Configure() override;
    
    bool Process(CompletedRequestPtr &completed_request) override;

private:
    // struct for coordinates
    #pragma pack(push, 1)
    struct Coordinates
    {
        uint16_t x;
        uint16_t y;
    };
    #pragma pack(pop)
};

#define NAME "position_cout"

char const *PositionCoutStage::Name() const {
    return NAME;
}

void PositionCoutStage::Read(boost::property_tree::ptree const &params)
{
}

void PositionCoutStage::Configure()
{
}

bool PositionCoutStage::Process(CompletedRequestPtr &completed_request)
{
    // read the marker position (written by marker_tracking_cv_stage) from frame metadata
    float cx = 0.0f, cy = 0.0f;

    completed_request->post_process_metadata.Get("marker_tracking.cx", cx);
    completed_request->post_process_metadata.Get("marker_tracking.cy", cy);

    // multiply position by 10 to improve resolution when transforming float to int
    cx *= 10.0f;
    cy *= 10.0f;

    // transfer coordinates to predefined struct for coherence with transmitting stages
    Coordinates coor = {};
    coor.x = static_cast<uint16_t>(cx);
    coor.y = static_cast<uint16_t>(cy);


    // write the determined position to cout
    std::cout << "(" << static_cast<int>(coor.x) << ", " << static_cast<int>(coor.y) << ")" << std::endl;

    return false;
}

static PostProcessingStage *Create(RPiCamApp *app)
{
    return new PositionCoutStage(app);
}

static RegisterStage reg(NAME, &Create);