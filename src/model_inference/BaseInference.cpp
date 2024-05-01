/************************************************************/
/*                     BaseInference                        */
/*                       模型推理基类                         */
/*  包含加载TensorRT模型、图像等比例缩放、前处理、模型前向推理等步骤  */
/***********************************************************/

#include "BaseInference.h"

BaseInference::BaseInference(string model_path, int width, int height, int classes, int device = 0)
{
    cudaSetDevice(device); // 设置GPU设备

    input_w = width;       // 模型输入图像宽
    input_h = height;      // 模型输入图像高
    num_classes = classes; // 类别数

    load_trt_model(model_path); // 加载TensorRT模型
}

BaseInference::~BaseInference()
{
    context->destroy();
    engine->destroy();
    runtime->destroy();
    delete[] prob;
}

/**
 * @brief BaseInference::load_trt_model 加载TensorRT的engine文件
 * @param model_path 模型路径
 */
void BaseInference::load_trt_model(string model_path)
{
    // create a model using the API directly and serialize it to a stream
    char *trtModelStream{nullptr};
    size_t size{0};
    ifstream file(model_path, std::ios::binary); // 以二进制方式加载模型文件
    if (file.good())
    {
        file.seekg(0, file.end);
        size = file.tellg();
        file.seekg(0, file.beg);
        trtModelStream = new char[size];
        assert(trtModelStream);
        file.read(trtModelStream, size);
        file.close();
    }
    runtime = createInferRuntime(gLogger); // 创建推理引擎
    assert(runtime != nullptr);
    engine = runtime->deserializeCudaEngine(trtModelStream, size); // 反序列化engine文件
    assert(engine != nullptr);
    context = engine->createExecutionContext(); // 创建上下文环境
    assert(context != nullptr);
    delete[] trtModelStream;
    auto out_dims = engine->getBindingDimensions(1); // 获取模型输出维度
    for (int j = 0; j < out_dims.nbDims; j++)
    {
        output_size *= out_dims.d[j];
    }
    prob = new float[output_size]; // 根据模型输出结果的尺寸，开辟数组空间
}

/**
 * @brief BaseInference::scale_resize 等比例缩放原图像至模型输入尺寸，周围灰度填充
 * @param img
 * @return
 */
cv::Mat BaseInference::scale_resize(cv::Mat &img)
{
    float r = min(input_w / (img.cols * 1.0), input_h / (img.rows * 1.0));
    int unpad_w = r * img.cols + 0.5;
    int unpad_h = r * img.rows + 0.5;
    cv::Mat img_resize;
    cv::resize(img, img_resize, cv::Size(unpad_w, unpad_h), cv::INTER_AREA);
    cv::Mat out(input_h, input_w, CV_8UC3, cv::Scalar(114, 114, 114));
    img_resize.copyTo(out(cv::Rect(0, 0, unpad_w, unpad_h)));
    return out;
}

/**
 * @brief BaseInference::preprocess 图像前处理过程，归一化后转为float*数组，虚函数
 * @param img
 * @return blob 模型输入数组
 */
float *BaseInference::preprocess(cv::Mat &img)
{
    // 归一化 RGB 浮点数
    cv::cvtColor(img, img, cv::COLOR_BGR2RGB);
    img.convertTo(img, CV_32F, 1.0 / 255);
    img = img - cv::Scalar(0.485, 0.456, 0.406);
    img = img / cv::Scalar(0.229, 0.224, 0.225);

    // 转成float*数组，注意取值时用的是Vec3f
    float *blob = new float[img.total() * 3];
    int channels = 3;
    int img_h = img.rows;
    int img_w = img.cols;
    for (int c = 0; c < channels; c++)
    {
        for (int h = 0; h < img_h; h++)
        {
            for (int w = 0; w < img_w; w++)
            {
                blob[c * img_w * img_h + h * img_w + w] = (float)img.at<cv::Vec3f>(h, w)[c]; // 将图像三个维度展平成一个维度，存入blob数组，作为模型输入
            }
        }
    }
    return blob;
}

/**
 * @brief BaseInference::model_inference 完成前处理和前向推理
 * @param src_img
 */
void BaseInference::model_inference(cv::Mat &src_img)
{
    cv::Mat src_resize = scale_resize(src_img); // 等比例缩放图像
    cv::Size input_shape = src_resize.size();
    float *blob = preprocess(src_resize); // 图像前处理

    // Pointers to input and output device buffers to pass to engine.
    // Engine requires exactly IEngine::getNbBindings() number of buffers.
    assert(engine->getNbBindings() == 2);
    void *buffers[2];

    // In order to bind the buffers, we need to know the names of the input and output tensors.
    // Note that indices are guaranteed to be less than IEngine::getNbBindings()
    const int inputIndex = engine->getBindingIndex(input_blob_name);
    assert(engine->getBindingDataType(inputIndex) == nvinfer1::DataType::kFLOAT);
    const int outputIndex = engine->getBindingIndex(output_blob_name);
    //assert(engine.getBindingDataType(outputIndex) == nvinfer1::DataType::kFLOAT);

    // Create GPU buffers on device
    CHECK(cudaMalloc(&buffers[inputIndex], 3 * input_shape.height * input_shape.width * sizeof(float))); // 分配显存
    CHECK(cudaMalloc(&buffers[outputIndex], output_size * sizeof(float)));

    // Create stream
    cudaStream_t stream;
    CHECK(cudaStreamCreate(&stream));

    // DMA input batch data to device, infer on the batch asynchronously, and DMA output back to host
    CHECK(cudaMemcpyAsync(buffers[inputIndex], blob, 3 * input_shape.height * input_shape.width * sizeof(float), cudaMemcpyHostToDevice, stream)); // 将输入blob异步从内存拷贝到显存
    context->enqueueV2(buffers, stream, nullptr);
    CHECK(cudaMemcpyAsync(prob, buffers[outputIndex], output_size * sizeof(float), cudaMemcpyDeviceToHost, stream)); // 将输出prob异步从显存拷贝到内存
    cudaStreamSynchronize(stream);

    // Release stream and buffers
    cudaStreamDestroy(stream);
    CHECK(cudaFree(buffers[inputIndex])); // 释放显存
    CHECK(cudaFree(buffers[outputIndex]));

    delete[] blob;
}
