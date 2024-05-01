/***************************************************/
/*               ImageCaptureThread                */
/*                   图像采集子线程                   */
/*  包含BMP和JPEG两种采图功能，在BaseImageModule中被使用 */
/***************************************************/

#include "image_capture_thread.h"
#include "plog/Log.h"

BlockingConcurrentQueue<QPair<cv::Mat, QString>> input_queue;

ImageCaptureThread::ImageCaptureThread(QString pos, float period) : QThread(nullptr)
{
    this->pos_ = pos;
    this->camera_ = new RtspCamera(pos);
    this->period_ = period;
}

bool ImageCaptureThread::open_camera()
{
    if(!this->camera_->open())
    {
        return false;
    }
    this->enque_interval_ = int(this->camera_->stream_fps_ * this->period_);
    return true;
}

void ImageCaptureThread::close_camera()
{
    this->camera_->close();
}

/**
 * @brief ImageCaptureThread::run 子线程run函数
 */
void ImageCaptureThread::run()
{
    if(!this->open_camera())
    {
        PLOG_ERROR << "开始采图失败！";
        return;
    }
    PLOG_INFO << "开始采图！";

    int idx = 0;
    int enque_idx = 0;
    double time = static_cast<double>(cv::getTickCount()); // 记录采图开始时间
    double use_time = 0;

    while(1)
    {
        cv::Mat SRC_image;
        bool ret = this->camera_->cap_->read(SRC_image);
        if (!ret)
        {
            PLOG_ERROR << "Read VideoCapture Failed!";
            continue;
        }

        ++idx;
        if (idx % this->enque_interval_ == 0 && idx != 0)
        {
            idx = 0;
            QPair<cv::Mat, QString> img_pos_pair;
            img_pos_pair.first = SRC_image;
            img_pos_pair.second = this->pos_;
            input_queue.enqueue(img_pos_pair); // 采集到的图像放入全局队列

            use_time = (static_cast<double>(cv::getTickCount()) - time) / cv::getTickFrequency(); // unit :s
            time = static_cast<double>(cv::getTickCount());                                       // 记录新一轮采图开始时间
            PLOG_VERBOSE << " Capture IDX: " << enque_idx++ << "  USED TIME = " << use_time << "(s)";
        }

//        clock_t use_time = clock() - t;   //记录采图结束时间
//        if(use_time <= 1000) QThread::msleep(1000 - use_time);  //若采图时间小于1s，线程睡眠，保证采图间隔为1s
    }

//    PLOG_INFO << "结束采图！";
}
