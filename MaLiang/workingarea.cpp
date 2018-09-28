#include "workingarea.h"
#include "ui_workingarea.h"

#include <QPainter>
#include <QDebug>

WorkingArea::WorkingArea(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WorkingArea)
{
    ui->setupUi(this);

    this->image = new QImage();

    this->image_label = new QLabel(this);
    this->image_label->setScaledContents(true);  // 设置控件上图片显示大小的属性为自适应控件

    this->scroll_area = new QScrollArea(this);
    this->scroll_area->setGeometry(0, 0, this->width(), this->height());  // 设置控件大小和工作区
    this->scroll_area->setBackgroundRole(QPalette::Dark);  // 背景色
    this->scroll_area->setWidget(this->image_label);
    this->scroll_area->setAlignment(Qt::AlignCenter);  // 居中对齐
}

WorkingArea::~WorkingArea()
{
    delete[] this->buffer;
    delete this->image;
    delete this->image_label;
    delete ui;
}

bool WorkingArea::InitImage()
{
    // 获取图片名称及后缀类型
    QFileInfo file_info(this->image_path);
    this->path = file_info.path();
    this->image_name = file_info.fileName();
    this->file_type = file_info.suffix();

    // 导入图片的二进制流
    this->image->loadFromData(this->buffer, this->image_size);
    // 导入图片到显示控件
    this->image_label->setPixmap(QPixmap::fromImage(*(this->image)));

    // 调整控件大小
    this->width_height = (double)this->image->width() / (double)this->image->height();
    this->AutoSize();  // 使用自适应大小模式
    this->auto_height = this->image_label->height();
    this->auto_width = this->image_label->width();
    this->is_saved = 0;
    return true;
}

int WorkingArea::SaveImage(QString save_path)
{
    this->image_label->pixmap()->toImage().save(save_path);
    this->is_saved = 0;

    return 0;
}

void WorkingArea::AutoSize()
{
    int label_width, label_height;
    if(this->image->width() < this->width() -10)
        label_width = this->image->width();
    else
        label_width = 0;
    if(this->image->height() < this->height() -10)
        label_height = this->image->height();
    else
        label_height = 0;

    if(label_width == 0 || label_height == 0)
    {
        label_height = this->height() -10;
        label_width = int(label_height * this->width_height);

        for(;label_height >= this->height() - 10 || label_width >= this->width() -10;)
        {
            label_height--;
            label_width = int(label_height * this->width_height);
        }
    }

    this->image_label->resize(label_width, label_height);
}

void WorkingArea::ChangeLabelSize(double scale)
{
    int new_height = int(this->auto_height * scale);
    int new_width =  int(this->width_height * new_height);

    this->image_label->resize(new_width, new_height);
}

void WorkingArea::RGB2Gray()
{
    int old_height = this->image->height();
    int old_width = this->image->width();

    QImage gray_img(old_width, old_height, QImage::Format_Indexed8);  //  8 位图
    gray_img.setColorCount(256);  // 256 种颜色

    for(int i = 0; i < 256; i++)
        gray_img.setColor(i, qRgb(i, i, i));  // 颜色表

    switch (this->image->format())
    {
        case QImage::Format_Indexed8:
            for(int i = 0; i < old_height; i++)
            {
                const uchar *p_src = (uchar * )this->image->constScanLine(i);
                uchar *p_dest = (uchar *)gray_img.scanLine(i);
                memcpy(p_dest, p_src, old_width);
            }
            break;
        case QImage::Format_RGB32:
        case QImage::Format_ARGB32:
        case QImage::Format_ARGB32_Premultiplied:
            for(int i = 0; i < old_height; i++)
            {
                const QRgb *p_src = (QRgb *)this->image->constScanLine(i);
                uchar *p_dest = (uchar *)gray_img.scanLine(i);

                for(int j =0; j <old_width; j++)
                    p_dest[j] = qGray(p_src[j]);  // (R*11+G*16+B*5)/32
            }
            break;
        default:
            break;
    }

    this->image_label->setPixmap(QPixmap::fromImage(gray_img));
}

void ThreadBit8Slice(QImage *temp, int bit, QString save_path)
{
    int width = temp->width();
    int height = temp->height();

    for(int i = 0; i < width; i++)
    {
        for(int j = 0; j < height; j++)
        {
            temp->setPixel(i, j,
                ((temp->pixel(i, j)&(1 << bit)) == 0? qRgb(0,0,0):qRgb(255,255,255)));
        }
    }

    temp->save(save_path);
}

void WorkingArea::Bit8Slice()
{
    QImage temp[8];
    for(int i = 0; i < 8; i++)
        temp[i] = *this->image;

    std::thread *th[8];

    QString save_path = this->path + "/" + this->image_name + "_8bit_slice_";
    for(int bit = 0; bit < 8; bit++)
    {
        QString path = save_path + QString::number(bit) + "." + this->file_type;
        th[bit] = new std::thread(ThreadBit8Slice, &temp[bit], bit, path);
    }

    for(int i = 0; i < 8; i++)
        th[i]->join();
    for(int i = 0; i < 8; i++)
        delete th[i];
}

void WorkingArea::Patterning()
{

}

void WorkingArea::SelectThresholding(int offset)
{
    int old_width = this->image->width();
    int old_height = this->image->height();
    QImage ret = *this->image;

    for(int i = 0; i < old_width; i++)
    {
        for(int j = 0; j < old_height; j++)
        {
            ret.setPixel(i, j,
                (ret.pixel(i, j) >= qRgb(128+offset,128+offset,128+offset)? qRgb(255,255,255):qRgb(0,0,0)));
        }
    }

    this->image_label->setPixmap(QPixmap::fromImage(ret));
}

void WorkingArea::FloydSteinberg()
{
    int old_width = this->image->width();
    int old_height = this->image->height();
    QImage ret = *this->image;

    int old_rgb,add_rgb;
    for(int i = 0; i < old_width - 1; i++)
    {
       for(int j = 0; j < old_height - 1; j++)
       {
           int diff;
           if(ret.pixel(i, j) > qRgb(128,128,128))
           {
               diff = qGray(ret.pixel(i, j)) - 255;
               ret.setPixel(i, j, qRgb(255,255,255));
           }
           else
           {
               diff=qGray(ret.pixel(i, j));
               ret.setPixel(i, j, qRgb(0,0,0));
           }

           old_rgb = qGray(ret.pixel(i+1, j));
           add_rgb = old_rgb+diff*3/16;
           ret.setPixel(i+1,j,qRgb(add_rgb,add_rgb,add_rgb));

           old_rgb = qGray(ret.pixel(i, j+1));
           add_rgb = old_rgb+diff*3/16;
           ret.setPixel(i,j+1,qRgb(add_rgb,add_rgb,add_rgb));

           old_rgb = qGray(ret.pixel(i+1, j+1));
           add_rgb = old_rgb+diff*1/4;
           ret.setPixel(i+1,j+1,qRgb(add_rgb,add_rgb,add_rgb));
       }

       ret.setPixel(i, old_height - 1,
           (ret.pixel(i, old_height - 1) >= qRgb(128,128,128)? qRgb(255,255,255):qRgb(0,0,0)));
    }

    for(int j = 0; j < old_height; j++)
       ret.setPixel(old_width - 1, j,
           (ret.pixel(old_width - 1, j) >= qRgb(128,128,128)? qRgb(255,255,255):qRgb(0,0,0)));

    this->image_label->setPixmap(QPixmap::fromImage(ret));
}

void WorkingArea::ToTxt()
{
    const char ch_txt[] = "@B%8&WM#*oahkbdpqwmZO0QLCJUYXzcvunxrjft/\\|()1{}[]?-_+~<>i!lI;:,\"^`'.";
    QString save_path = this->path + "/" + this->image_name + "_2txt.txt";
    int old_width = this->image->width();
    int old_height = this->image->height();

    std::string content = "";

    for(int i = 0; i < old_height; i++)
    {
        for(int j = 0; j < old_width; j++)
        {
            int gray = qGray(this->image->pixel(j,i));
            int gray_lever = 69.0/255.0*gray;
            content += ch_txt[gray_lever];
        }
        content += "\n";
    }

    std::fstream file((char *)save_path.toStdString().data(), std::ios::out);
    file << content;
    file.close();
}

void WorkingArea::GetGrayHistogram()
{

}
