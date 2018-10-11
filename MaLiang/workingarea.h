#ifndef WORKINGAREA_H
#define WORKINGAREA_H

#include <QWidget>
#include <QString>
#include <QImage>
#include <QPixmap>
#include <QLabel>
#include <QScrollArea>
#include <QFileDialog>

#include "grayhistogram.h"

#include <thread>
#include <fstream>
#include <cmath>

namespace Ui {
class WorkingArea;
}

class WorkingArea : public QWidget
{
    Q_OBJECT

public:
    explicit WorkingArea(QWidget *parent = 0);
    ~WorkingArea();

    bool InitImage();  // 初始化加载图片
    int SaveImage(QString save_path);  // 从QLabel导出图片保存
    void ChangeLabelSize(double scale);  // 改变显示图片用的QLabel控件的大小
    void RGB2Gray();   // RGB转灰度图
    void Bit8Slice();  // 针对8位颜色(主要是灰度)，生成8幅位平面
    void Patterning();  // 图案法
    void SelectThresholding(int offset);  // 带有阈值选择的二值化
    void FloydSteinberg();  // Floyd-Steinberg 抖动(dithering)算法
    void ToTxt();  // txt字符画
    void GetGrayHistogram();  // 计算灰度直方图相关
    void HistogramEqualization();  // 直方图均衡化

    int is_saved;  // 状态类型
    unsigned char *buffer;
    unsigned long image_size;
    QString image_path;
    int image_type;

public slots:


private:
    Ui::WorkingArea *ui;

    void AutoSize();  // 自适应大小

    QImage *image;  // 存显示的图片
    QLabel *image_label;  // 显示图片的用的载体控件
    QScrollArea *scroll_area;  // 滚动显示区，当图片过大时出现滚动条
    double width_height;  // 图片原始宽高比
    int auto_width;  // 自适应时的宽度
    int auto_height;  // 自适应时的高度
    QString image_name;  // 不包含后缀的文件名
    QString file_type;  // 文件后缀
    QString path;  // 不包含文件名的路径
};

#endif // WORKINGAREA_H
