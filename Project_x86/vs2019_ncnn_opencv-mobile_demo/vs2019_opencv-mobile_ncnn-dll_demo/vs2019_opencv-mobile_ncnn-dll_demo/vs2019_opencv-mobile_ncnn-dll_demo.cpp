#include "net.h"
#include <algorithm>
#include <opencv2/opencv.hpp>
#include <vector>
#include <iostream>
#include "wechat_qrcode.hpp"

using namespace std;

int main()
{
    cv::Mat img = cv::imread("assert/test.png");

    cv::Ptr<cv::wechat_qrcode::WeChatQRCode> detector = cv::makePtr<cv::wechat_qrcode::WeChatQRCode>("assert/detect.param", "assert/detect.bin", "assert/sr.param", "assert/sr.bin");

    vector<cv::Mat> points;
    vector<string> res = detector->detectAndDecode(img, points);
    for (const auto& t : res) cout << t << endl;

    return 0;
}
