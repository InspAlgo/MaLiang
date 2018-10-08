#ifndef GRAYHISTOGRAM_H
#define GRAYHISTOGRAM_H

#include <QWidget>
#include <QPainter>
#include <QImage>
#include <QPaintEvent>

namespace Ui {
class GrayHistogram;
}

class GrayHistogram : public QWidget
{
    Q_OBJECT

public:
    explicit GrayHistogram(QWidget *parent = 0);
    ~GrayHistogram();

    void Init(int *gray_array, int gray_max);
    void Set(int pixel_sum, int gray_mid, int gray_mean, float gray_sd);
private:
    Ui::GrayHistogram *ui;

    int *gray_array;  // 灰度数组
    int gray_max;  // 灰度数组中数目最多的那个灰度值的个数
    int pixel_sum;
    int gray_mid;
    int gray_mean;
    float gray_sd;
};

#endif // GRAYHISTOGRAM_H
