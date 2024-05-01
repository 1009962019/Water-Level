#ifndef IMAGECAPTURETHREAD_H
#define IMAGECAPTURETHREAD_H

#include <QObject>
#include <QThread>
#include <QDebug>
#include <QDateTime>

#include <opencv2/opencv.hpp>
#include "blockingconcurrentqueue.h"
#include "rtsp_camera.h"

using namespace std;
using namespace moodycamel;

extern BlockingConcurrentQueue<QPair<cv::Mat, QString>> input_queue;    //采集原图像队列

class ImageCaptureThread : public QThread
{
    Q_OBJECT
public:
    ImageCaptureThread(QString pos, float period);

    float period_;       // 采图周期(s)
    int enque_interval_; // 读多少帧往全局队列中入队一张图
    QString pos_ = "";   //相机所在的位置及用途，如inside_waterlevel
    RtspCamera *camera_;

    bool open_camera(void);
    void close_camera(void);

protected:
    void run() override;

signals:
    void capture_error(int);    //相机采图失败的信号，参数为错误码

public slots:
};

#endif // IMAGECAPTURETHREAD_H
