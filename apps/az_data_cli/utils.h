#pragma once

#include "az/pen/pen_op.h"

#include <string>

void display_in_opencv_highgui(
        az::pen::PenOp *data,
        const int &w,
        const int &h,
        const float &stroke_width,
        std::string_view win_name = "",
        const int &win_width = 720,
        const int &win_height = 480
);

void save_frames(
        az::pen::PenOp *data,
        const int &w,
        const int &h,
        const float &stroke_width,
        std::string_view image_prefix,
        std::string_view output_directory,
        const int &image_width = 2560,
        const int &image_height = 1440
);
