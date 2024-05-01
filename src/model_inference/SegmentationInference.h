#ifndef SEGMENTATIONINFERENCE_H
#define SEGMENTATIONINFERENCE_H

#include "BaseInference.h"

class SegmentationInference : public BaseInference
{
public:
    SegmentationInference(string model_path, int width, int height, int classes, int device=0);

    ~SegmentationInference();

    cv::Mat do_inference(cv::Mat& input_image);     //图像深度学习推理过程

private:
    const int color_list[5][3] =    //颜色列表RGB，黑色为闸室墙，红色为水体，绿色为船舶
    {
        {   0,   0,   0 },
        { 128,   0,   0 },
        {   0, 128,   0 },
        { 128, 128,   0 },
        {   0,   0, 128 }
    };
};

#endif // SEGMENTATIONINFERENCE_H
