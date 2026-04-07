mkdir -p ../tracking_videos  # creats output folder if missing
rpicam-vid -t 30s \
           -o ../tracking_videos/tracking%03d.mp4 \
           -c ../configuration/configuration_camera.txt \
           --post-process-libs ../rpicam-apps/build \
           --post-process-file ../configuration/configuration_post_processing/configuration_ethernet.json
# on Raspberry Pi 4 and earlier, output to mp4 container requires --codec libav