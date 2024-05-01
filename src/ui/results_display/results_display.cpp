#include "results_display.h"


ResultsDisplay::ResultsDisplay(QString pos, QLabel *label_preview)
{
    this->pos_ = pos;
    this->label_preview_ = label_preview;
}

/**
 * @brief ResultsDisplay::calculate_waterline 由水位线直线方程和标定结果计算水位
 * @param line_equation 水位线直线方程参数，ax+by+c=0，依次保存a,b,c
 * @param param 水位相机参数，内含标定结果
 * @return 计算的水位
 */
double ResultsDisplay::calculate_waterline(vector<double>& line_equation, WaterlevelCameraParam* param)
{
    double A = line_equation[0], B = line_equation[1], C = line_equation[2];    //取水位线直线方程参数
    if (A == 0 && B == 0 && C == 0) return -1;  //当a,b,c均为0时，未检测到水位线，返回-1
    double waterline_y = -(A * param->calibration_location_x + C) / B;  //代入水位标定点的x坐标，计算y坐标

    if(waterline_y >= param->pixel_world_mapping_relation[0].second)
    {
        //采用分段线性插值的方法计算水位
        for(int i = 0; i < param->pixel_world_mapping_relation.size(); i++)
        {
            pair<double, double> _pair = param->pixel_world_mapping_relation[i];
            if(waterline_y >= _pair.second)
            {
                waterline_y -= _pair.second;
            }
            else
            {
                double unit = param->pixel_world_mapping_relation[i - 1].first - param->pixel_world_mapping_relation[i].first;  //相邻两个标定点间的实际距离
                double calculated_water_level = _pair.first + (_pair.second - waterline_y) / _pair.second * unit;   //线性插值
                return calculated_water_level;
            }
        }
    }
    return -1;  //水位线y坐标高于标定结果中最高的高程，无法计算，返回-1
}

void ResultsDisplay::show_result(cv::Mat &result)
{
    cv::cvtColor(result, result, cv::COLOR_BGR2RGB);
    QImage image = QImage(result.data,
                          result.cols,
                          result.rows,
                          result.cols * result.channels(),
                          QImage::Format_RGB888);
    this->label_preview_->clear();

    image.scaled(this->label_preview_->size(), Qt::KeepAspectRatio);
    this->label_preview_->setScaledContents(true);
    this->label_preview_->setPixmap(QPixmap::fromImage(image));
    this->label_preview_->show();
}

/**
 * @brief ResultsDisplay::calculate_waterline_and_show_result 根据水位线检测结果计算水位，显示结果
 * @param waterline_detect_result 水位线检测结果
 */
void ResultsDisplay::calculate_waterline_and_show_result(WaterlineResult& waterline_detect_result)
{
    WaterlevelCameraParam* waterlevel_camera_param = Config::get_instance()->get_waterlevel_param(pos_); //获取水位相机参数

    double calculated_water_level = calculate_waterline(waterline_detect_result.line_equation, waterlevel_camera_param);    //由水位线直线方程和标定结果计算水位
    cv::Mat src = waterline_detect_result.img;

    if (calculated_water_level >= 0)
    {
        //绘制直线（在原图上画更清楚些）
        double A = waterline_detect_result.line_equation[0], B = waterline_detect_result.line_equation[1], C = waterline_detect_result.line_equation[2];
        cv::Point2d ptStart, ptEnd; //水位线的起始点和终止点
        ptStart.x = 0;
        ptStart.y = -(A * ptStart.x + C) / B;
        ptEnd.x = src.cols - 1;
        ptEnd.y = -(A * ptEnd.x + C) / B;
        int line_width = 3;    //判断当前显示结果的框是否被放大，使用不同粗细的线绘制
        cv::line(src, ptStart, ptEnd, cv::Scalar(0, 0, 255), line_width, 8);    //画线
    }
    cv::resize(src, src, cv::Size(512, 512), cv::INTER_AREA);
    show_result(src);   //显示水位检测结果
}


