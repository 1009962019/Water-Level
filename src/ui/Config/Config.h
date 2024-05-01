#ifndef Config_H
#define Config_H

#include <QWidget>
#include <QVariant>
#include <QSettings>
#include <QDir>
#include <QDebug>
#include <QMessageBox>
#include <QButtonGroup>
#include <QFileDialog>

#include "common/ParamSet.h"

namespace Ui
{
    class Config;
}

class Config : public QWidget
{
    Q_OBJECT

public:
    ~Config();

    // 单例模式(懒汉)
    static Config *get_instance()
    {
        if (cfg == nullptr)
            cfg = new Config();
        return cfg;
    }

    void cfg_read_all();             // 从ini文件中读所有参数到变量中
    void cfg_write_all();            // 将变量全部写入ini文件中
    void cfg_show_connect_param();   // 在参数配置界面上显示值
    void cfg_update_connect_param(); // 将参数配置界面上的值更新到变量中

    WaterlevelCameraParam inside_waterlevel_camera_param, outside_waterlevel_camera_param; // 闸室内和引航道水位相机参数

    WaterlevelCameraParam *get_waterlevel_param(QString pos); // 获取水位相机参数

private slots:
    void on_pushButton_save_clicked();

private:
    Ui::Config *ui;
    Config(QWidget *parent = 0);
    static Config *cfg; // 单例模式

    QSettings *m_psetting;     // ini文件读取实例指针
    QButtonGroup *buttongroup; // 上下游模式按钮组

    void load_cfg_file();                                              // 加载配置文件
    void set_value(QString nodename, QString keyname, QVariant value); // ini文件写值
    QVariant get_value(QString nodename, QString keyname);             // ini文件读值

    QStringList vec2string(vector<pair<double, double>> &vec);                // vector转QString
    void string2vec(QStringList str_list, vector<pair<double, double>> &vec); // QString转vector
};

#endif // Config_H
