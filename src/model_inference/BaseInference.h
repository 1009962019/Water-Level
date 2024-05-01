#ifndef BASEINFERENCE_H
#define BASEINFERENCE_H

#include <fstream>
#include <iostream>
#include <sstream>
#include <numeric>
#include <chrono>
#include <vector>
#include <opencv2/opencv.hpp>
#include "NvInfer.h"
#include "cuda_runtime_api.h"
#include "logging.h"

using namespace std;
using namespace nvinfer1;

#define CHECK(status)                                          \
    do                                                         \
    {                                                          \
        auto ret = (status);                                   \
        if (ret != 0)                                          \
        {                                                      \
            std::cerr << "Cuda failure: " << ret << std::endl; \
            abort();                                           \
        }                                                      \
    } while (0)

class BaseInference 
{
public:
    BaseInference(string model_path, int width, int height, int classes, int device);

    virtual ~BaseInference();

    void model_inference(cv::Mat &src_img); // 模型推理函数(包括前处理和前向推理)

    cv::Mat scale_resize(cv::Mat &img); // 等比例缩放原图像至模型输入尺寸

    virtual float *preprocess(cv::Mat &img); // 图像前处理函数

    float *prob = nullptr; // 存放模型推理结果的数组指针

    int input_w;     // 模型输入图像宽
    int input_h;     // 模型输入图像高
    int num_classes; // 类别数

private:
    Logger gLogger;
    IRuntime *runtime = nullptr;
    ICudaEngine *engine = nullptr;
    IExecutionContext *context = nullptr;
    int output_size = 1;                       // 模型输出结果的尺寸
    const char *input_blob_name = "input_0";   // 模型输入节点名称
    const char *output_blob_name = "output_0"; // 模型输出节点名称

    void load_trt_model(string model_path); // 加载TensorRT模型
};

#endif // BASEINFERENCE_H
