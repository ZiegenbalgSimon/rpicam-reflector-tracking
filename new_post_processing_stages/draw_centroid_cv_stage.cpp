/* SPDX-License-Identifier: BSD-2-Clause */
/*
 * draw_centroid_cv_stage.cpp - draws position of centroid from metadata
 */

#include <libcamera/stream.h>

#include "core/rpicam_app.hpp"
#include "post_processing_stages/post_processing_stage.hpp"

#include "opencv2/core.hpp"
#include "opencv2/imgproc.hpp"

using namespace cv;

using Stream = libcamera::Stream;

class DrawCentroidCvStage : public PostProcessingStage
{
public:
    DrawCentroidCvStage(RPiCamApp *app) : PostProcessingStage(app) {}

    char const *Name() const override;

    void Read(boost::property_tree::ptree const &params) override;
    
    void Configure() override;
    
    bool Process(CompletedRequestPtr &completed_request) override;

private:
    Stream *stream_;
    int radius_ = 16;
    int thickness_ = 2;
};

#define NAME "draw_centroid_cv"

char const *DrawCentroidCvStage::Name() const {
    return NAME;
}

void DrawCentroidCvStage::Read(boost::property_tree::ptree const &params)
{
    // read parameters from JSON file
    radius_ = params.get<int>("radius", 16);
    thickness_ = params.get<int>("thickness", 2);
}

void DrawCentroidCvStage::Configure()
{
    // once upon starting a rpicam-app with this stage:
    // choose the video stream to use (YUV420 format)
    stream_ = app_->GetMainStream();
    if (!stream_ || stream_->configuration().pixelFormat != libcamera::formats::YUV420)
        throw std::runtime_error("DrawCentroidCvStage: only YUV420 format supported");
}

bool DrawCentroidCvStage::Process(CompletedRequestPtr &completed_request)
{
    // read the marker position (written by marker_tracking_cv_stage) from frame metadata
    float cx = 0.0f, cy = 0.0f;

    completed_request->post_process_metadata.Get("marker_tracking.cx", cx);
    completed_request->post_process_metadata.Get("marker_tracking.cy", cy);

    if(!(cx == 0.0f && cy == 0.0f)){  // only draw the position if a marker was found
        // everything needed for working with the video stream
        StreamInfo info = app_->GetStreamInfo(stream_);
        BufferWriteSync w(app_, completed_request->buffers[stream_]);
        libcamera::Span<uint8_t> buffer = w.Get()[0];
        uint8_t *ptr = (uint8_t *)buffer.data();

        // create an OpenCV Mat object from the Y plane (brightness)
        Mat y_plane(info.height, info.width, CV_8U, ptr, info.stride);
        // create a OpenCV Mat object from the U plane and V plane (color, half resolution)
        uint8_t *u_ptr = ptr + info.stride * info.height;
        uint8_t *v_ptr = u_ptr + (info.stride / 2) * (info.height / 2);
        Mat u_plane(info.height / 2, info.width / 2, CV_8U, u_ptr, info.stride / 2);
        Mat v_plane(info.height / 2, info.width / 2, CV_8U, v_ptr, info.stride / 2);

        // draw the marker position in the Y plane (point + circle, OpenCV functions)
        Point center(cvRound(cx), cvRound(cy));
        circle(y_plane, center, radius_, Scalar(76), thickness_);
        circle(y_plane, center, 2, Scalar(76), FILLED);

        // draw the marker position in the UV planes (point + circle, OpenCV functions)
        Point uv_center(center.x / 2, center.y / 2);
        circle(u_plane, uv_center, radius_ / 2, Scalar(84), thickness_ / 2);
        circle(v_plane, uv_center, radius_ / 2, Scalar(255), thickness_ / 2);
        circle(u_plane, uv_center, 1, Scalar(84), FILLED);
        circle(v_plane, uv_center, 1, Scalar(255), FILLED);
    }

    return false;
}

static PostProcessingStage *Create(RPiCamApp *app)
{
    return new DrawCentroidCvStage(app);
}

static RegisterStage reg(NAME, &Create);
