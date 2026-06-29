mkdir -p ../captured_frames  # creats output folder if missing
rpicam-jpeg -o ../captured_frames/frame_$(date +%Y%m%d_%H%M%S).jpeg \
            -c ../configuration/configuration_camera.txt \
            -t 1
# on Raspberry Pi 4 and earlier, output to mp4 container requires --codec libav
