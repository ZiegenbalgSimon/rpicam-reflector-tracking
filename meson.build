project('new_postproc', 'cpp')

libcamera_dep = dependency('libcamera')
opencv_dep = dependency('opencv4')

transmission_postproc_src = files([
  'new_post_processing_stages/position_cout_stage.cpp',
  'new_post_processing_stages/position_ethernet_stage.cpp',
  'new_post_processing_stages/position_pwm_stage.cpp',
  'new_post_processing_stages/position_uart_stage.cpp'
])

transmission_postproc_lib = shared_module(
  'transmission-postproc',
  transmission_postproc_src,
  dependencies: [libcamera_dep]
)

opencv_postproc_srcs = ([
  'new_post_processing_stages/marker_tracking_cv_stage.cpp',
  'new_post_processing_stages/draw_centroid_cv_stage.cpp'
])

opencv_postproc_lib = shared_module(
  'opencv-postproc',
  opencv_postproc_srcs,
  dependencies: [libcamera_dep, opencv_dep]
)
