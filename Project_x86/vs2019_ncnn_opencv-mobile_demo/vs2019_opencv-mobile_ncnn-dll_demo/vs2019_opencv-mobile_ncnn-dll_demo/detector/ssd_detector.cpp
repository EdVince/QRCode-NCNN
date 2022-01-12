// This file is part of OpenCV project.
// It is subject to the license terms in the LICENSE file found in the top-level directory
// of this distribution and at http://opencv.org/license.html.
//
// Tencent is pleased to support the open source community by making WeChat QRCode available.
// Copyright (C) 2020 THL A29 Limited, a Tencent company. All rights reserved.
#include "../precomp.hpp"
#include "ssd_detector.hpp"
#define CLIP(x, x1, x2) max(x1, min(x, x2))
namespace cv {
namespace wechat_qrcode {
int SSDDetector::init(const string& proto_path, const string& model_path) {
    net_.load_param(proto_path.c_str());
    net_.load_model(model_path.c_str());
    return 0;
}

vector<Mat> SSDDetector::forward(Mat img, const int target_width, const int target_height) {
    int img_w = img.cols;
    int img_h = img.rows;
    Mat input;
    resize(img, input, Size(target_width, target_height), 0, 0, INTER_CUBIC);

    ncnn::Mat ncnn_input = ncnn::Mat::from_pixels(input.data, ncnn::Mat::PIXEL_GRAY, input.cols, input.rows);
    const float norm_vals[3] = { 1.f / 255.f, 1.f / 255.f, 1.f / 255.f };
    ncnn_input.substract_mean_normalize(0, norm_vals);
    ncnn::Extractor ex = net_.create_extractor();
    ex.input("data", ncnn_input);

    ncnn::Mat prob;
    ex.extract("detection_output", prob);

    std::vector<cv::Mat> point_list;
    for (int row = 0; row < prob.h; row++) {
        float* prob_score = (float*)prob.channel(0) + 6 * row;
        if (prob_score[0] == 1 && prob_score[1] > 1E-5) {
            auto point = cv::Mat(4, 2, CV_32FC1);
            float x0 = CLIP(prob_score[2] * img_w, 0.0f, img_w - 1.0f);
            float y0 = CLIP(prob_score[3] * img_h, 0.0f, img_h - 1.0f);
            float x1 = CLIP(prob_score[4] * img_w, 0.0f, img_w - 1.0f);
            float y1 = CLIP(prob_score[5] * img_h, 0.0f, img_h - 1.0f);

            point.at<float>(0, 0) = x0;
            point.at<float>(0, 1) = y0;
            point.at<float>(1, 0) = x1;
            point.at<float>(1, 1) = y0;
            point.at<float>(2, 0) = x1;
            point.at<float>(2, 1) = y1;
            point.at<float>(3, 0) = x0;
            point.at<float>(3, 1) = y1;
            point_list.push_back(point);
        }
    }
    return point_list;
}
}  // namespace wechat_qrcode
}  // namespace cv