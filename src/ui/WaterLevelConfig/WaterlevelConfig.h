#ifndef WATERLEVELCONFIG_H
#define WATERLEVELCONFIG_H

#include <QWidget>
#include <QStandardItem>
#include <QStandardItemModel>
#include "Config/Config.h"
#include <opencv2/opencv.hpp>

namespace Ui
{
    class WaterlevelConfig;
}

class WaterlevelConfig : public QWidget
{
    Q_OBJECT

public:
    WaterlevelConfig(QString pos, QWidget *parent = 0);

    ~WaterlevelConfig();

    void set_background_image(cv::Mat& img);    //设置背景图片

private slots:
    void handle_roi_textchanged(QString arg);   //当修改ROI区域和标定点x坐标时，实时在图上显示出来

    void handle_mapping_relation_changed(int row, int col);     //当修改表格中绝对像素高度时，实时在图上画点

    void handle_add_point(int y);   //添加标定点槽函数，在图上点击后，在表格内添加绝对像素高度

    void on_pushButton_delete_clicked();    //删除选中行的标定数据

    void on_pushButton_save_param_clicked();    //保存水位相机参数

private:
    Ui::WaterlevelConfig *ui;

    WaterlevelCameraParam* waterlevel_camera_param = nullptr;   //水位相机参数指针

    void show_mapping_relation(vector<vector<double>>& mapping_relation);   //初始化表格时显示映射关系


};

#endif // WATERLEVELCONFIG_H
