#ifndef IMAGEPROCESSTHREAD_H
#define IMAGEPROCESSTHREAD_H

#include <QObject>
#include <QThread>
#include "blockingconcurrentqueue.h"

#include "SegmentationInference.h"
#include "water_level_process.h"

using namespace moodycamel;

/**
 * @brief WaterlineResult 水位线检测结果类
 */
class WaterlineResult
{
public:
    QString pos;    //闸室位置描述，如inside_waterlevel
    cv::Mat img;    //水位原始图像 2160*2160
    vector<double> line_equation = {0.0, 0.0, 0.0}; //水位线直线方程参数
};


class ImageProcessThread : public QThread
{
    Q_OBJECT
public:
    ImageProcessThread();
    ~ImageProcessThread();

    SegmentationInference *waterlevel_inference_rough = nullptr, *waterlevel_inference_fine = nullptr;    //水位图像分割模型推理类指针
    void waterlevel_process(QPair<cv::Mat, QString>& img_pos_pair);     //水位检测图像处理

protected:
    void run() override;

signals:
    void waterline_process_completed();     //水位线检测完成信号

};

#endif // IMAGEPROCESSTHREAD_H
