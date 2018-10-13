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

    QImage gray_img = *this->image;
    int gray = 0;

    for(int i = 0; i < old_width; i++)
    {
        for(int j = 0; j < old_height; j++)
        {
            gray = qGray(this->image->pixel(i, j));  // Gray = (R * 11 + G * 16 + B * 5)/32
            gray_img.setPixel(i, j, qRgba(gray, gray, gray, qAlpha(this->image->pixel(i, j))));  // qAlpha() 为保留原透明度
        }
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

void WorkingArea::SelectThresholding(int val)
{
    int old_width = this->image->width();
    int old_height = this->image->height();
    QImage ret = *this->image;

    for(int i = 0; i < old_width; i++)
    {
        for(int j = 0; j < old_height; j++)
        {
            ret.setPixel(i, j,
                (ret.pixel(i, j) > qRgb(val,val,val)? qRgb(255,255,255):qRgb(0,0,0)));
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
//    const char ch_txt[] = "##############################........................................";
    QString save_path = this->path + "/" + this->image_name + "_2txt.txt";
    int old_width = this->image->width();
    int old_height = this->image->height();

    std::string content = "";

    for(int i = 0; i < old_height; i++)
    {
//        content += "\"";
        for(int j = 0; j < old_width; j++)
        {
            int gray = qGray(this->image->pixel(j,i));
            int gray_lever = 69.0/255.0*gray;
            content += ch_txt[gray_lever];
        }
        content += "\n";
    }

    std::fstream file((char *)save_path.toLocal8Bit().data(), std::ios::out);
    file << content;
    file.close();
}

void WorkingArea::GetGrayHistogram()
{
    int gray_sum = 0;     // 图片总灰度

    QImage label_image = this->image_label->pixmap()->toImage();

    int image_width = label_image.width();
    int image_height = label_image.height();


    int pixel_gray = 0;  // 当前像素点的灰度值
    int pixel_sum = image_width * image_height;    // 总像素数

    int gray_array[256];  // 灰度数组
    memset(gray_array,0,sizeof(gray_array));

    // 统计灰度数组
    for(int i = 0; i < image_width; i++)
    {
        for(int j = 0; j < image_height; j++)
        {
            pixel_gray = qGray(label_image.pixel(i, j));
            gray_array[pixel_gray]++;
            gray_sum += pixel_gray;
        }
    }

    int gray_mean = gray_sum / pixel_sum;  // 平均灰度值

    // 求灰度中位数，从两端开始搜，两端较大的减去较小的
    int pointer_st = 0, pointer_ed = 255;
    int gray_copy_array[256];
    memcpy(gray_copy_array, gray_array,sizeof(gray_array));
    while(pointer_ed > pointer_st)
    {
        if(gray_copy_array[pointer_st] > gray_copy_array[pointer_ed])
        {
            gray_copy_array[pointer_st] -= gray_copy_array[pointer_ed];
            gray_copy_array[pointer_ed] = 0;
        }
        else
        {
            gray_copy_array[pointer_ed] -= gray_copy_array[pointer_st];
            gray_copy_array[pointer_st] = 0;
        }

        if(gray_copy_array[pointer_st] <= 0)
            pointer_st++;
        else
            gray_copy_array[pointer_st]--;

        if(gray_copy_array[pointer_ed] <= 0)
            pointer_ed--;
        else
            gray_copy_array[pointer_ed]--;
    }

    int gray_mid = (pointer_st + pointer_ed) / 2;  // 灰度中位数

    float gray_standard_deviation = 0;  // 灰度标准差
    int gray_max = 0;  // 最大灰度
    for(int i = 0; i < 256; i++)  // 加权平均，而非直接 1/n
    {
        if(gray_array[pixel_gray] > gray_max)  // 考虑减少复杂度，所以把求最多灰度值放这里了
            gray_max = gray_array[pixel_gray];

        gray_standard_deviation += float((gray_mean - i)*(gray_mean - i))/float(pixel_sum)*float(gray_array[i]);
    }

    gray_standard_deviation = sqrt(gray_standard_deviation);

    GrayHistogram *gray_histogram = new GrayHistogram(NULL);
    gray_histogram->Init(gray_array, gray_max);
    gray_histogram->Set(pixel_sum, gray_mid, gray_mean, gray_standard_deviation);
    gray_histogram->show();
}


void WorkingArea::HistogramEqualization()
{
    int image_width = this->image->width();
    int image_height = this->image->height();
    int pixel_sum = image_width * image_height;  // 总像素数
    int r_hist[256], g_hist[256], b_hist[256];  // rgb各分量灰度级数组
    int r_equ[256], g_equ[256], b_equ[256];  // 均衡化后 rgb 各分量灰度级数组
    memset(r_hist, 0, sizeof(r_hist));
    memset(g_hist, 0, sizeof(g_hist));
    memset(b_hist, 0, sizeof(b_hist));
    float r_old[256], g_old[256], b_old[256];  // 均衡化前各灰度级概率
    float r_new[256], g_new[256], b_new[256];  // 均衡化后各灰度级概率

    QRgb pixel_rgb;

    // 统计各灰度级数量
    for(int i = 0; i < image_width; i++)
    {
        for(int j = 0; j < image_height; j++)
        {
            pixel_rgb = this->image->pixel(i, j);
            r_hist[qRed(pixel_rgb)]++;
            g_hist[qGreen(pixel_rgb)]++;
            b_hist[qBlue(pixel_rgb)]++;
        }
    }

    // 均衡化前各灰度级概率
    for(int i = 0; i < 256; i++)
    {
        r_old[i] = (float)r_hist[i]/(float)pixel_sum;
        g_old[i] = (float)g_hist[i]/(float)pixel_sum;
        b_old[i] = (float)b_hist[i]/(float)pixel_sum;
    }

    // 均衡化后各灰度级概率
    r_new[0] = r_old[0];
    g_new[0] = g_old[0];
    b_new[0] = b_old[0];
    for(int i = 1; i < 256; i++)
    {
        r_new[i] = r_new[i - 1] + r_old[i];
        g_new[i] = g_new[i - 1] + g_old[i];
        b_new[i] = b_new[i - 1] + b_old[i];
    }

    // 均衡化后对应的像素值
    for(int i = 0; i < 256; i++)
    {
        r_equ[i] = int(r_new[i]*255);
        g_equ[i] = int(g_new[i]*255);
        b_equ[i] = int(b_new[i]*255);
    }

    QImage ret = *this->image;
    for(int i = 0; i < image_width; i++)
    {
        for(int j = 0; j < image_height; j++)
        {
            pixel_rgb = this->image->pixel(i, j);
            ret.setPixel(i, j,
                qRgba(r_equ[qRed(pixel_rgb)], g_equ[qGreen(pixel_rgb)], b_equ[qBlue(pixel_rgb)], qAlpha(pixel_rgb)));
        }
    }

    this->image_label->setPixmap(QPixmap::fromImage(ret));
}

void WorkingArea::TransformLinear(float linear_a, float linear_b)
{
    int image_width = this->image->width();
    int image_height = this->image->height();

    QImage ret = *this->image;
    QRgb rgb;

    int r_new, g_new, b_new;

    for(int i = 0; i < image_width; i++)
    {
        for(int j = 0; j < image_height; j++)
        {
            rgb = this->image->pixel(i, j);
            r_new = int(qRed(rgb) * linear_a + linear_b);
            g_new = int(qGreen(rgb) * linear_a + linear_b);
            b_new = int(qBlue(rgb) * linear_a + linear_b);

            if(r_new > 255)
                r_new = 255;
            if(r_new < 0)
                r_new = 0;
            if(g_new > 255)
                g_new = 255;
            if(g_new < 0)
                g_new = 0;
            if(b_new > 255)
                b_new = 255;
            if(b_new < 0)
                b_new = 0;

            ret.setPixel(i, j, qRgba(r_new, g_new, b_new, qAlpha(rgb)));
        }
    }

    this->image_label->setPixmap(QPixmap::fromImage(ret));
}

void WorkingArea::TransformLaplace(float laplace_a, float laplace_b)
{
    int image_width = this->image->width();
    int image_height = this->image->height();

    QImage ret = *this->image;
    QRgb rgb,rgb1,rgb2,rgb3,rgb4;

    int r_new, g_new, b_new;

    for(int i = 0; i < image_width; i++)
    {
        for(int j = 0; j < image_height; j++)
        {
            if(i + 1 >= image_width || j + 1 >= image_height
                || i - 1 < 0 || j - 1 < 0)
                continue;

            rgb = this->image->pixel(i, j);
            rgb1 = this->image->pixel(i + 1, j);
            rgb2 = this->image->pixel(i - 1, j);
            rgb3 = this->image->pixel(i, j + 1);
            rgb4 = this->image->pixel(i, j - 1);

            r_new = qRed(rgb) + 4*qRed(rgb)-qRed(rgb1)-qRed(rgb2)-qRed(rgb3)-qRed(rgb4);
            g_new = qGreen(rgb) + 4*qGreen(rgb)-qGreen(rgb1)-qGreen(rgb2)-qGreen(rgb3)-qGreen(rgb4);
            b_new = qBlue(rgb) +  4*qBlue(rgb)-qBlue(rgb1)-qBlue(rgb2)-qBlue(rgb3)-qBlue(rgb4);

            if(r_new > 255)
                r_new = 255;
            if(r_new < 0)
                r_new = 0;
            if(g_new > 255)
                g_new = 255;
            if(g_new < 0)
                g_new = 0;
            if(b_new > 255)
                b_new = 255;
            if(b_new < 0)
                b_new = 0;

            ret.setPixel(i, j, qRgba(r_new, g_new, b_new, qAlpha(rgb)));
        }
    }

    this->image_label->setPixmap(QPixmap::fromImage(ret));
}

void WorkingArea::TransformLog(float log_a, float log_b, float log_c)
{
    int image_width = this->image->width();
    int image_height = this->image->height();

    QImage ret = *this->image;
    QRgb rgb;

    int r_new, g_new, b_new;

    for(int i = 0; i < image_width; i++)
    {
        for(int j = 0; j < image_height; j++)
        {
            rgb = this->image->pixel(i, j);
            r_new = int(log_c * qLn(qRed(rgb) + log_b) / qLn(log_a));
            g_new = int(log_c * qLn(qGreen(rgb) + log_b) / qLn(log_a));
            b_new = int(log_c * qLn(qBlue(rgb) + log_b) / qLn(log_a));

            if(r_new > 255)
                r_new = 255;
            if(r_new < 0)
                r_new = 0;
            if(g_new > 255)
                g_new = 255;
            if(g_new < 0)
                g_new = 0;
            if(b_new > 255)
                b_new = 255;
            if(b_new < 0)
                b_new = 0;

            ret.setPixel(i, j, qRgba(r_new, g_new, b_new, qAlpha(rgb)));
        }
    }

    this->image_label->setPixmap(QPixmap::fromImage(ret));
}

void WorkingArea::TransformGamma(float gamma_a, float gamma_b, float gamma_c)
{
    int image_width = this->image->width();
    int image_height = this->image->height();

    QImage ret = *this->image;
    QRgb rgb;

    int r_new, g_new, b_new;

    for(int i = 0; i < image_width; i++)
    {
        for(int j = 0; j < image_height; j++)
        {
            rgb = this->image->pixel(i, j);
            r_new = int(gamma_c * qPow(qRed(rgb) + gamma_b, gamma_a));
            g_new = int(gamma_c * qPow(qGreen(rgb) + gamma_b, gamma_a));
            b_new = int(gamma_c * qPow(qBlue(rgb) + gamma_b, gamma_a));

            if(r_new > 255)
                r_new = 255;
            if(r_new < 0)
                r_new = 0;
            if(g_new > 255)
                g_new = 255;
            if(g_new < 0)
                g_new = 0;
            if(b_new > 255)
                b_new = 255;
            if(b_new < 0)
                b_new = 0;

            ret.setPixel(i, j, qRgba(r_new, g_new, b_new, qAlpha(rgb)));
        }
    }

    this->image_label->setPixmap(QPixmap::fromImage(ret));
}
