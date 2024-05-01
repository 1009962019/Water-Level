/***************************************************/
/*              WaterlevelProcess                  */
/*                水位图像处理函数                    */
/*  包含裁剪ROI区域、提取粗检测水位线邻域、水位线拟合等功能  */
/***************************************************/

#include "water_level_process.h"

/**
 * @brief crop_ROI 裁剪水位图像ROI区域
 * @param src 水位图像
 * @param position  闸室位置，用于获取不同的裁剪区域
 */
void crop_ROI(cv::Mat& src, QString position)
{
    WaterlevelCameraParam* param = Config::get_instance()->get_waterlevel_param(position);  //获取水位相机参数

    int x1 = param->x1;
    int y1 = param->y1;
    int x2 = x1 + param->w;
    int y2 = y1 + param->h;
    src = src(cv::Rect(cv::Point2d(x1, y1), cv::Point2d(x2, y2)));  //裁剪图像
}

/**
 * @brief get_waterline_position 由粗检测掩膜图获取裁剪区域的左上角y坐标
 * @param rough_output 粗检测掩膜图
 * @return 压缩图像中的左上角y坐标
 */
int get_waterline_position(cv::Mat& rough_output)
{
    /************由分割得到的掩膜图包含三类，BGR像素值分别为闸室墙(0,0,0)，水体(0,0,128)，船舶(0,128,0)，对图像的R通道除以128，方便提取闸室墙与水体的边界***************/
    cv::Mat convert;
    rough_output.convertTo(convert, CV_32F, 1.0);
    convert = convert / cv::Scalar(1.0, 1.0, 128.0);    //对图像的R通道除以128

    /***********************************求边界图**********************************/
    cv::Mat gray, sobel_y;
    cv::cvtColor(convert, gray, cv::COLOR_BGR2GRAY);
    cv::Sobel(gray, sobel_y, CV_64F, 0, 1);
    //cv_show(sobel_y);

    /***********************统计每一行中闸室墙与水体的边界点的个数**********************/
    vector<int> sum_each_rows(sobel_y.rows, 0);
    for (int row = 0; row < sobel_y.rows; row++)
    {
        for (int col = 0; col < sobel_y.cols; col++)
        {
            if (sobel_y.at<double>(row, col) > 0 && sobel_y.at<double>(row, col) <= 4)  //边界图上响应值在(0,4]范围内的为闸室墙与水体的边界点
            {
                sum_each_rows[row] += 1;
            }
        }
    }

    /**************以高度为60像素的窗口做滚动求和，从上至下计算高度60的窗口内闸室墙与水体边界点的总个数*****************/
    for (int i = 0; i < sum_each_rows.size() - 60; i++)
    {
        sum_each_rows[i] = accumulate(&sum_each_rows[i], &sum_each_rows[i + 60], 0, [](int a, int b)
        {
            return a + b;
        });
    }
    sum_each_rows.assign(sum_each_rows.begin(), sum_each_rows.end() - 60);

    /**************经过滚动求和后，不是水位线的地方窗口内边界点总个数恒为0，而水位线邻域的窗口内边界点总个数恒为某个正值*************/

    /**************************统计每个元素的数量，以哈希表形式存储************************/
    map<int, int> number_freq_dict;
    for (int n : sum_each_rows)
    {
        if(number_freq_dict.find(n) != number_freq_dict.end())
            number_freq_dict[n]++;
        else
            number_freq_dict[n] = 1;
    }

    /******************************找出次数最多的元素**********************************/
    int max_freq = 0;
    int max_freq_number = -1;
    int two_multiply = 0;
    for (auto  m : number_freq_dict)
    {
        if (m.first > 100 && m.first * m.second > two_multiply)
        {
            max_freq_number = m.first;
            max_freq = m.second;
            two_multiply = m.first * m.second;
        }
    }
    //cout << "max freq " << max_freq << endl;
    //cout << "max freq number " << max_freq_number << endl;

    /****************************求裁剪区域左上角的坐标**********************************/
    int left_up_y = 0;
    for (int i = 0; i < sum_each_rows.size(); i++)
    {
        if (sum_each_rows[i] == max_freq_number)    //对所有边界点数量最多的窗口的高度求平均
            left_up_y += i;
    }
    if(max_freq == 0) max_freq = 1; //防止除0
    left_up_y /= max_freq;

    return left_up_y;
}

/**
 * @brief fitting_waterline 水位线拟合
 * @param fine_output 精检测掩膜图
 * @param left_up_y_in_src 裁剪邻域图像左上角在原图中的y坐标
 * @param pos 闸室位置，用于获取不同的水位线固定斜率
 * @return 水位线直线方程参数ax+by+c=0
 */
vector<double> fitting_waterline(cv::Mat& fine_output, int left_up_y_in_src, QString pos)
{
    /************由分割得到的掩膜图包含三类，BGR像素值分别为闸室墙(0,0,0)，水体(0,0,128)，船舶(0,128,0)，对图像的R通道除以128，方便提取闸室墙与水体的边界***************/
    cv::Mat convert;
    fine_output.convertTo(convert, CV_32F, 1.0);
    convert = convert / cv::Scalar(1.0, 1.0, 128.0);    //对图像的R通道除以128

    /***********************************求边界图**********************************/
    cv::Mat gray, sobel_y;
    cv::cvtColor(convert, gray, cv::COLOR_BGR2GRAY);
    cv::Sobel(gray, sobel_y, CV_64F, 0, 1);

    /***************************存储原图像中的水位线边界点***************************/
    vector<cv::Point2d> points_in_src;
    for (int row = 0; row < sobel_y.rows; row++)
    {
        for (int col = 0; col < sobel_y.cols; col++)
        {
            if (sobel_y.at<double>(row, col) > 0 && sobel_y.at<double>(row, col) <= 4)  //边界图上响应值在(0,4]范围内的为闸室墙与水体的边界点
            {
                if (pos == "outside_waterlevel" && col < 450) continue;     //由于上游引航道的水位图像最左侧有根立柱凸出来，舍弃这部分边界点
                points_in_src.push_back(cv::Point2d(col, row + left_up_y_in_src));    //存储水位线边界点
            }
        }
    }

    /****************边界点集为空或边界点数量过少，表明未检测到水位线*******************/
    if(points_in_src.empty() || points_in_src.size() <= 200)
    {
        vector<double> line_equation = {0.0, 0.0, 0.0};
        return line_equation;
    }

    double A = 0, B = 0, C = 0;
    vector<bool> inliers;
    fitLineRANSAC(points_in_src, A, B, C, inliers);     //采用RANSAC方法进行直线拟合
    if(fabs(A) >= 0.03 || fabs(B) <= 0.9995)    //若水位线斜率偏离太大，使用固定斜率再次拟合
        fitLineWithConstant_k(points_in_src, A, B, C, pos);
    vector<double> line_equation = {A, B, C};
    return line_equation;
}

/************************************************************************************************************/
/**********************************以下均为直线拟合相关代码，可不做详细了解******************************************/
/************************************************************************************************************/

/**
 * @brief fitLineWithConstant_k 固定斜率拟合
 * @param ptSet 边界点集
 * @param a 直线方程参数
 * @param b
 * @param c
 * @param position 闸室位置，用于获取不同的水位线固定斜率
 */
void fitLineWithConstant_k(vector<cv::Point2d> ptSet, double &a, double &b, double &c, QString position)
{
    int N = ptSet.size();
    double residual_error = 3; //内点阈值

    WaterlevelCameraParam* param = Config::get_instance()->get_waterlevel_param(position);  //获取水位相机参数
    a = param->constant_a;  //取水位线固定斜率
    b = param->constant_b;

    srand((unsigned)time(NULL));
    random_shuffle(ptSet.begin(), ptSet.end());
    int inlier_count = 0;
    for(int n = 0; n < N; n++)
    {
        cv::Point2d pt = ptSet[n];
        double _c = -a * pt.x - b * pt.y;

        int _inlier_count = 0;
        //内点检验
        for (unsigned int i = 0; i < ptSet.size(); i++)
        {
            cv::Point2d pt = ptSet[i];
            double resid_ = fabs(pt.x * a + pt.y * b + _c);
            if (resid_ < residual_error)
                ++_inlier_count;
        }
        if (_inlier_count >= inlier_count)
        {
            inlier_count = _inlier_count;
            c = _c;
        }
    }
}

/**
 * @brief fitLineRANSAC RANSAC直线拟合
 * @param ptSet 边界点集
 * @param a 直线方程参数
 * @param b
 * @param c
 * @param inlierFlag 是否为内点
 */
void fitLineRANSAC(vector<cv::Point2d> ptSet, double &a, double &b, double &c, vector<bool> &inlierFlag)
{
    double residual_error = 3; //内点阈值

    bool stop_loop = false;
    int maximum = 0;  //最大内点数

    //最终内点标识及其残差
    inlierFlag = vector<bool>(ptSet.size(), false);
    vector<double> resids_(ptSet.size(), 3);
    int sample_count = 0;
    int N = 500;

    double res = 0;

    // RANSAC
    srand((unsigned int)time(NULL)); //设置随机数种子
    vector<int> ptsID;
    for (unsigned int i = 0; i < ptSet.size(); i++)
        ptsID.push_back(i);
    while (N > sample_count && !stop_loop)
    {
        vector<bool> inlierstemp;
        vector<double> residualstemp;
        vector<int> ptss;
        int inlier_count = 0;
        if (!getSample(ptsID, ptss))
        {
            stop_loop = true;
            continue;
        }

        vector<cv::Point2d> pt_sam;
        pt_sam.push_back(ptSet[ptss[0]]);
        pt_sam.push_back(ptSet[ptss[1]]);

        if (!verifyComposition(pt_sam))
        {
            ++sample_count;
            continue;
        }

        // 计算直线方程
        calcLinePara(pt_sam, a, b, c, res);

        //内点检验
        for (unsigned int i = 0; i < ptSet.size(); i++)
        {
            cv::Point2d pt = ptSet[i];
            double resid_ = fabs(pt.x * a + pt.y * b + c);
            residualstemp.push_back(resid_);
            inlierstemp.push_back(false);
            if (resid_ < residual_error)
            {
                ++inlier_count;
                inlierstemp[i] = true;
            }
        }
        // 找到最佳拟合直线
        if (inlier_count >= maximum)
        {
            maximum = inlier_count;
            resids_ = residualstemp;
            inlierFlag = inlierstemp;
        }
//        // 更新RANSAC迭代次数，以及内点概率
//        if (inlier_count == 0)
//        {
//            N = 500;
//        }
//        else
//        {
//            double epsilon = 1.0 - double(inlier_count) / (double)ptSet.size(); //野值点比例
//            double p = 0.99; //所有样本中存在1个好样本的概率
//            double s = 2.0;
//            N = int(log(1.0 - p) / log(1.0 - pow((1.0 - epsilon), s)));
//        }
        ++sample_count;
    }

    //利用所有内点重新拟合直线
    vector<cv::Point2d> pset;
    for (unsigned int i = 0; i < ptSet.size(); i++)
    {
        if (inlierFlag[i])
            pset.push_back(ptSet[i]);
    }

    if(pset.size() >= 2)
        calcLinePara(pset, a, b, c, res);
}

/**
 * @brief calcLinePara 根据点集拟合直线ax+by+c=0(最小二乘拟合)
 * @param pts
 * @param a
 * @param b
 * @param c
 * @param res 残差
 */
void calcLinePara(vector<cv::Point2d> pts, double &a, double &b, double &c, double &res)
{
    res = 0;
    cv::Vec4f line;
    vector<cv::Point2f> ptsF;
    for (unsigned int i = 0; i < pts.size(); i++)
        ptsF.push_back(pts[i]);

    cv::fitLine(ptsF, line, cv::DIST_L2, 0, 1e-2, 1e-2);
    a = line[1];
    b = -line[0];
    c = line[0] * line[3] - line[1] * line[2];

    for (unsigned int i = 0; i < pts.size(); i++)
    {
        double resid_ = fabs(pts[i].x * a + pts[i].y * b + c);
        res += resid_;
    }
    res /= pts.size();
}

/**
 * @brief getSample 在所有样本点中随机选2个点
 * @param set
 * @param sset
 * @return 是否成功
 */
bool getSample(vector<int> set, vector<int> &sset)
{
    int i[2];
    if (set.size() > 2)
    {
        do
        {
            for (int n = 0; n < 2; n++)
                i[n] = int(uniformRandom() * (set.size() - 1));
        }
        while (!(i[1] != i[0]));
        for (int n = 0; n < 2; n++)
        {
            sset.push_back(i[n]);
        }
    }
    else
    {
        return false;
    }
    return true;
}

/**
 * @brief verifyComposition 两个随机样本点的距离不能太近
 * @param pts
 * @return
 */
bool verifyComposition(const vector<cv::Point2d> pts)
{
    cv::Point2d pt1 = pts[0];
    cv::Point2d pt2 = pts[1];
    if (abs(pt1.x - pt2.x) < 100)
        return false;
    return true;
}

/**
 * @brief uniformRandom 生成[0,1]之间符合均匀分布的数
 * @return
 */
double uniformRandom(void)
{
    return (double)rand() / (double)RAND_MAX;
}

