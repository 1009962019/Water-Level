/******************************************************/
/*                      Config                        */
/*                   系统参数配置界面                    */
/*   包含相机、网络IO模块、网络IP功放等参数，通过ini文件保存   */
/******************************************************/

#include "Config.h"
#include "ui_Config.h"

Config* Config::cfg = nullptr;

Config::Config(QWidget* parent) : QWidget(parent), ui(new Ui::Config)
{
    ui->setupUi(this);
    setWindowFlags(windowFlags() & ~Qt::WindowMaximizeButtonHint); //禁用最大化按钮
    setFixedSize(this->width(), this->height()); //固定窗口大小

    //创建ButtonGroup，添加上游船闸和下游船闸的选择按钮，使得同时只能选择一个选项
    buttongroup = new QButtonGroup;
    buttongroup->addButton(ui->radioButton_upstream, 0);
    buttongroup->addButton(ui->radioButton_downstream, 1);

    load_cfg_file();    //加载配置文件
    cfg_read_all();     //读取所有参数
    cfg_show_connect_param();   //显示所有参数
}

Config::~Config()
{
    delete ui;
    delete m_psetting;
    m_psetting = nullptr;
}

/**
 * @brief Config::get_waterlevel_param 获取水位相机参数
 * @param pos 闸室位置，如inside_waterlevel或outside_waterlevel
 * @return 水位相机参数
 */
WaterlevelCameraParam* Config::get_waterlevel_param(QString pos)
{
    if (pos.startsWith("inside"))
        return &inside_waterlevel_camera_param;
    else
        return &outside_waterlevel_camera_param;
}



/**
 * @brief Config::cfg_read_all  从ini文件中读所有参数到变量中
 */
void Config::cfg_read_all()
{
    /********************闸室内水位相机参数***********************/
    inside_waterlevel_camera_param.host = get_value("inside_waterlevel_camera", "host").toString();
    inside_waterlevel_camera_param.port = get_value("inside_waterlevel_camera", "port").toInt();
    inside_waterlevel_camera_param.username = get_value("inside_waterlevel_camera", "username").toString();
    inside_waterlevel_camera_param.password = get_value("inside_waterlevel_camera", "password").toString();
    inside_waterlevel_camera_param.x1 = get_value("inside_waterlevel_camera", "x1").toInt();
    inside_waterlevel_camera_param.y1 = get_value("inside_waterlevel_camera", "y1").toInt();
    inside_waterlevel_camera_param.w = get_value("inside_waterlevel_camera", "w").toInt();
    inside_waterlevel_camera_param.h = get_value("inside_waterlevel_camera", "h").toInt();
    inside_waterlevel_camera_param.constant_a = get_value("inside_waterlevel_camera", "constant_a").toDouble();
    inside_waterlevel_camera_param.constant_b = get_value("inside_waterlevel_camera", "constant_b").toDouble();
    inside_waterlevel_camera_param.calibration_location_x = get_value("inside_waterlevel_camera", "calibration_location_x").toInt();
    string2vec(get_value("inside_waterlevel_camera", "pixel_world_mapping_relation").toStringList(), inside_waterlevel_camera_param.pixel_world_mapping_relation);


    /********************引航道水位相机参数***********************/
    outside_waterlevel_camera_param.host = get_value("outside_waterlevel_camera", "host").toString();
    outside_waterlevel_camera_param.port = get_value("outside_waterlevel_camera", "port").toInt();
    outside_waterlevel_camera_param.username = get_value("outside_waterlevel_camera", "username").toString();
    outside_waterlevel_camera_param.password = get_value("outside_waterlevel_camera", "password").toString();
    outside_waterlevel_camera_param.x1 = get_value("outside_waterlevel_camera", "x1").toInt();
    outside_waterlevel_camera_param.y1 = get_value("outside_waterlevel_camera", "y1").toInt();
    outside_waterlevel_camera_param.w = get_value("outside_waterlevel_camera", "w").toInt();
    outside_waterlevel_camera_param.h = get_value("outside_waterlevel_camera", "h").toInt();
    outside_waterlevel_camera_param.constant_a = get_value("outside_waterlevel_camera", "constant_a").toDouble();
    outside_waterlevel_camera_param.constant_b = get_value("outside_waterlevel_camera", "constant_b").toDouble();
    outside_waterlevel_camera_param.calibration_location_x = get_value("outside_waterlevel_camera", "calibration_location_x").toInt();
    string2vec(get_value("outside_waterlevel_camera", "pixel_world_mapping_relation").toStringList(), outside_waterlevel_camera_param.pixel_world_mapping_relation);
}

/**
 * @brief Config::cfg_write_all 将变量全部写入ini文件中
 */
void Config::cfg_write_all()
{
    /********************闸室内水位相机参数***********************/
    set_value("inside_waterlevel_camera", "host", inside_waterlevel_camera_param.host);
    set_value("inside_waterlevel_camera", "port", QString::number(inside_waterlevel_camera_param.port));
    set_value("inside_waterlevel_camera", "username", inside_waterlevel_camera_param.username);
    set_value("inside_waterlevel_camera", "password", inside_waterlevel_camera_param.password);
    set_value("inside_waterlevel_camera", "x1", QString::number(inside_waterlevel_camera_param.x1));
    set_value("inside_waterlevel_camera", "y1", QString::number(inside_waterlevel_camera_param.y1));
    set_value("inside_waterlevel_camera", "w", QString::number(inside_waterlevel_camera_param.w));
    set_value("inside_waterlevel_camera", "h", QString::number(inside_waterlevel_camera_param.h));
    set_value("inside_waterlevel_camera", "constant_a", QString::number(inside_waterlevel_camera_param.constant_a));
    set_value("inside_waterlevel_camera", "constant_b", QString::number(inside_waterlevel_camera_param.constant_b));
    set_value("inside_waterlevel_camera", "calibration_location_x", QString::number(inside_waterlevel_camera_param.calibration_location_x));
    set_value("inside_waterlevel_camera", "pixel_world_mapping_relation", vec2string(inside_waterlevel_camera_param.pixel_world_mapping_relation));

    /********************引航道水位相机参数***********************/
    set_value("outside_waterlevel_camera", "host", outside_waterlevel_camera_param.host);
    set_value("outside_waterlevel_camera", "port", QString::number(outside_waterlevel_camera_param.port));
    set_value("outside_waterlevel_camera", "username", outside_waterlevel_camera_param.username);
    set_value("outside_waterlevel_camera", "password", outside_waterlevel_camera_param.password);
    set_value("outside_waterlevel_camera", "x1", QString::number(outside_waterlevel_camera_param.x1));
    set_value("outside_waterlevel_camera", "y1", QString::number(outside_waterlevel_camera_param.y1));
    set_value("outside_waterlevel_camera", "w", QString::number(outside_waterlevel_camera_param.w));
    set_value("outside_waterlevel_camera", "h", QString::number(outside_waterlevel_camera_param.h));
    set_value("outside_waterlevel_camera", "constant_a", QString::number(outside_waterlevel_camera_param.constant_a));
    set_value("outside_waterlevel_camera", "constant_b", QString::number(outside_waterlevel_camera_param.constant_b));
    set_value("outside_waterlevel_camera", "calibration_location_x", QString::number(outside_waterlevel_camera_param.calibration_location_x));
    set_value("outside_waterlevel_camera", "pixel_world_mapping_relation", vec2string(outside_waterlevel_camera_param.pixel_world_mapping_relation));
}

/**
 * @brief Config::cfg_show_connect_param 在参数配置界面上显示值
 */
void Config::cfg_show_connect_param()
{
    /********************闸室内水位相机参数***********************/
    ui->lineEdit_inside_waterlevel_host->setText(inside_waterlevel_camera_param.host);
    ui->lineEdit_inside_waterlevel_port->setText(QString::number(inside_waterlevel_camera_param.port));
    ui->lineEdit_inside_waterlevel_user->setText(inside_waterlevel_camera_param.username);
    ui->lineEdit_inside_waterlevel_password->setText(inside_waterlevel_camera_param.password);

    /********************引航道水位相机参数***********************/
    ui->lineEdit_outside_waterlevel_host->setText(outside_waterlevel_camera_param.host);
    ui->lineEdit_outside_waterlevel_port->setText(QString::number(outside_waterlevel_camera_param.port));
    ui->lineEdit_outside_waterlevel_user->setText(outside_waterlevel_camera_param.username);
    ui->lineEdit_outside_waterlevel_password->setText(outside_waterlevel_camera_param.password);
}

/**
 * @brief Config::cfg_update_connect_param 将参数配置界面上的值更新到变量中
 */
void Config::cfg_update_connect_param()
{


    /********************闸室内水位相机参数***********************/
    inside_waterlevel_camera_param.host = ui->lineEdit_inside_waterlevel_host->text();
    inside_waterlevel_camera_param.port = ui->lineEdit_inside_waterlevel_port->text().toInt();
    inside_waterlevel_camera_param.username = ui->lineEdit_inside_waterlevel_user->text();
    inside_waterlevel_camera_param.password = ui->lineEdit_inside_waterlevel_password->text();



    /********************引航道水位相机参数***********************/
    outside_waterlevel_camera_param.host = ui->lineEdit_outside_waterlevel_host->text();
    outside_waterlevel_camera_param.port = ui->lineEdit_outside_waterlevel_port->text().toInt();
    outside_waterlevel_camera_param.username = ui->lineEdit_outside_waterlevel_user->text();
    outside_waterlevel_camera_param.password = ui->lineEdit_outside_waterlevel_password->text();


}

/**
 * @brief Config::load_cfg_file 加载ini文件
 */
void Config::load_cfg_file()
{
    QDir dir(QApplication::applicationDirPath());
    dir.cdUp();
//    m_psetting = new QSettings(dir.path() + "/cfg.ini", QSettings::IniFormat);
    m_psetting = new QSettings("/home/nvidia/Desktop/Water_Level/cfg.ini", QSettings::IniFormat);
}

/**
 * @brief Config::set_value ini文件写值
 * @param nodename  节点名
 * @param keyname   键
 * @param value     值
 */
void Config::set_value(QString nodename, QString keyname, QVariant value)
{
    m_psetting->setValue(QString("/%1/%2").arg(nodename).arg(keyname), value);
}

/**
 * @brief Config::get_value ini文件读值
 * @param nodename  节点名
 * @param keyname   键
 * @return          值
 */
QVariant Config::get_value(QString nodename, QString keyname)
{
    QVariant qvar = m_psetting->value(QString("/%1/%2").arg(nodename).arg(keyname));
    return qvar;
}

/**
 * @brief Config::string2vec    QString转vector
 * @param str   1 2,3 4,5 6
 * @param vec   {{1,2},{3,4},{5,6}}
 */
void Config::string2vec(QStringList str_list, vector<pair<double, double>>& vec)
{
    for (int i = 0; i < str_list.size(); i++)
    {
        QStringList temp = str_list[i].split(" ");
        pair<double, double> _pair = {temp[0].toDouble(), temp[1].toDouble()};
        vec.push_back(_pair);
    }
}

/**
 * @brief Config::vec2string    vector转QString
 * @param vec   {{1,2},{3,4},{5,6}}
 * @return      1 2,3 4,5 6
 */
QStringList Config::vec2string(vector<pair<double, double>>& vec)
{
    QStringList result;
    for (pair<double, double> v : vec)
    {
        QString res = QString::number(v.first) + " " + QString::number(v.second);
        result.append(res);
    }
    return result;
}

/**
 * @brief Config::on_pushButton_save_clicked 保存参数
 */
void Config::on_pushButton_save_clicked()
{
    cfg_update_connect_param();
    cfg_write_all();
    QMessageBox::information(NULL, u8"提示", u8"参数配置成功！");
}


