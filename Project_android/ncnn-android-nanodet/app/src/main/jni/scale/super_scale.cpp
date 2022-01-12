// This file is part of OpenCV project.
// It is subject to the license terms in the LICENSE file found in the top-level directory
// of this distribution and at http://opencv.org/license.html.
//
// Tencent is pleased to support the open source community by making WeChat QRCode available.
// Copyright (C) 2020 THL A29 Limited, a Tencent company. All rights reserved.
#include "../precomp.hpp"
#include "super_scale.hpp"
#include "cpu.h"

#define CLIP(x, x1, x2) max(x1, min(x, x2))
namespace cv {
namespace wechat_qrcode {
int SuperScale::init(AAssetManager* mgr) {

    ncnn::set_cpu_powersave(2);
    ncnn::set_omp_num_threads(ncnn::get_big_cpu_count());
    srnet_.opt = ncnn::Option();
    srnet_.opt.num_threads = ncnn::get_big_cpu_count();

    srnet_.load_param(mgr, "sr.param");
    srnet_.load_model(mgr, "sr.bin");
    net_loaded_ = true;
    return 0;
}

Mat SuperScale::processImageScale(const Mat &src, float scale, const bool &use_sr,
                                  int sr_max_size) {
    Mat dst = src;
    if (scale == 1.0) {  // src
        return dst;
    }

    int width = src.cols;
    int height = src.rows;
    if (scale == 2.0) {  // upsample
        int SR_TH = sr_max_size;
        if (use_sr && (int)sqrt(width * height * 1.0) < SR_TH && net_loaded_) {
            int ret = superResoutionScale(src, dst);
            if (ret == 0) return dst;
        }

        { resize(src, dst, Size(), scale, scale, INTER_CUBIC); }
    } else if (scale < 1.0) {  // downsample
        resize(src, dst, Size(), scale, scale, INTER_AREA);
    }

    return dst;
}

int SuperScale::superResoutionScale(const Mat &src, Mat &dst) {
    ncnn::Mat blob = ncnn::Mat::from_pixels(src.data, ncnn::Mat::PIXEL_GRAY, src.cols, src.rows);
    const float norm_vals[3] = { 1.f / 255.f };
    blob.substract_mean_normalize(0, norm_vals);

    ncnn::Extractor ex = srnet_.create_extractor();
    ex.input("data", blob);

    ncnn::Mat prob;
    ex.extract("fc", prob);

    dst = Mat(prob.w, prob.h, CV_8UC1);

    int cnt = 0;
    for (int row = 0; row < prob.h; row++) {
        for (int col = 0; col < prob.w; col++) {
            float pixel = prob[cnt++] * 255.0;
            dst.at<uint8_t>(row, col) = static_cast<uint8_t>(CLIP(pixel, 0.0f, 255.0f));
        }
    }

    return 0;
}
}  // namespace wechat_qrcode
}  // namespace cv