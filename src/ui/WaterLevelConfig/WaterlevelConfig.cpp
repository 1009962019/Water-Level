///**********************************************************************/
///*                          WaterlevelConfig                          */
///*                           水位相机标定界面                            */
///*      通过信号和槽连接界面左侧的输入框和右侧的图像，实现参数修改后的动态更新      */
///*                  具体绘制功能在ScaleableLabel中实现                    */
///**********************************************************************/

//#include "WaterlevelConfig.h"
//#include "ui_WaterlevelConfig.h"

//WaterlevelConfig::WaterlevelConfig(QString pos, QWidget *parent) : QWidget(parent), ui(new Ui::WaterlevelConfig)
//{
//    ui->setupUi(this);
//    setWindowFlags(windowFlags()&~Qt::WindowMaximizeButtonHint);    //禁用最大化按钮
//    setFixedSize(this->width(), this->height());    //固定窗口大小
//    setAttribute(Qt::WA_DeleteOnClose);     //关闭窗口时delete对象

//    //根据闸室内和引航道设置窗口名
//    if (pos.startsWith("inside"))
//    {
//        setWindowTitle(u8"闸室内水位相机标定");
//    }
//    else
//    {
//        setWindowTitle(u8"引航道水位相机标定");
//    }

//    //ROI区域参数被修改时，在界面上实时绘制新的ROI区域
//    connect(ui->lineEdit_roi_x, SIGNAL(textChanged(QString)), this, SLOT(handle_roi_textchanged(QString)));
//    connect(ui->lineEdit_roi_y, SIGNAL(textChanged(QString)), this, SLOT(handle_roi_textchanged(QString)));
//    connect(ui->lineEdit_roi_w, SIGNAL(textChanged(QString)), this, SLOT(handle_roi_textchanged(QString)));
//    connect(ui->lineEdit_roi_h, SIGNAL(textChanged(QString)), this, SLOT(handle_roi_textchanged(QString)));
//    connect(ui->lineEdit_calibration_x, SIGNAL(textChanged(QString)), this, SLOT(handle_roi_textchanged(QString)));

//    connect(ui->label_waterlevel_image, SIGNAL(add_point(int)), this, SLOT(handle_add_point(int))); //连接添加标定点信号和槽函数

//    //获取并显示水位相机参数
//    waterlevel_camera_param = Config::get_instance()->get_waterlevel_param(pos);
//    ui->lineEdit_roi_x->setText(QString::number(waterlevel_camera_param->x1));  //ROI区域x
//    ui->lineEdit_roi_y->setText(QString::number(waterlevel_camera_param->y1));  //ROI区域y
//    ui->lineEdit_roi_w->setText(QString::number(waterlevel_camera_param->w));   //ROI区域w
//    ui->lineEdit_roi_h->setText(QString::number(waterlevel_camera_param->h));   //ROI区域h
//    ui->lineEdit_equation_a->setText(QString::number(waterlevel_camera_param->constant_a));     //水位线固定直线方程参数a
//    ui->lineEdit_equation_b->setText(QString::number(waterlevel_camera_param->constant_b));     //水位线固定直线方程参数b
//    ui->lineEdit_calibration_x->setText(QString::number(waterlevel_camera_param->calibration_location_x));  //标定点x坐标

//    //像素坐标与水位的映射关系，在水位相机参数中是以相对像素坐标保存，这里需要转换为绝对像素坐标。如585,230,221,218转换为585,815,1036,1254
//    vector<vector<double>> mapping_relation;
//    for (int i = 0; i < waterlevel_camera_param->pixel_world_mapping_relation.size(); i++)
//    {
//        pair<double, double> relation = waterlevel_camera_param->pixel_world_mapping_relation[i];
//        if (i == 0)
//        {
//            mapping_relation.push_back({relation.first, relation.second});
//        }
//        else
//        {
//            mapping_relation.push_back({relation.first, mapping_relation[mapping_relation.size() - 1][1] + relation.second});
//        }
//    }

//    //初始化tablewidget
//    ui->tableWidget->setRowCount(mapping_relation.size());  //设置行数
//    ui->tableWidget->setColumnCount(2); //设置列数
//    ui->tableWidget->verticalHeader()->setVisible(false); //隐藏行标签
//    ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);  //列自适应宽度
//    ui->tableWidget->setHorizontalHeaderLabels({u8"实际高程", u8"绝对像素高度"}); //设置列标签
//    show_mapping_relation(mapping_relation);    //初始化表格显示标定数据
//    connect(ui->tableWidget, SIGNAL(cellChanged(int,int)), this, SLOT(handle_mapping_relation_changed(int,int)));   //当表格内容被修改时，动态更新图上显示的标定点
//    handle_mapping_relation_changed(0, 1);  //初始化在图上显示标定点
//}

//WaterlevelConfig::~WaterlevelConfig()
//{
//    delete ui;
//}

///**
// * @brief WaterlevelConfig::set_background_image 设置背景图片
// * @param img
// */
//void WaterlevelConfig::set_background_image(cv::Mat &img)
//{
//    ui->label_waterlevel_image->set_background_image(img);
//}

///**
// * @brief WaterlevelConfig::handle_roi_textchanged 当修改ROI区域和标定点x坐标时，实时在图上显示出来
// * @param arg
// */
//void WaterlevelConfig::handle_roi_textchanged(QString arg)
//{
//    //获取参数
//    ui->label_waterlevel_image->x1 = ui->lineEdit_roi_x->text().toInt();
//    ui->label_waterlevel_image->y1 = ui->lineEdit_roi_y->text().toInt();
//    ui->label_waterlevel_image->w = ui->lineEdit_roi_w->text().toInt();
//    ui->label_waterlevel_image->h = ui->lineEdit_roi_h->text().toInt();
//    ui->label_waterlevel_image->coordinate_x = ui->lineEdit_calibration_x->text().toInt();
//    ui->label_waterlevel_image->update();   //刷新界面
//}

///**
// * @brief WaterlevelConfig::show_mapping_relation 初始化表格时显示映射关系
// */
//void WaterlevelConfig::show_mapping_relation(vector<vector<double>>& mapping_relation)
//{
//    if (mapping_relation.empty()) return ;
//    for (int i = 0; i < mapping_relation.size(); i++)
//    {
//        for (int j = 0; j < mapping_relation[0].size(); j++)
//        {
//            QTableWidgetItem* item = new QTableWidgetItem(QString::number(mapping_relation[i][j]));
//            item->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
//            ui->tableWidget->setItem(i, j, item);   //行、列、元素
//        }
//    }
//}

///**
// * @brief WaterlevelConfig::handle_mapping_relation_changed 当修改表格中绝对像素高度时，实时在图上画点
// * @param row 修改单元格的行数
// * @param col 修改单元格的列数
// */
//void WaterlevelConfig::handle_mapping_relation_changed(int row, int col)
//{
//    if (col != 1) return ;  //只处理像素高度改变的事件，即第一列
//    vector<double> height;
//    for (int i = 0; i < ui->tableWidget->rowCount(); i++)
//    {
//        height.push_back(ui->tableWidget->item(i, 1)->text().toDouble());   //获取表格中所有的绝对像素高度
//    }
//    ui->label_waterlevel_image->pixel_height = height;
//    ui->label_waterlevel_image->update();
//}

///**
// * @brief WaterlevelConfig::handle_add_point 添加标定点槽函数，在图上点击后，在表格内添加绝对像素高度
// * @param y 点击点的像素纵坐标
// */
//void WaterlevelConfig::handle_add_point(int y)
//{
//    int row = ui->tableWidget->rowCount();
//    ui->tableWidget->setRowCount(row + 1);  //扩充表格
//    QTableWidgetItem* item = new QTableWidgetItem(QString::number(y));
//    item->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
//    ui->tableWidget->setItem(row, 1, item); //在表格尾添加元素(第二列，绝对像素高度)

//    item = new QTableWidgetItem("");
//    item->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
//    ui->tableWidget->setItem(row, 0, item); //在表格尾添加空元素(第一列，实际高程)

//    handle_mapping_relation_changed(0, 1);  //手动调用槽函数，更新图上的点
//}

///**
// * @brief WaterlevelConfig::on_pushButton_delete_clicked 删除选中行的标定数据
// */
//void WaterlevelConfig::on_pushButton_delete_clicked()
//{
//    while(ui->tableWidget->selectedItems().count() > 0)
//    {
//      int rowIdx = ui->tableWidget->selectedItems()[0]->row();
//      ui->tableWidget->removeRow(rowIdx);
//    }
//    handle_mapping_relation_changed(0, 1);  //手动调用槽函数，更新图上的点
//}

///**
// * @brief WaterlevelConfig::on_pushButton_save_param_clicked 保存水位相机参数
// */
//void WaterlevelConfig::on_pushButton_save_param_clicked()
//{
//    //保存标定参数
//    vector<pair<double, double>> mapping_relation;
//    for (int i = 0; i < ui->tableWidget->rowCount(); i++)
//    {
//        //判断表格中是否存在空值
//        if (ui->tableWidget->item(i, 0)->text() == "" || ui->tableWidget->item(i, 1)->text() == "")
//        {
//            QMessageBox::warning(NULL, u8"警告", u8"参数配置失败，存在空值！");
//            return ;
//        }

//        //添加像素高度和实际高程的映射关系，由绝对像素高度转为相对像素高度，并要求绝对像素高度严格递增，实际高程严格递减
//        if (i == 0)
//        {
//            mapping_relation.push_back({ui->tableWidget->item(i, 0)->text().toDouble(), ui->tableWidget->item(i, 1)->text().toDouble()});
//        }
//        else
//        {
//            if ((ui->tableWidget->item(i, 0)->text().toDouble() - ui->tableWidget->item(i - 1, 0)->text().toDouble() >= 0) ||
//                (ui->tableWidget->item(i, 1)->text().toDouble() - ui->tableWidget->item(i - 1, 1)->text().toDouble() <= 0))
//            {
//                QMessageBox::warning(NULL, u8"警告", u8"参数配置失败，不严格单调！");
//                return ;
//            }
//            mapping_relation.push_back({ui->tableWidget->item(i, 0)->text().toDouble(), ui->tableWidget->item(i, 1)->text().toDouble() - ui->tableWidget->item(i - 1, 1)->text().toDouble()});
//        }
//    }
//    waterlevel_camera_param->pixel_world_mapping_relation = mapping_relation;

//    //保存ROI区域、固定直线方程、标定点x坐标等参数
//    waterlevel_camera_param->x1 = ui->lineEdit_roi_x->text().toInt();
//    waterlevel_camera_param->y1 = ui->lineEdit_roi_y->text().toInt();
//    waterlevel_camera_param->w = ui->lineEdit_roi_w->text().toInt();
//    waterlevel_camera_param->h = ui->lineEdit_roi_h->text().toInt();
//    waterlevel_camera_param->constant_a = ui->lineEdit_equation_a->text().toDouble();
//    waterlevel_camera_param->constant_b = ui->lineEdit_equation_b->text().toDouble();
//    waterlevel_camera_param->calibration_location_x = ui->lineEdit_calibration_x->text().toInt();

//    Config::get_instance()->cfg_write_all();  //写入ini文件
//    QMessageBox::information(NULL, u8"提示", u8"参数配置成功！");
//}
