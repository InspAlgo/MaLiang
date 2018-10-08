#include "grayhistogram.h"
#include "ui_grayhistogram.h"

GrayHistogram::GrayHistogram(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::GrayHistogram)
{
    ui->setupUi(this);

    this->setWindowTitle(tr("Gray Histogram"));
    this->ui->label_histogram->setScaledContents(true);  // 设置控件上图片显示大小的属性为自适应控件
}

GrayHistogram::~GrayHistogram()
{
    delete ui;
}

void GrayHistogram::Init(int *gray_array, int gray_max)
{
    this->gray_array = gray_array;
    this->gray_max = gray_max;

    int maxcount = this->gray_max;

    int h_width = 1000, h_height = 2000;

    QImage hist(h_width, h_height,QImage::Format_RGB888);
    hist.fill(qRgb(255,255,255));

    QPainter p(&hist);
    p.setRenderHint(QPainter::Antialiasing, true);  // 设置反走样，避免绘制出来的线条出现锯齿
    p.setBrush(QBrush(QColor(255,255,255)));
    p.drawRect(0,0,h_width,h_height);
    p.translate(0,h_height);

    p.drawLine(0,0,100,100);

    int wid=h_width-30;
    int hei=h_height-1600;

    p.drawLine(5,-10,wid-10,-10);// 横轴
    p.drawLine(10,-10,10,-hei+10);//纵轴

    float xstep = float(wid-20) / 256;
    float ystep = float(hei-20) / maxcount;

    for (int i = 0; i < 256; i++)
    {

        p.setPen(QColor(13,13,13));
        p.setBrush(QColor(13,13,13));
//        if(i != 255)
//            p.drawLine(QPointF(5+(i+0.5)*xstep,-10-ystep*this->gray_array[i]),QPointF(10+(i+1.5)*xstep,-10-ystep*this->gray_array[i+1]));
        p.drawRect(10+i*xstep,-10,xstep,-10-ystep*this->gray_array[i]);

        if(i % 32 == 0||i==255)
        {
            p.drawText(QPointF(5+(i-0.5)*xstep,0),QString::number(i));
        }
    }

    this->ui->label_histogram->setPixmap(QPixmap::fromImage(hist));
}

void GrayHistogram::Set(int pixel_sum, int gray_mid, int gray_mean, float gray_sd)
{
    this->pixel_sum = pixel_sum;
    this->gray_mid = gray_mid;
    this->gray_mean = gray_mean;
    this->gray_sd = gray_sd;

    this->ui->label_pixel_sum->setText(QString::number(this->pixel_sum));
    this->ui->label_gray_mid->setText(QString::number(this->gray_mid));
    this->ui->label_gray_mean->setText(QString::number(this->gray_mean));
    this->ui->label_gray_sd->setText(QString::number(this->gray_sd));
    this->ui->label_gray_max->setText(QString::number(this->gray_max));
}
