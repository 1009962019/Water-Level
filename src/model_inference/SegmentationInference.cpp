/****************************************************************/
/*                     SegmentationInference                    */
/*               语义分割派生类，继承于基类BaseInference             */
/*             包含整个推理过程中的后处理(转换成掩膜图像)功能           */
/****************************************************************/

#include "SegmentationInference.h"

SegmentationInference::SegmentationInference(string model_path, int width, int height, int classes, int device) : BaseInference(model_path, width, height, classes, device)
{

}

SegmentationInference::~SegmentationInference()
{

}

/**
 * @brief SegmentationInference::do_inference 图像深度学习推理过程
 * @param input_image 输入图像
 * @return 输出掩膜图像
 */
cv::Mat SegmentationInference::do_inference(cv::Mat& input_image)
{
    model_inference(input_image);   //执行前处理和前向推理

    //后处理
    cv::Mat output(input_h, input_w, CV_8UC3, cv::Scalar(0, 0, 0));
    for (int row = 0; row < input_h; row++)
    {
        for (int col = 0; col < input_w; col++)
        {
            float max_number = -10;
            int max_idx = 0;
            for (int cls = 0; cls < num_classes; cls++)
            {
                int basic_pos = cls * input_h * input_w + row * input_w + col;
                if (prob[basic_pos] > max_number)   //对图像中的每一个像素点取分类置信度最高的一类作为最终结果
                {
                    max_number = prob[basic_pos];
                    max_idx = cls;
                }
            }
            //根据类别和颜色列表修改输出掩膜图像在对应位置的RGB值
            output.at<cv::Vec3b>(row, col)[0] = color_list[max_idx][2];	//B
            output.at<cv::Vec3b>(row, col)[1] = color_list[max_idx][1];	//G
            output.at<cv::Vec3b>(row, col)[2] = color_list[max_idx][0];	//R
        }
    }
    return output;
}
