#ifndef RESULTS_DISPLAY_H
#define RESULTS_DISPLAY_H

#include "config/ParamSet.h"
#include "image_process/image_process_thread.h"
#include "opencv2/opencv.hpp"

#include <QObject>
#include <QLabel>
#include <QString>

class ResultsDisplay : public QObject
{
    Q_OBJECT
public:
    ResultsDisplay(QString pos, QLabel *label_preview);

    ~ResultsDisplay(void)
    {

    }

    //由水位线直线方程和标定结果计算水位
    double calculate_waterline(vector<double>& line_equation, WaterlevelCameraParam* param);

    //根据水位线检测结果计算水位，显示结果
    void calculate_waterline_and_show_result(WaterlineResult& waterline_detect_result);

    void show_result(cv::Mat& result);

private:
    QString pos_; //inside or outside
    QLabel* label_preview_;
};

#endif
