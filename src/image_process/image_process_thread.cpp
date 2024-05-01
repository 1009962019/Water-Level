/******************************************************************************************/
/*                                ImageProcessThread                                      */
/*                                    图像处理子线程                                         */
/*                         包含图像处理、深度学习模型推理、光流跟踪                               */
/*从全局队列input_queue中获取输入图像，将结果放入全局队列waterline_detect_result,ship_detect_result*/
/*******************************************************************************************/

#include "image_process_thread.h"
#include "plog/Log.h"
BlockingConcurrentQueue<QPair<cv::Mat, QString>> input_queue;     // 采集的原图像队列
BlockingConcurrentQueue<WaterlineResult> waterline_detect_result; // 水位线检测结果队列

ImageProcessThread::ImageProcessThread()
{
    /*****************************************实例化深度学习模型推理对象************************************/
    waterlevel_inference_rough = new SegmentationInference("/home/nvidia/Desktop/WaterLine/Model/rough_waternet.engine", 512, 512, 3); // 水位线粗检测
    waterlevel_inference_fine = new SegmentationInference("/home/nvidia/Desktop/WaterLine/Model/fine_waternet.engine", 2160, 256, 3);  // 水位线精检测
}

ImageProcessThread::~ImageProcessThread()
{
    delete waterlevel_inference_rough;
    delete waterlevel_inference_fine;
}

/**
 * @brief ImageProcessThread::run 子线程run函数
 */
void ImageProcessThread::run()
{
    QPair<cv::Mat, QString> img_pos_pair;
    while (1)
    {
        // 从输入原图像队列中取出对象，根据第二个变量的字符串值，使用不同的图像处理函数
        if (input_queue.try_dequeue(img_pos_pair))
        {
            if (img_pos_pair.second.endsWith("waterlevel"))
                waterlevel_process(img_pos_pair);
        }
    }
}

/**
 * @brief 水位检测图像处理,计算结果:水位直线的直线方程！计算结果被存入全局结果队列
 * @param img_pos_pair “图像-图像描述”对
 */
void ImageProcessThread::waterlevel_process(QPair<cv::Mat, QString> &img_pos_pair)
{
    // 记录水位开始时间
    double time = static_cast<double>(cv::getTickCount());

    // 裁剪ROI区域，压缩至低分辨率图像
    cv::Mat src = img_pos_pair.first;
    crop_ROI(src, img_pos_pair.second);
    cv::Mat src_resize;
    cv::resize(src, src_resize, cv::Size(512, 512), cv::INTER_AREA);

    // 新建水位线检测结果类实例
    WaterlineResult waterline_result;
    waterline_result.pos = img_pos_pair.second;
    waterline_result.img = src;

    // 取图像对角线上的512个像素点，三通道值均相同时为红外夜间模式
    int num_night = 0;
    for (int i = 0; i < 512; i++)
    {
        if (src_resize.at<cv::Vec3b>(i, i)[0] == src_resize.at<cv::Vec3b>(i, i)[1] && src_resize.at<cv::Vec3b>(i, i)[0] == src_resize.at<cv::Vec3b>(i, i)[2])
        {
            num_night++;
        }
    }

    // 不是夜间模式时正常检测
    if (num_night != 512)
    {
        // 粗检测
        cv::Mat rough_output = waterlevel_inference_rough->do_inference(src_resize); // 粗检测模型推理
        int left_up_y = get_waterline_position(rough_output);                        // 由粗检测结果获取裁剪区域的左上角y坐标
        int left_up_y_in_src = (int)(left_up_y / 512.0 * src.rows);                  // 将y坐标由低分辨率图像映射回原图像

        // 能在图像中找到粗检测水位线邻域时，进一步进行精检测
        if (left_up_y != 0)
        {
            // 精检测
            cv::Mat edge = src(cv::Rect(cv::Point2d(0, left_up_y_in_src), cv::Point2d(src.cols, min(left_up_y_in_src + 256, src.rows)))); // 根据原图像中的左上角y坐标裁剪邻域图像
            cv::Mat fine_output = waterlevel_inference_fine->do_inference(edge);                                                          // 精检测模型推理

            // 水位线拟合
            vector<double> line_equation = fitting_waterline(fine_output, left_up_y_in_src, img_pos_pair.second);
            waterline_result.line_equation = line_equation;
        }
    }
    waterline_detect_result.enqueue(waterline_result); // 放入水位线检测结果队列
    emit waterline_process_completed();                // 发送水位线检测完成信号，通知主线程(在mainwindow.cpp中被处理)

    double use_time = (static_cast<double>(cv::getTickCount()) - time) / cv::getTickFrequency(); // unit :s
    PLOG_VERBOSE << " Process USED TIME = " << use_time << "(s)";
}
