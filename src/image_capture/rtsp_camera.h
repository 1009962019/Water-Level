#ifndef RSTP_CAMARA_H
#define RSTP_CAMARA_H
#include "opencv2/opencv.hpp"
#include <QString>

class RtspCamera
{
public:
    RtspCamera(QString pos);
    ~RtspCamera();

    bool isOpened_ = false;
    QString pos_ = "";   //相机所在的位置及用途，如inside_waterlevel
    cv::VideoCapture *cap_ = nullptr;
    int stream_fps_;     //视频流的帧率

public:
    bool get_single_frame(void);
    bool open(void);
    void close(void);
};

#endif
