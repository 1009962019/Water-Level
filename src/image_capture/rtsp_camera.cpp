#include "rtsp_camera.h"
#include "plog/Log.h"
#include <ctime>
#include <mutex>
#include <condition_variable>
#include <thread>

RtspCamera::RtspCamera(QString pos)
{
    this->pos_ = pos;
    this->isOpened_ = false;
    this->stream_fps_ = -1;
}

RtspCamera::~RtspCamera()
{
    if (this->isOpened_)
    {
        this->close();
    }
}

/**
 * @brief 打开RTSP相机,并更新相关的成员变量
 * 具体逻辑:创建个互斥锁锁住主线程5s,并在子线程中打开cap_,
 *          如果5s内未成功打开,则在log中报错,但是该函数不会立即退出,
 *          只有在cap_->open()返回后才会结束该函数
 *
 * @return true:相机已打开打开/成功
 *         false:相机打开失败
 */
bool RtspCamera::open(void)
{
    /* 判断相机是否已打开 */
    if (this->isOpened_ == true)
    {
        PLOG_ERROR << "Camera already Open !";
        return true;
    }

    std::mutex g_mutex_wait;             // 互斥锁
    std::condition_variable g_cond_wait; // 条件变量

    /* 初始化并打开 Videocapture */
    this->cap_ = new cv::VideoCapture;
    #ifdef WIN32
    std::string rstp_config = "rtsp://admin:waterline123456@192.168.1.64:554";
    #else
    std::string rstp_config =
        "rtspsrc location=rtsp://admin:waterline123456@192.168.1.64:554,latency=0 ! \
                 rtph264depay ! h264parse ! omxh264dec ! nvvidconv ! \
                 video/x-raw, width=3840, height=2160, format=BGRx ! \
                 videoconvert ! appsink sync=false";

    #endif

    /* 创建个子线程,在该线程中尝试打开相机 */
    std::thread thread_openCamera([&]
    {
        this->cap_->open(rstp_config); // 这里最好开个定时器,超时就属于连接错误
        g_cond_wait.notify_one();
    });
    std::unique_lock<std::mutex> lockWait(g_mutex_wait);//对互斥锁上锁

    PLOG_DEBUG << "Start to open Camera...";

    std::cv_status cvsts = g_cond_wait.wait_for(lockWait, std::chrono::seconds(5));

    /* 超过5s未打开相机,则认为打开失败 */
    if (cvsts == std::cv_status::timeout)
    {
        PLOG_ERROR << "Open Camera Timeout! "
                   << "Please Change configs!";
    }
    thread_openCamera.join(); //阻塞原线程,等待cap_->open()返回

    if (!this->cap_->isOpened())
    {
        PLOG_ERROR << "Open Camera Failed !";
        return false;
    }

    /* ------ 更新状态变量 ------ */
    PLOG_DEBUG << "Open Camera SUCCESS !";
    this->stream_fps_ = int(this->cap_->get(cv::CAP_PROP_FPS));
    this->isOpened_ = true;
    return true;
}

/**
 * @brief 关闭RTSP相机,并更新相关的成员变量
 */
void RtspCamera::close(void)
{
    if (this->isOpened_ == false)
    {
        PLOG_ERROR << "Camera not Open !";
        return;
    }

    /* 停止并删除 Videocapture */
    this->cap_->release();
    delete this->cap_;

    /* 更新状态变量 */
    PLOG_DEBUG << "Release Camera SUCCESS !";
    this->isOpened_ = false;
    this->stream_fps_ = -1;
    return;
}

/**
 * @brief 拍一张图片并保存到本地
 * @return
 */
bool RtspCamera::get_single_frame(void)
{
    if (!this->isOpened_)
    {
        //PLOG_ERROR << "Camera not Open !";
        return false;
    }

//    if (this->image_cap_ture_thread->latest_frame.empty())
//    {
//        //PLOG_ERROR << "No image has been cap_tured!";
//        return false;
//    }

//    /* 文件名相关 */
//    time_t now = time(0);
//    tm *ltm = localtime(&now);
//    char buffer[80]; // 创建一个缓冲区来保存格式化的日期和时间字符串
//    std::strftime(buffer, sizeof(buffer), "%Y_%m_%d_%H_%M_%S", ltm);
//    string temp(buffer);
//    #ifdef WIN32
//    string pic_name = "F:/Works/2_WaterLine/WaterLine/cap_ture/" + temp + ".jpg"; // 单次采图保存位置
//    #else
//    string pic_name = "/home/nvidia/Desktop/WaterLine/cap_ture/" + temp + ".jpg";
//    #endif
//    return cv::imwrite(pic_name, this->image_cap_ture_thread->latest_frame);
}
