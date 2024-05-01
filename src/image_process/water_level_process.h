#include <iostream>
#include <QString>
#include <opencv2/opencv.hpp>

using namespace std;


void crop_ROI(cv::Mat& src, QString position);  //裁剪水位图像ROI区域

int get_waterline_position(cv::Mat& rough_output);      //由粗检测掩膜图获取裁剪区域的左上角y坐标

vector<double> fitting_waterline(cv::Mat& fine_output, int left_up_y_in_src, QString pos);      //水位线拟合



void fitLineWithConstant_k(vector<cv::Point2d> ptSet, double &a, double &b, double &c, QString position);   //固定斜率拟合

void fitLineRANSAC(vector<cv::Point2d> ptSet, double &a, double &b, double &c, vector<bool> &inlierFlag);   //RANSAC直线拟合

void calcLinePara(vector<cv::Point2d> pts, double &a, double &b, double &c, double &res);   //根据点集拟合直线ax+by+c=0(最小二乘拟合)

bool getSample(vector<int> set, vector<int> &sset);     //在所有样本点中随机选2个点

bool verifyComposition(const vector<cv::Point2d> pts);  //两个随机样本点的距离不能太近

double uniformRandom();     //生成[0,1]之间符合均匀分布的数
