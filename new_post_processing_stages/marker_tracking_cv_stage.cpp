/* SPDX-License-Identifier: BSD-2-Clause */
/*
 * marker-tracking - Intensity Weighted Centroid
 */

#include <libcamera/stream.h>

#include "core/rpicam_app.hpp"
#include "post_processing_stages/post_processing_stage.hpp"

#include "opencv2/core.hpp"
#include "opencv2/imgproc.hpp"

#include <iostream>

using namespace cv;

using Stream = libcamera::Stream;

class MarkerTrackingCvStage : public PostProcessingStage
{
public:
    MarkerTrackingCvStage(RPiCamApp *app) : PostProcessingStage(app) {}

    char const *Name() const override;

    void Read(boost::property_tree::ptree const &params) override;
    
    void Configure() override;

    bool Process(CompletedRequestPtr &completed_request) override;

private:
    Stream *stream_;
    int threshold_ = 150;
};

#define NAME "marker_tracking_cv"

char const *MarkerTrackingCvStage::Name() const {
    return NAME;
}

void MarkerTrackingCvStage::Read(boost::property_tree::ptree const &params)
{
    // read parameters from JSON file
    threshold_ = params.get<int>("threshold", 150);
}

void MarkerTrackingCvStage::Configure()
{
    // once upon starting a rpicam-app with this stage:
    // choose the video stream to use (YUV420 format)
    stream_ = app_->GetMainStream();
    if (!stream_ || stream_->configuration().pixelFormat != libcamera::formats::YUV420)
        throw std::runtime_error("MarkerTrackingCvStage: only YUV420 format supported");
}

bool MarkerTrackingCvStage::Process(CompletedRequestPtr &completed_request)
{
    // everything needed for working with the video stream
    StreamInfo info = app_->GetStreamInfo(stream_);
    BufferWriteSync w(app_, completed_request->buffers[stream_]);
    libcamera::Span<uint8_t> buffer = w.Get()[0];
    uint8_t *ptr = (uint8_t *)buffer.data();

    // create an OpenCV Mat object from the Y plane (brightness)
    Mat y_plane(info.height, info.width, CV_8U, ptr, info.stride);

    // remove pixels below a brightness threshold (OpenCV function)
    threshold(y_plane, y_plane, threshold_, 255, THRESH_TOZERO);

    // calculate moments (up to third order)
    Moments m = moments(y_plane, false);

    // calculate intensity weighted centroid from moments
    float cx = 0.0;
    float cy = 0.0;

    if (m.m00 != 0.0) {
        cx = m.m10 / m.m00;
        cy = m.m01 / m.m00;
    }

    // write determined pixel position to metadata of the frame
    completed_request->post_process_metadata.Set("marker_tracking.cx", cx);
    completed_request->post_process_metadata.Set("marker_tracking.cy", cy);

    return false;
}

static PostProcessingStage *Create(RPiCamApp *app)
{
    return new MarkerTrackingCvStage(app);
}

static RegisterStage reg(NAME, &Create);
