/******************************************************/
/*                     ParamSet                       */
/*                    各类参数结构体                     */
/*  包含数据库、相机、网络IO模块、网络IP功放等参数定义的结构体  */
/******************************************************/

#ifndef PARAMSET_H
#define PARAMSET_H

#include <QString>
#include <iostream>
#include <vector>

using namespace std;

struct BasicParam
{
    QString host;
    int port;
};

struct CameraParam : public BasicParam
{
    QString username;
    QString password;
};

/**
 * @brief WaterlevelCameraParam 水位相机参数
 */
struct WaterlevelCameraParam : public CameraParam
{
    int x1, y1, w, h;   //裁剪ROI区域左上角坐标和宽高
    double constant_a, constant_b;  //直线拟合的固定斜率
    int calibration_location_x; //标定点横坐标
    vector<pair<double, double>> pixel_world_mapping_relation;    //像素坐标与世界坐标映射关系
};


#endif // PARAMSET_H
