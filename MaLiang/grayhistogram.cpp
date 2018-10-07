#include "grayhistogram.h"
#include "ui_grayhistogram.h"

GrayHistogram::GrayHistogram(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::GrayHistogram)
{
    ui->setupUi(this);
}

GrayHistogram::~GrayHistogram()
{
    delete ui;
}

void GrayHistogram::Init(int *gray_array, int gray_max, QString &gray_info)
{
    this->gray_array = gray_array;
    this->gray_max = gray_max;

    int maxcount = this->gray_max;

    int h_width = 1000, h_height = 1000;

    QImage hist(h_width, h_height,QImage::Format_RGB888);
    hist.fill(qRgb(255,255,255));

    QPainter p(&hist);
    p.setBrush(QBrush(QColor(255,255,255)));
    p.drawRect(0,0,h_width,h_height);
    p.translate(0,h_height);

    p.drawLine(0,0,100,100);

    int wid=h_width-30;
    int hei=h_height-600;

    p.drawLine(10,-10,wid-10,-10);// 横轴
    p.drawLine(10,-10,10,-hei+10);//纵轴

    float xstep = float(wid-20) / 256;
    float ystep = float(hei-20) / maxcount;

    for (int i = 0; i < 256; i++)
    {

        p.setPen(QColor(13,13,13));
        p.setBrush(QColor(13,13,13));
        if(i != 255)
            p.drawLine(QPointF(10+(i+0.5)*xstep,-10-ystep*this->gray_array[i]),QPointF(10+(i+1.5)*xstep,-10-ystep*this->gray_array[i+1]));
        //p.drawRect(10+i*xstep,-10,xstep,-10-ystep*this->gray_array[i]);

        if(i % 32 == 0||i==255)
        {
            p.drawText(QPointF(10+(i-0.5)*xstep,0),QString::number(i));
        }
    }

    hist.save("D:/Users/AS/Desktop/cshis.bmp");
}

