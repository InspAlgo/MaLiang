#ifndef WORKINGAREA_H
#define WORKINGAREA_H

#include <QWidget>
#include <QString>
#include <QImage>
#include <QPixmap>
#include <QLabel>
#include <QScrollArea>
#include <QFileDialog>
#include <QtMath>
#include <QMessageBox>
#include <QFile>
#include <QTextStream>

#include "grayhistogram.h"
#include "enhancementsconvolutionsetting.h"
#include "huffman_tree.h"

#include <iostream>
#include <cstring>
#include <string>
#include <thread>
#include <fstream>
#include <cmath>
#include <vector>
#include <queue>

namespace Ui {
class WorkingArea;
}

class WorkingArea : public QWidget
{
    Q_OBJECT

public:
    explicit WorkingArea(QWidget *parent = 0);
    ~WorkingArea();

    // 文件部分的实现
    bool InitImage();  // 初始化加载图片
    int SaveImage(QString save_path);  // 从QLabel导出图片保存

    // 工具栏部分的实现
    void ChangeLabelSize(double scale);  // 改变显示图片用的QLabel控件的大小

    // 模式部分的实现
    void RGB2Gray();   // RGB转灰度图
    void Bit8Slice();  // 针对8位颜色(主要是灰度)，生成8幅位平面
    void Patterning();  // 图案法
    void SelectThresholding(int offset);  // 带有阈值选择的二值化
    void FloydSteinberg();  // Floyd-Steinberg 抖动(dithering)算法
    void ToTxt();  // txt字符画
    void RGB24bitTo256();  // RGB24位真彩图转256色

    // 直方图部分的实现
    void GetGrayHistogram();  // 计算灰度直方图相关
    void HistogramEqualization();  // 直方图均衡化

    // 变换部分的实现
    void TransformLinear(float linear_a, float linear_b);  // 线性变换
    void TransformLaplace(float laplace_a, float laplace_b);  // 拉普拉斯变换
    void TransformLog(float log_a, float log_b, float log_c);  // 对数变换
    void TransformGamma(float gamma_a, float gamma_b, float gamma_c);  // 伽马变换

    // 空间变换部分的实现
    void SpatialTransNN(float size_a, float size_b);  // 最近邻插值缩放
    void SpatialTransBI(float size_a, float size_b);  // 双线性插值缩放
    void SpatialTransRotaing(float size_a);  // 旋转
    void SpatialTransTranslation(float size_a, float size_b);  // 平移

    // 图像增强部分的实现
    void EnhancementsMean();  // 均值平滑
    void EnhancementsMedian();  // 中值平滑
    void EnhancementsKNeighborhood();  // K邻域平滑
    void EnhancementsMinimumMeanVariance();  // 最小均方差平滑
    void EnhancementsRoberts();  // Roberts锐化
    void EnhancementsSobel();  // Sobel锐化
    void EnhancementsLaplace();  // Laplace锐化
    void EnhancementsConvolution(EnhancementsConvolutionSetting *convolution_setting, int type);  // 自定义卷积模板

    // 边缘检测部分的实现
    void EdgeLaplace();
    void EdgeKirsch();
    void EdgeCanny();
    void EdgeTracking();
    void EdgeHuogh();

    // 编码解码部分的实现
    void CodeHuffmanC();
    void CodeHuffmanD();
    void CodeRunsC();
    void CodeRunsD();

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

    EnhancementsConvolutionSetting *convolution_setting;
};

#endif // WORKINGAREA_H
