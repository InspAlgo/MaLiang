#include "workingarea.h"
#include "ui_workingarea.h"

#include <QPainter>
#include <QDebug>

#include <algorithm>

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

void WorkingArea::RGB24bitTo256()
{
    int colorstatistic[4096];  // 原始数据
    int colorindex[4096];  // 存放颜色索引
    memset(colorstatistic, 0 , sizeof(colorstatistic));
    memset(colorindex, 0, sizeof(colorindex));
    int w = this->image->width();
    int h = this->image->height();

    // 近似替换，取RGB像素的高4位组成一个12位数
    for(int i = 0; i < w; i++)
    {
        for(int j = 0; j < h; j++)
        {
            int r = (qRed(this->image->pixel(i,j)))&0xf0;
            int g = (qGreen(this->image->pixel(i,j)))&0xf0;
            int b = (qBlue(this->image->pixel(i,j)))&0xf0;
            int index = (r<<4)+g+(b>>4);
            colorstatistic[index]++;
        }
    }

    // 将有值的保存下来
    int palCounts=0;
    for(int i = 0; i < 4096; i++)  // i代表颜色索引
    {
        if(colorstatistic[i] != 0)
        {
            colorstatistic[palCounts] = colorstatistic[i];  // 保存各种颜色索引出现的次数
            colorindex[palCounts] = i;  // 保存各种颜色的索引，与colorstatistic中的颜色出现的次数一一对应
            palCounts++;
        }
    }

    // 冒泡排序
    for(int i = 0; i < palCounts-1; i++)
    {
        for(int j = i+1; j < palCounts; j++)
        {
            if(colorstatistic[j] > colorstatistic[i])
            {
                int temp=colorstatistic[i];
                colorstatistic[i]=colorstatistic[j];  // 根据出现的次数
                colorstatistic[j] = temp;

                temp = colorindex[i];
                colorindex[i] = colorindex[j];  // 对颜色索引排序
                colorindex[j] = temp;
            }
        }
    }

    QImage imgindex8;
    imgindex8 = this->image->convertToFormat(QImage::Format_Indexed8);
    memset(colorstatistic,0,sizeof(colorstatistic));

    //获取调色板
    for(int i=0;i<256;i++)
    {
        int r = (colorindex[i]&0xf00)>>4;
        int g = colorindex[i]&0x0f0;
        int b = (colorindex[i]&0x00f)<<4;
        imgindex8.setColor(i,qRgb(r,g,b));
        colorstatistic[i]=i;  // 此时已经不需要颜色出现的次数,出现最多的256种颜色对应调色板中的256种颜色
    }

    if(palCounts > 256)
    {
        // 将超过的部分也映射到前 256 色中
        for(int i = 256; i < palCounts; i++)
        {
            long error = 999999999999;
            int red = (colorindex[i]&0xf00)>>4;  // colorindex中存放的是colorstatistic的颜色索引
            int green = (colorindex[i]&0x0f0);
            int blue = (colorindex[i]&0x00f)<<4;

            int clrIndex=0;

            // 颜色近似替换，找前256种颜色中最接近的做替换
            for(int j=0;j<256;j++)
            {
                long error2=(long)((qBlue(imgindex8.color(j))-blue)*(qBlue(imgindex8.color(j))-blue)+
                                   (qGreen(imgindex8.color(j))-green)*(qGreen(imgindex8.color(j))-green)+
                                   (qRed(imgindex8.color(j))-red)*(qRed(imgindex8.color(j))-red));
                if(error2<error)
                {
                    error=error2;
                    clrIndex=j;
                }
            }
            colorstatistic[i]=clrIndex;  // 保存所有颜色在调色板中的对应颜色
        }
    }

    int MapTable[4096];
    memset(MapTable, 0, sizeof(MapTable));
    for(int i = 0; i < 4096; i++)
    {
        int index = colorindex[i];  // 获取原图中的颜色
        MapTable[index] = colorstatistic[i];  // 获取在颜色表中的映射
    }

    for(int i = 0; i < w; i++)
    {
        for(int j = 0; j < h; j++)
        {
            int r=(qRed(this->image->pixel(i,j)))&0xf0;
            int g=(qGreen(this->image->pixel(i,j)))&0xf0;
            int b=(qBlue(this->image->pixel(i,j)))&0xf0;
            int index = (r<<4)+g+(b>>4);
            imgindex8.setPixel(i,j,MapTable[index]);
        }
    }

    this->image_label->setPixmap(QPixmap::fromImage(imgindex8));
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

void WorkingArea::SpatialTransNN(float size_a, float size_b)
{
    QImage ret(this->image->width()*size_a, this->image->height()*size_b, this->image->format());
    int new_width = ret.width();
    int new_height = ret.height();

    QRgb rgb;
    QColor color;

    for(int i = 0; i < new_width; i++)
    {
        for(int j = 0; j < new_height; j++)
        {
            rgb = this->image->pixel(int(i/size_a), int(j/size_b));
            color = QColor(rgb);

            ret.setPixel(i, j, qRgba(color.red(),color.green(), color.blue(), qAlpha(rgb)));
        }
    }

    this->image_label->setPixmap(QPixmap::fromImage(ret));
}

void WorkingArea::SpatialTransBI(float size_a, float size_b)
{
    QImage ret(this->image->width()*size_a, this->image->height()*size_b, this->image->format());
    int new_width = ret.width();
    int new_height = ret.height();

    QRgb rgb1, rgb2, rgb3, rgb4;
    int red, green, blue;

    for(int i = 0; i < new_width; i++)
    {
        for(int j = 0; j < new_height; j++)
        {
            float u = float(i/size_a) - int(i/size_a);  // 取小数
            float v = float(j/size_b) - int(j/size_b);

            int x = float(i/size_a) - 1.0f < 0.0f? 0: float(i/size_a) - 1;  // 取整数
            int y = float(j/size_b) - 1.0f < 0.0f? 0: float(j/size_b) - 1;

            rgb1 = this->image->pixel(x, y);
            rgb2 = this->image->pixel(x, y + 1);
            rgb3 = this->image->pixel(x + 1, y);
            rgb4 = this->image->pixel(x + 1, y + 1);

            red = QColor(rgb1).red()*(1.0f - u)*(1.0f - v) +
                    QColor(rgb2).red()*(1.0f - u)*(v) +
                    QColor(rgb3).red()*(u)*(1.0f - v) +
                    QColor(rgb4).red()*(u)*(v);

            green = QColor(rgb1).green()*(1.0f - u)*(1.0f - v) +
                    QColor(rgb2).green()*(1.0f - u)*(v) +
                    QColor(rgb3).green()*(u)*(1.0f - v) +
                    QColor(rgb4).green()*(u)*(v);

            blue = QColor(rgb1).blue()*(1.0f - u)*(1.0f - v) +
                    QColor(rgb2).blue()*(1.0f - u)*(v) +
                    QColor(rgb3).blue()*(u)*(1.0f - v) +
                    QColor(rgb4).blue()*(u)*(v);

            if(red < 0)
                red = 0;
            if(red > 255)
                red = 255;
            if(green < 0)
                green = 0;
            if(green > 255)
                green = 255;
            if(blue < 0)
                blue = 0;
            if(blue > 255)
                blue = 255;

            ret.setPixel(i, j, qRgb(red, green, blue));
        }
    }

    this->image_label->setPixmap(QPixmap::fromImage(ret));
}

void WorkingArea::SpatialTransRotaing(float size_a)
{
    int diagonal= sqrt(this->image->width()*this->image->width() + this->image->height()*this->image->height());

    QImage ret(2*diagonal, 2*diagonal, this->image->format());

    int width = this->image->width();
    int height = this->image->height();

    float angle = size_a*3.1415926/180;

    QColor col;

    for(int i = 0; i < width; i++)
    {
        for(int j = 0; j < height; j++)
        {
            int x = i*cos(angle) + j*sin(angle);
            int y = j*cos(angle) - i*sin(angle);

            col = QColor(this->image->pixel(i, j));

            ret.setPixel(x + diagonal, y + diagonal,
                        qRgba(col.red(), col.green(), col.blue(), col.alpha()));

            if((i && j) || (i != width && j != height))
            {
                ret.setPixel(x + diagonal + 1, y + diagonal + 1,
                             qRgba(col.red(), col.green(), col.blue(), col.alpha()));
                ret.setPixel(x + diagonal - 1, y + diagonal - 1,
                             qRgba(col.red(), col.green(), col.blue(), col.alpha()));
            }
        }
    }

    this->image_label->setPixmap(QPixmap::fromImage(ret));
}

void WorkingArea::SpatialTransTranslation(float size_a, float size_b)
{
    int width = this->image->width();
    int height = this->image->height();

    QImage ret(width + fabs(size_a), height + fabs(size_b), this->image->format());

    QColor col;

    for(int i = 0; i < width; i++)
    {
        for(int j = 0; j < height; j++)
        {
            col = QColor(this->image->pixel(i,j));

            ret.setPixel(i + size_a > 0? i + size_a: i - size_a,
                         j + size_b > 0? j + size_b: j - size_b,
                         qRgba(col.red(), col.green(), col.blue(), col.alpha()));
        }
    }

    this->image_label->setPixmap(QPixmap::fromImage(ret));
}

void WorkingArea::EnhancementsMean()
{
    int width = this->image->width();
    int height = this->image->height();

    QImage ret = *this->image;
    QColor col[9];
    int red, green, blue, alpha;

    for(int i = 1; i < width - 1; i++)  // 边界不处理
    {
        for(int j = 1; j < height - 1; j++)
        {
            col[0] = QColor(this->image->pixel(i, j));
            col[1] = QColor(this->image->pixel(i, j - 1));
            col[2] = QColor(this->image->pixel(i, j + 1));
            col[3] = QColor(this->image->pixel(i + 1, j));
            col[4] = QColor(this->image->pixel(i - 1, j));
            col[5] = QColor(this->image->pixel(i - 1, j - 1));
            col[6] = QColor(this->image->pixel(i - 1, j + 1));
            col[7] = QColor(this->image->pixel(i + 1, j - 1));
            col[8] = QColor(this->image->pixel(i + 1, j + 1));

            red = 0;
            green = 0;
            blue = 0;
            alpha = 0;

            for(int k = 0; k < 9; k++)
            {
                red += col[k].red();
                green += col[k].green();
                blue += col[k].blue();
                alpha += col[k].alpha();
            }

            red = red / 9;
            green = green / 9;
            blue = blue / 9;
            alpha = alpha / 9;

            ret.setPixel(i, j, qRgba(red, green, blue, alpha));
        }
    }

    this->image_label->setPixmap(QPixmap::fromImage(ret));
}

void WorkingArea::EnhancementsMedian()
{
    int width = this->image->width();
    int height = this->image->height();

    QImage ret = *this->image;
    QColor col[9];
    int red[9], green[9], blue[9], alpha[9];

    for(int i = 1; i < width - 1; i++)  // 边界不处理
    {
        for(int j = 1; j < height - 1; j++)
        {
            col[0] = QColor(this->image->pixel(i, j));
            col[1] = QColor(this->image->pixel(i, j - 1));
            col[2] = QColor(this->image->pixel(i, j + 1));
            col[3] = QColor(this->image->pixel(i + 1, j));
            col[4] = QColor(this->image->pixel(i - 1, j));
            col[5] = QColor(this->image->pixel(i - 1, j - 1));
            col[6] = QColor(this->image->pixel(i - 1, j + 1));
            col[7] = QColor(this->image->pixel(i + 1, j - 1));
            col[8] = QColor(this->image->pixel(i + 1, j + 1));

            for(int k = 0; k < 9; k++)
            {
                red[k] = col[k].red();
                green[k] = col[k].green();
                blue[k] = col[k].blue();
                alpha[k] = col[k].alpha();
            }

            std::sort(red, red + 9);
            std::sort(green, green + 9);
            std::sort(blue, blue + 9);
            std::sort(alpha, alpha + 9);

            ret.setPixel(i, j, qRgba(red[4], green[4], blue[4], alpha[4]));
        }
    }

    this->image_label->setPixmap(QPixmap::fromImage(ret));
}

void WorkingArea::EnhancementsKNeighborhood()
{
    int width = this->image->width();
    int height = this->image->height();

    QImage ret = *this->image;
    QColor col[9];
    int red[9], green[9], blue[9], alpha[9];
    int red_temp, green_temp, blue_temp, alpha_temp;
    int red2, green2, blue2, alpha2;

    for(int i = 1; i < width - 1; i++)  // 边界不处理
    {
        for(int j = 1; j < height - 1; j++)
        {
            col[0] = QColor(this->image->pixel(i, j));
            col[1] = QColor(this->image->pixel(i, j - 1));
            col[2] = QColor(this->image->pixel(i, j + 1));
            col[3] = QColor(this->image->pixel(i + 1, j));
            col[4] = QColor(this->image->pixel(i - 1, j));
            col[5] = QColor(this->image->pixel(i - 1, j - 1));
            col[6] = QColor(this->image->pixel(i - 1, j + 1));
            col[7] = QColor(this->image->pixel(i + 1, j - 1));
            col[8] = QColor(this->image->pixel(i + 1, j + 1));

            for(int k = 0; k < 9; k++)
            {
                red[k] = col[k].red();
                green[k] = col[k].green();
                blue[k] = col[k].blue();
                alpha[k] = col[k].alpha();
            }

            red_temp = fabs(red[1] - red[0]);
            green_temp = fabs(green[1] - green[0]);
            blue_temp = fabs(blue[1] - blue[0]);
            alpha_temp = fabs(alpha[1] - alpha[0]);

            red2 = green2 = blue2 = alpha2 = 1;

            for(int k = 2; k < 9; k++)
            {
                if(fabs(red[k] - red[0]) < red_temp)
                {
                    red2 = k;
                    red_temp = fabs(red[k] - red[0]);
                }
                if(fabs(green[k] - green[0]) < green_temp)
                {
                    green2 = k;
                    green_temp = fabs(green[k] - green[0]);
                }
                if(fabs(blue[k] - blue[0]) < blue_temp)
                {
                    blue2 = k;
                    blue_temp = fabs(blue[k] - blue[0]);
                }
                if(fabs(alpha[k] - alpha[0]) < alpha_temp)
                {
                    alpha2 = k;
                    alpha_temp = fabs(alpha[k] - alpha[0]);
                }
            }


            ret.setPixel(i, j, qRgba(red[red2], green[green2], blue[blue2], alpha[alpha2]));
        }
    }

    this->image_label->setPixmap(QPixmap::fromImage(ret));
}

struct VarianceArray
{
    float variance;
    int gray;
    int template_id;
};

bool VarianceCmp(const VarianceArray &a, const VarianceArray &b)
{
    return a.variance < b.variance;
}

void WorkingArea::EnhancementsMinimumMeanVariance()
{
    int width = this->image->width();
    int height = this->image->height();

    QImage ret = *this->image;
    QColor template_a[8][7], template_b[9];  // 前8个模板是7个数，最后一个模板是9个数

    VarianceArray temp_red[9], temp_green[9], temp_blue[9], temp_alpha[9];

    for(int i = 2; i < width - 2; i++)  // 忽略边缘
    {
        for(int j = 2; j < height - 2; j++)
        {
            template_a[0][0] = QColor(this->image->pixel(i, j));
            template_a[0][1] = QColor(this->image->pixel(i - 1, j));
            template_a[0][2] = QColor(this->image->pixel(i - 1, j - 1));
            template_a[0][3] = QColor(this->image->pixel(i - 1, j + 1));
            template_a[0][4] = QColor(this->image->pixel(i - 2, j));
            template_a[0][5] = QColor(this->image->pixel(i - 2, j - 1));
            template_a[0][6] = QColor(this->image->pixel(i - 2, j + 1));

            template_a[1][0] = QColor(this->image->pixel(i, j));
            template_a[1][1] = QColor(this->image->pixel(i, j - 1));
            template_a[1][2] = QColor(this->image->pixel(i - 1, j - 1));
            template_a[1][3] = QColor(this->image->pixel(i + 1, j - 1));
            template_a[1][4] = QColor(this->image->pixel(i, j - 2));
            template_a[1][5] = QColor(this->image->pixel(i - 1, j - 2));
            template_a[1][6] = QColor(this->image->pixel(i + 1, j - 2));

            template_a[2][0] = QColor(this->image->pixel(i, j));
            template_a[2][1] = QColor(this->image->pixel(i + 1, j));
            template_a[2][2] = QColor(this->image->pixel(i + 1, j - 1));
            template_a[2][3] = QColor(this->image->pixel(i + 1, j + 1));
            template_a[2][4] = QColor(this->image->pixel(i + 2, j));
            template_a[2][5] = QColor(this->image->pixel(i + 2, j - 1));
            template_a[2][6] = QColor(this->image->pixel(i + 2, j + 1));

            template_a[3][0] = QColor(this->image->pixel(i, j));
            template_a[3][1] = QColor(this->image->pixel(i, j + 1));
            template_a[3][2] = QColor(this->image->pixel(i - 1, j + 1));
            template_a[3][3] = QColor(this->image->pixel(i + 1, j + 1));
            template_a[3][4] = QColor(this->image->pixel(i, j + 2));
            template_a[3][5] = QColor(this->image->pixel(i - 1, j + 2));
            template_a[3][6] = QColor(this->image->pixel(i + 1, j + 2));

            template_a[4][0] = QColor(this->image->pixel(i, j));
            template_a[4][1] = QColor(this->image->pixel(i - 1, j - 1));
            template_a[4][2] = QColor(this->image->pixel(i, j - 1));
            template_a[4][3] = QColor(this->image->pixel(i - 1, j));
            template_a[4][4] = QColor(this->image->pixel(i - 2, j - 2));
            template_a[4][5] = QColor(this->image->pixel(i - 1, j - 2));
            template_a[4][6] = QColor(this->image->pixel(i - 2, j - 1));

            template_a[5][0] = QColor(this->image->pixel(i, j));
            template_a[5][1] = QColor(this->image->pixel(i + 1, j - 1));
            template_a[5][2] = QColor(this->image->pixel(i, j - 1));
            template_a[5][3] = QColor(this->image->pixel(i + 1, j));
            template_a[5][4] = QColor(this->image->pixel(i + 2, j - 2));
            template_a[5][5] = QColor(this->image->pixel(i + 1, j - 2));
            template_a[5][6] = QColor(this->image->pixel(i + 2, j - 1));

            template_a[6][0] = QColor(this->image->pixel(i, j));
            template_a[6][1] = QColor(this->image->pixel(i + 1, j + 1));
            template_a[6][2] = QColor(this->image->pixel(i, j + 1));
            template_a[6][3] = QColor(this->image->pixel(i + 1, j));
            template_a[6][4] = QColor(this->image->pixel(i + 2, j + 2));
            template_a[6][5] = QColor(this->image->pixel(i + 1, j + 2));
            template_a[6][6] = QColor(this->image->pixel(i + 2, j + 1));

            template_a[7][0] = QColor(this->image->pixel(i, j));
            template_a[7][1] = QColor(this->image->pixel(i - 1, j + 1));
            template_a[7][2] = QColor(this->image->pixel(i, j + 1));
            template_a[7][3] = QColor(this->image->pixel(i - 1, j));
            template_a[7][4] = QColor(this->image->pixel(i - 2, j + 2));
            template_a[7][5] = QColor(this->image->pixel(i - 1, j + 2));
            template_a[7][6] = QColor(this->image->pixel(i - 2, j + 1));

            template_b[0] = QColor(this->image->pixel(i, j));
            template_b[1] = QColor(this->image->pixel(i - 1, j - 1));
            template_b[2] = QColor(this->image->pixel(i, j - 1));
            template_b[3] = QColor(this->image->pixel(i + 1, j - 1));
            template_b[4] = QColor(this->image->pixel(i + 1, j));
            template_b[5] = QColor(this->image->pixel(i + 1, j + 1));
            template_b[6] = QColor(this->image->pixel(i, j + 1));
            template_b[7] = QColor(this->image->pixel(i - 1, j + 1));
            template_b[8] = QColor(this->image->pixel(i - 1, j));

            for(int k = 0; k < 8; k++)
            {
                int sum_red = 0, sum_green = 0, sum_blue = 0, sum_alpha = 0;
                float ave_red = 0.0f, ave_green = 0.0f, ave_blue = 0.0f, ave_alpha = 0.0f;

                for(int z = 0; z < 7; z++)
                {
                    sum_red += template_a[k][z].red();
                    sum_green += template_a[k][z].green();
                    sum_blue += template_a[k][z].blue();
                    sum_alpha += template_a[k][z].alpha();
                }

                ave_red = float(sum_red/7);
                ave_green = float(sum_green/7);
                ave_blue = float(sum_blue/7);
                ave_alpha = float(sum_alpha/7);

                float s_red = 0.0f, s_green = 0.0f, s_blue = 0.0f, s_alpha = 0.0f;
                for(int z = 0; z < 7; z++)
                {
                    s_red += (template_a[k][z].red() - ave_red)*(template_a[k][z].red() - ave_red);
                    s_green += (template_a[k][z].green() - ave_green)*(template_a[k][z].green() - ave_green);
                    s_blue += (template_a[k][z].blue() - ave_blue)*(template_a[k][z].blue() - ave_blue);
                    s_alpha += (template_a[k][z].alpha() - ave_alpha)*(template_a[k][z].alpha() - ave_alpha);
                }

                temp_red[k].variance = s_red / 7;
                temp_red[k].gray = sum_red / 7;
                temp_red[k].template_id = k;
                temp_green[k].variance = s_green / 7;
                temp_green[k].gray = sum_green / 7;
                temp_green[k].template_id = k;
                temp_blue[k].variance = s_blue / 7;
                temp_blue[k].gray = sum_blue / 7;
                temp_blue[k].template_id = k;
                temp_alpha[k].variance = s_alpha / 7;
                temp_alpha[k].gray = sum_alpha / 7;
                temp_alpha[k].template_id = k;
            }

            {
                int sum_red = 0, sum_green = 0, sum_blue = 0, sum_alpha = 0;
                float ave_red = 0.0f, ave_green = 0.0f, ave_blue = 0.0f, ave_alpha = 0.0f;

                for(int z = 0; z < 9; z++)
                {
                    sum_red += template_b[z].red();
                    sum_green += template_b[z].green();
                    sum_blue += template_b[z].blue();
                    sum_alpha += template_b[z].alpha();
                }

                ave_red = float(sum_red/9);
                ave_green = float(sum_green/9);
                ave_blue = float(sum_blue/9);
                ave_alpha = float(sum_alpha/9);

                float s_red = 0.0f, s_green = 0.0f, s_blue = 0.0f, s_alpha = 0.0f;
                for(int z = 0; z < 9; z++)
                {
                    s_red += (template_b[z].red() - ave_red)*(template_b[z].red() - ave_red);
                    s_green += (template_b[z].green() - ave_green)*(template_b[z].green() - ave_green);
                    s_blue += (template_b[z].blue() - ave_blue)*(template_b[z].blue() - ave_blue);
                    s_alpha += (template_b[z].alpha() - ave_alpha)*(template_b[z].alpha() - ave_alpha);
                }

                temp_red[8].variance = s_red / 9;
                temp_red[8].gray = sum_red / 9;
                temp_red[8].template_id = 8;
                temp_green[8].variance = s_green / 9;
                temp_green[8].gray = sum_green / 9;
                temp_green[8].template_id = 8;
                temp_blue[8].variance = s_blue / 9;
                temp_blue[8].gray = sum_blue / 9;
                temp_blue[8].template_id = 8;
                temp_alpha[8].variance = s_alpha / 9;
                temp_alpha[8].gray = sum_alpha / 9;
                temp_alpha[8].template_id = 8;
            }

            std::sort(temp_red, temp_red + 8, VarianceCmp);
            std::sort(temp_green, temp_green + 8, VarianceCmp);
            std::sort(temp_blue, temp_blue + 8, VarianceCmp);
            std::sort(temp_alpha, temp_alpha + 8, VarianceCmp);

            ret.setPixel(i, j, qRgba(temp_red[0].gray, temp_green[0].gray, temp_blue[0].gray, temp_alpha[0].gray));
        }
    }

    this->image_label->setPixmap(QPixmap::fromImage(ret));
}

// 罗伯茨交叉边缘检测
void WorkingArea::EnhancementsRoberts()
{
    int width = this->image->width();
    int height = this->image->height();

    QImage ret = *this->image;

    QColor col[4];
    int red, green, blue, alpha;

    for(int i = 0; i < width - 1; i++)
    {
        for(int j = 0; j < height - 1; j++)
        {
            col[0] = QColor(this->image->pixel(i, j));
            col[1] = QColor(this->image->pixel(i + 1, j + 1));
            col[2] = QColor(this->image->pixel(i + 1, j));
            col[3] = QColor(this->image->pixel(i, j + 1));

            // 这里使用了近似公式 G = |Gx| + |Gy|, 原式为 G = sqrt(Gx^2 + Gy^2)
            red = fabs(col[0].red()-col[1].red()) + fabs(col[2].red()-col[3].red()) + col[0].red();
            green = fabs(col[0].green()-col[1].green()) + fabs(col[2].green()-col[3].green()) + col[0].green();
            blue = fabs(col[0].blue()-col[1].blue()) + fabs(col[2].blue()-col[3].blue()) + col[0].blue();
            alpha = fabs(col[0].alpha()-col[1].alpha()) + fabs(col[2].alpha()-col[3].alpha()) + col[0].alpha();

            if(red > 255)
                red = 255;
            if(green > 255)
                green = 255;
            if(blue > 255)
                blue = 255;
            if(alpha > 255)
                alpha = 255;

            ret.setPixel(i, j, qRgba(red, green, blue, alpha));
        }
    }

    this->image_label->setPixmap(QPixmap::fromImage(ret));
}

void WorkingArea::EnhancementsSobel()
{
    int width = this->image->width();
    int height = this->image->height();

    QImage ret = *this->image;
    QColor col[2][6];
    int red_x, red_y, green_x, green_y, blue_x, blue_y, alpha_x, alpha_y;
    int red, green, blue, alpha;

    for(int i = 1; i < width - 1; i++)
    {
        for(int j = 1; j < height - 1; j++)
        {
            col[0][0] = QColor(this->image->pixel(i - 1, j - 1));
            col[0][1] = QColor(this->image->pixel(i + 1, j - 1));
            col[0][2] = QColor(this->image->pixel(i - 1, j));
            col[0][3] = QColor(this->image->pixel(i + 1, j));
            col[0][4] = QColor(this->image->pixel(i - 1, j + 1));
            col[0][5] = QColor(this->image->pixel(i + 1, j + 1));

            col[1][0] = QColor(this->image->pixel(i - 1, j - 1));
            col[1][1] = QColor(this->image->pixel(i, j - 1));
            col[1][2] = QColor(this->image->pixel(i + 1, j - 1));
            col[1][3] = QColor(this->image->pixel(i - 1, j + 1));
            col[1][4] = QColor(this->image->pixel(i, j + 1));
            col[1][5] = QColor(this->image->pixel(i + 1, j + 1));

            red_x = fabs(col[0][0].red()-col[0][1].red()+2*col[0][2].red()-2*col[0][3].red()+col[0][4].red()-col[0][5].red());
            green_x = fabs(col[0][0].green()-col[0][1].green()+2*col[0][2].green()-2*col[0][3].green()+col[0][4].green()-col[0][5].green());
            blue_x = fabs(col[0][0].blue()-col[0][1].blue()+2*col[0][2].blue()-2*col[0][3].blue()+col[0][4].blue()-col[0][5].blue());
            alpha_x = fabs(col[0][0].alpha()-col[0][1].alpha()+2*col[0][2].alpha()-2*col[0][3].alpha()+col[0][4].alpha()-col[0][5].alpha());

            red_y = fabs(-col[1][0].red()-2*col[1][1].red()-col[1][2].red()+col[1][3].red()+2*col[1][4].red()+col[1][5].red());
            green_y = fabs(-col[1][0].green()-2*col[1][1].green()-col[1][2].green()+col[1][3].green()+2*col[1][4].green()+col[1][5].green());
            blue_y = fabs(-col[1][0].blue()-2*col[1][1].blue()-col[1][2].blue()+col[1][3].blue()+2*col[1][4].blue()+col[1][5].blue());
            alpha_y = fabs(-col[1][0].alpha()-2*col[1][1].alpha()-col[1][2].alpha()+col[1][3].alpha()+2*col[1][4].alpha()+col[1][5].alpha());

            // 这里使用了近似公式 G = |Gx| + |Gy|, 原式为 G = sqrt(Gx^2 + Gy^2)
            red = red_x + red_y + QColor(this->image->pixel(i, j)).red();
            green = green_x + green_y + QColor(this->image->pixel(i, j)).green();
            blue = blue_x + blue_y + QColor(this->image->pixel(i, j)).blue();
            alpha = alpha_x + alpha_y + QColor(this->image->pixel(i, j)).alpha();

            if(red > 255)
                red = 255;
            if(green > 255)
                green = 255;
            if(blue > 255)
                blue = 255;
            if(alpha > 255)
                alpha = 255;

            ret.setPixel(i, j, qRgba(red, green, blue, alpha));
        }

        this->image_label->setPixmap(QPixmap::fromImage(ret));
    }
}

void WorkingArea::EnhancementsLaplace()
{
    int image_width = this->image->width();
    int image_height = this->image->height();

    QImage ret = *this->image;
    QRgb rgb,rgb1,rgb2,rgb3,rgb4;

    int r_new, g_new, b_new, a_new;

    for(int i = 1; i < image_width - 1; i++)
    {
        for(int j = 1; j < image_height - 1; j++)
        {
            rgb = this->image->pixel(i, j);
            rgb1 = this->image->pixel(i + 1, j);
            rgb2 = this->image->pixel(i - 1, j);
            rgb3 = this->image->pixel(i, j + 1);
            rgb4 = this->image->pixel(i, j - 1);

            r_new = qRed(rgb) + 4*qRed(rgb)-qRed(rgb1)-qRed(rgb2)-qRed(rgb3)-qRed(rgb4);
            g_new = qGreen(rgb) + 4*qGreen(rgb)-qGreen(rgb1)-qGreen(rgb2)-qGreen(rgb3)-qGreen(rgb4);
            b_new = qBlue(rgb) +  4*qBlue(rgb)-qBlue(rgb1)-qBlue(rgb2)-qBlue(rgb3)-qBlue(rgb4);
            a_new = qAlpha(rgb) +  4*qAlpha(rgb)-qAlpha(rgb1)-qAlpha(rgb2)-qAlpha(rgb3)-qAlpha(rgb4);

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
            if(a_new > 255)
                a_new = 255;
            if(a_new < 0)
                a_new = 0;

            ret.setPixel(i, j, qRgba(r_new, g_new, b_new, a_new));
        }
    }

    this->image_label->setPixmap(QPixmap::fromImage(ret));
}

void WorkingArea::EnhancementsConvolution(EnhancementsConvolutionSetting *convolution_setting, int type)
{
    int width = this->image->width();
    int height = this->image->height();

    QImage ret = *this->image;

    if(type == 0)
    {
        QColor col[9];
        int red, green, blue, alpha;
        float val[3][3];
        for(int i = 0; i < 3; i++)
            for(int j = 0; j < 3; j++)
                val[i][j] = convolution_setting->conl33[i][j];

        for(int i = 1; i < width - 1; i++)
        {
            for(int j = 1; j <height - 1; j++)
            {
                col[0] = QColor(this->image->pixel(i - 1, j - 1));
                col[1] = QColor(this->image->pixel(i, j - 1));
                col[2] = QColor(this->image->pixel(i + 1, j - 1));
                col[3] = QColor(this->image->pixel(i - 1, j));
                col[4] = QColor(this->image->pixel(i, j));
                col[5] = QColor(this->image->pixel(i + 1, j));
                col[6] = QColor(this->image->pixel(i - 1, j + 1));
                col[7] = QColor(this->image->pixel(i, j + 1));
                col[8] = QColor(this->image->pixel(i + 1, j + 1));

                red = col[4].red();
                green = col[4].green();
                blue = col[4].blue();
                alpha = col[4].alpha();

                for(int x = 0; x < 3; x++)
                {
                    for(int y = 0; y < 3; y++)
                    {
                        red += float(val[x][y]*col[x*3+y].red());
                        green += float(val[x][y]*col[x*3+y].green());
                        blue += float(val[x][y]*col[x*3+y].blue());
                        alpha += float(val[x][y]*col[x*3+y].alpha());
                    }
                }

                if(red > 255)
                    red = 255;
                if(red < 0)
                    red = 0;
                if(green > 255)
                    green = 255;
                if(green < 0)
                    green = 0;
                if(blue > 255)
                    blue = 255;
                if(blue < 0)
                    blue = 0;
                if(alpha > 255)
                    alpha = 255;
                if(alpha < 0)
                    alpha = 0;

                ret.setPixel(i, j, qRgba(red, green, blue, alpha));
            }
        }
    }

    if(type == 1)
    {
        QColor col[25];
        int red, green, blue, alpha;
        float val[5][5];
        for(int i = 0; i < 5; i++)
            for(int j = 0; j < 5; j++)
                val[i][j] = convolution_setting->conl55[i][j];

        for(int i = 2; i < width - 2; i++)
        {
            for(int j = 2; j <height - 2; j++)
            {
                col[0] = QColor(this->image->pixel(i - 2, j - 2));
                col[1] = QColor(this->image->pixel(i - 1, j - 2));
                col[2] = QColor(this->image->pixel(i, j - 2));
                col[3] = QColor(this->image->pixel(i + 1, j - 2));
                col[4] = QColor(this->image->pixel(i + 2, j - 2));

                col[5] = QColor(this->image->pixel(i - 2, j - 1));
                col[6] = QColor(this->image->pixel(i - 1, j - 1));
                col[7] = QColor(this->image->pixel(i, j - 1));
                col[8] = QColor(this->image->pixel(i - 1, j - 1));
                col[9] = QColor(this->image->pixel(i + 2, j - 1));

                col[10] = QColor(this->image->pixel(i - 2, j));
                col[11] = QColor(this->image->pixel(i - 1, j));
                col[12] = QColor(this->image->pixel(i, j));
                col[13] = QColor(this->image->pixel(i - 1, j));
                col[14] = QColor(this->image->pixel(i + 2, j));

                col[15] = QColor(this->image->pixel(i - 2, j + 1));
                col[16] = QColor(this->image->pixel(i - 1, j + 1));
                col[17] = QColor(this->image->pixel(i, j + 1));
                col[18] = QColor(this->image->pixel(i - 1, j + 1));
                col[19] = QColor(this->image->pixel(i + 2, j + 1));

                col[20] = QColor(this->image->pixel(i - 2, j + 2));
                col[21] = QColor(this->image->pixel(i - 1, j + 2));
                col[22] = QColor(this->image->pixel(i, j + 2));
                col[23] = QColor(this->image->pixel(i - 1, j + 2));
                col[24] = QColor(this->image->pixel(i + 2, j + 2));

                red = col[12].red();
                green = col[12].green();
                blue = col[12].blue();
                alpha = col[12].alpha();

                for(int x = 0; x < 5; x++)
                {
                    for(int y = 0; y < 5; y++)
                    {
                        red += float(val[x][y]*col[x*5+y].red());
                        green += float(val[x][y]*col[x*5+y].green());
                        blue += float(val[x][y]*col[x*5+y].blue());
                        alpha += float(val[x][y]*col[x*5+y].alpha());
                    }
                }

                if(red > 255)
                    red = 255;
                if(red < 0)
                    red = 0;
                if(green > 255)
                    green = 255;
                if(green < 0)
                    green = 0;
                if(blue > 255)
                    blue = 255;
                if(blue < 0)
                    blue = 0;
                if(alpha > 255)
                    alpha = 255;
                if(alpha < 0)
                    alpha = 0;

                ret.setPixel(i, j, qRgba(red, green, blue, alpha));
            }

        }
    }
    this->image_label->setPixmap(QPixmap::fromImage(ret));
}

void WorkingArea::EdgeLaplace()
{
    int image_width = this->image->width();
    int image_height = this->image->height();

    QImage ret = *this->image;
    int rgb,rgb1,rgb2,rgb3,rgb4;

    int gray_new;

    for(int i = 0; i < image_width; i++)
    {
        for(int j = 0; j < image_height; j++)
        {
            if(i + 1 >= image_width || j + 1 >= image_height
                || i - 1 < 0 || j - 1 < 0)
                continue;

            rgb = qGray(this->image->pixel(i, j));
            rgb1 = qGray(this->image->pixel(i + 1, j));
            rgb2 = qGray(this->image->pixel(i - 1, j));
            rgb3 = qGray(this->image->pixel(i, j + 1));
            rgb4 = qGray(this->image->pixel(i, j - 1));

            gray_new = 4*rgb-rgb1-rgb2-rgb3-rgb4;

            if(gray_new > 255)
                gray_new = 255;
            if(gray_new < 0)
                gray_new = 0;

            ret.setPixel(i, j, qRgb(gray_new, gray_new, gray_new));
        }
    }

    this->image_label->setPixmap(QPixmap::fromImage(ret));
}

//struct KirschTemp
//{
//    int val;
//    int id;
//};

//bool KirschCmp(const KirschTemp &a, const KirschTemp &b)
//{
//    return a.val < b.val;
//}

void WorkingArea::EdgeKirsch()
{
    // 3x3 Kirsch算子的八方向模板
    int kirsch_dir[8][9] = {
        {-5,3,3,-5,0,3,-5,3,3},
        {3,3,3,-5,0,3,-5,-5,3},
        {3,3,3,3,0,3,-5,-5,-5},
        {3,3,3,3,0,-5,3,-5,-5},
        {3,3,-5,3,0,-5,3,3,-5},
        {3,-5,-5,3,0,-5,3,3,3},
        {-5,-5,-5,3,0,3,3,3,3},
        {-5,-5,3,-5,0,3,3,3,3}
    };

    int dir_xy[9][2] = {
        {-1,-1},{0,-1},{1,-1},
        {-1,0},{0,0},{1,0},
        {1,-1},{0,1},{1,1}
    };

    int width = this->image->width();
    int height = this->image->height();

    QImage ret = *this->image;

    int gray[8];

    for(int i = 1; i < width - 1; i++)
    {
        for(int j = 1; j < height - 1; j++)
        {
            for(int k = 0; k < 8; k++)
            {
                gray[k] = 0;

                for(int p = 0; p < 9; p++)
                {
                    gray[k] += kirsch_dir[k][p]*qGray(this->image->pixel(i + dir_xy[p][0], j + dir_xy[p][1]));
                }
            }

            std::sort(gray, gray + 8);

            if(gray[7] > 255)
                gray[7] = 255;
            if(gray[7] < 0)
                gray[7] = 0;

            ret.setPixel(i, j, qRgb(gray[7], gray[7], gray[7]));
        }
    }

    this->image_label->setPixmap(QPixmap::fromImage(ret));
}

void WorkingArea::EdgeCanny()
{
    int width = this->image->width();
    int height = this->image->height();
    float *gray_image = new float[width*height];
    QColor col;

    for(int i = 0; i < width; i++)
    {
        for(int j = 0; j < height; j++)
        {
            col = QColor(this->image->pixel(i, j));
            gray_image[i*width+j] = 0.299*col.red() + 0.587*col.green() + 0.114*col.blue();
        }
    }
}

// 一种边缘连接方法，主要用于处理边缘断裂
/*
 * 对所有的端点（该点 8 邻域内有且只有一个前景点）查找其更外层的 16 个点，
 * 若有前景点，则将该端点和前景点之间的点也改为前景点
 * 边缘强化，增加像素值
*/
void WorkingArea::EdgeTracking()
{
    int width = this->image->width();
    int height = this->image->height();

    QImage ret = *this->image;

    for(int i = 2; i < width - 2; i++)
    {
        for(int j = 2; j < height - 2; j++)
        {
            // 如果该中心点为 255，则考虑它的 8 邻域
            if(qGray(this->image->pixel(i, j)) > 254)
            {
                int num = 0;
                for(int k = -1; k < 2; k++)
                {
                    for(int p = -1; p < 2; p++)
                    {
                        // 如果八邻域中有灰度值为 0 的点，则去找该点的 16 邻域
                        if(!(k == 0 && p == 0) &&
                            qGray(this->image->pixel(i + p, j + k)) > 254)
                            num++;
                    }
                }

                // 如果八邻域中只有一个点是255，说明该中心点为端点，则考虑他的 16 邻域
                if(num == 1)
                {
                    for(int k = -2; k < 3; k++)
                    {
                        for(int p = -2; p < 3; p++)
                        {
                            // 如果该点的 16 邻域中有 255 的点，则该点与中心点之间的点置为 255
                            if(!(k < 2 && k > -2 && p < 2 && p > -2)
                                && qGray(this->image->pixel(i + p, j + k)) > 254)
                                ret.setPixel(i + p/2, j + k/2, qRgb(255, 255, 255));
                        }
                    }
                }
            }

            // 边缘强化
            if(qGray(ret.pixel(i,j)) > 50 && qGray(ret.pixel(i,j)) < 128)
            {
                int gray = qGray(ret.pixel(i,j)) + 80;

                ret.setPixel(i, j,qRgb(gray, gray, gray));

                if(qGray(ret.pixel(i - 1, j)) < 50)
                    ret.setPixel(i - 1, j,qRgb(gray, gray, gray));
                if(qGray(ret.pixel(i + 1, j)) < 50)
                    ret.setPixel(i + 1, j,qRgb(gray, gray, gray));
                if(qGray(ret.pixel(i, j - 1)) < 50)
                    ret.setPixel(i, j - 1,qRgb(gray, gray, gray));
                if(qGray(ret.pixel(i, j + 1)) < 50)
                    ret.setPixel(i, j + 1,qRgb(gray, gray, gray));
            }
        }
    }

    this->image_label->setPixmap(QPixmap::fromImage(ret));
}

// 霍夫变换
/*
 * 1. 建立一个参数 (r,θ) 空间的二维的数组，该数组相当于一个累加器
 * 2. 顺序搜索图像中所有目标(黑色)像素，对于每一个目标像素，在参数空间中根据
 *    公式 r=x*cosθ+y*sinθ 找到对应位置，然后在累加器的对应位置加 1
 * 3. 统计参数空间 (r,θ) 对信息
 * 4. 通过参数空间位置，根据公式 r=x*cosθ+y*sinθ 找到图像空间中相对应的直线参数
*/
int r_theta[100000][180];  // (r,θ) 空间
void WorkingArea::EdgeHuogh()
{
    // 为避免霍夫变换程序中多次计算三角函数值，可以采用先用数组存储每个可能的三角函数值，
    // 再用查找表的方法查找对应三角函数值，从而提高效率
    float sin_val[180], cos_val[180];
    for(int i = -90; i < 90; i++)
    {
        sin_val[i + 90] = sin(i*3.1415926f/180);
        cos_val[i + 90] = cos(i*3.1415926f/180);
    }

    int count_record[180][1200];  // 统计每一对 (r,θ) 的出现次数
    memset(r_theta, 0, sizeof(r_theta));
    memset(count_record, 0, sizeof(count_record));

    int width = this->image->width();
    int height = this->image->height();

    QImage ret = *this->image;
    int dot_num = 0;

    for(int i = 0; i < width; i++)
    {
        for(int j = 0; j < height; j++)
        {
            if(qGray(this->image->pixel(i, j)) < 100)
                continue;

            for(int k = 0; k < 180; k++)  // 对 θ 的所有取值进行计算
                r_theta[dot_num][k] = (int)(i*cos_val[k] + j*sin_val[k]);

            dot_num++;
            if(dot_num >= 100000)
                goto out;
        }
    }

out:
    for(int i = 0; i < dot_num; i++)
    {
        for(int j = 0; j < 180; j++)
            count_record[j][r_theta[i][j] + 300]++;
    }

    for(int i = 0; i < 180; i++)
    {
        for(int j = 0; j < 1200; j++)
        {
            if(count_record[i][j] < 50)  // 阈值控制
                continue;

            // 遍历匹配
            for(int w = 0; w < width; w++)
            {
                for(int h = 0; h < height; h++)
                {
                    float abs_ans = fabs(w*cos_val[i] + h*sin_val[i] - j + 300);

                    if(abs_ans < 0.01)
                        ret.setPixel(w, h, qRgb(255,0,0));
                }
            }
        }
    }

    this->image_label->setPixmap(QPixmap::fromImage(ret));
}

void WorkingArea::CodeHuffmanC()
{
    int nr[256];
    memset(nr, 0, sizeof(nr));
    for(int i = 0; i < this->image->width(); i++)
    {
        for(int j = 0; j < this->image->height(); j++)
            nr[qGray(this->image->pixel(i, j))]++;
    }

    Huff *huf  = new Huff(256,nr);
    char **pbm,**pbm_o;
    pbm=huf->pbm;
    pbm_o=huf->pbm;
    QString suffix=tr("所有文件 (*.*)");

    QString fileName=QFileDialog::getOpenFileName(this,
                                                  QString(tr("打开(Open)")),
                                                  QDir::currentPath(),
                                                  suffix);

    QFile file(fileName);
    if(fileName.isNull() || fileName.isEmpty())
        return ;
    if(!file.open(QIODevice::ReadWrite | QIODevice::Text)) {
        qDebug("Can't open the file!");
    }

    QTextStream stream(&file);
    int geshiint,wint,hint;
    geshiint= this->image->format();
    stream << geshiint << endl;
    wint = this->image->width();
    stream << wint << endl;
    hint = this->image->height();
    stream << hint << endl;

    for(int i=0;i<256;i++)
    {
        stream << *pbm << endl;
        pbm++;
    }
    pbm=huf->pbm;

    for(int i = 0; i < this->image->width(); i++)
        for(int j = 0; j < this->image->height(); j++)
        {
            QString str(pbm[qGray(this->image->pixel(i, j))]);
            stream<< str << endl;
        }

    file.close();
}

void WorkingArea::CodeHuffmanD()
{
    QString suffix=tr("所有文件 (*.*)");
    QString fileName=QFileDialog::getOpenFileName(this,
                                                  QString(tr("打开(Open)")),
                                                  QDir::currentPath(),
                                                  suffix);
    QFile file(fileName);
    if(fileName.isNull()||fileName.isEmpty())
        return ;
    if(!file.open(QIODevice::ReadWrite | QIODevice::Text)) {
        qDebug("Can't open file");
    }

    QTextStream stream(&file);
    int geshiint,wint,hint;
    stream>>geshiint>>wint>>hint;
    QImage * Img = new QImage(wint,hint,(QImage::Format)geshiint);
    QString strl[257];
    for(int i=0;i<256;i++)
    {
        stream>> strl[i];
    }
    QColor rgb;
    for(int i=0;i<Img->width();i++)
        for(int ii=0;ii<Img->height();ii++)
        {
            QString str;
            stream>> str ;
            int B=1;
            int num=0;
            for(;num<256;num++)
            {
                if(str==strl[num])
                {
                    B=0;
                    break;
                }
            }
            if(B) qDebug("Error");
            rgb.setRgb(num,num,num);
            Img->setPixel(i,ii,rgb.rgb());
        }
    file.close();

    *this->image = *Img;
    this->image_label->setPixmap(QPixmap::fromImage(*Img));

    // 调整控件大小
    this->width_height = (double)this->image->width() / (double)this->image->height();
    this->AutoSize();  // 使用自适应大小模式
    this->auto_height = this->image_label->height();
    this->auto_width = this->image_label->width();
    this->is_saved = 0;
}

void WorkingArea::CodeRunsC()
{
    int nr[256];
    memset(nr, 0, sizeof(nr));

    int r,g,b;
    QColor col;
    QRgb val;
    for(int h=0;h< this->image->height();h++)///灰度
       for(int w=0;w< this->image->width();w++)
       {
           val = this->image->pixel(w,h);
           col.setRgb(val);
           col.getRgb(&r,&g,&b);
           int t=r>128?255:0;
           val = qRgb(t,t,t);
           this->image->setPixel(w,h,val);
           nr[qGray(val)]++;
       }

    QString suffix=tr("所有文件 (*.*)");

    QString fileName=QFileDialog::getOpenFileName(this,
                                                 QString(tr("打开(Open)")),
                                                 QDir::currentPath(),
                                                 suffix);
    QFile file(fileName);
    if(fileName.isNull()||fileName.isEmpty())
       return ;
    if(!file.open(QIODevice::ReadWrite | QIODevice::Text)) {
       qDebug("Can't open the file!");
    }
    QTextStream stream(&file);
    int geshiint,wint,hint;
    geshiint = this->image->format();
    stream << geshiint <<endl;
    wint = this->image->width();
    stream << wint <<endl;
    hint = this->image->height();
    stream << hint <<endl;

    QString y("1"),l("0"),v,t;
    int count=0;
    for(int i = 0; i < this->image->width(); i++)
       for(int ii = 0; ii < this->image->height(); ii++)
       {
           val = this->image->pixel(i,ii);
           col.setRgb(val);
           col.getRgb(&r,&g,&b);
           t = r>128?y:l;
           if(i==0 && ii==0) v=t;
           if(t==v)
           {
               count++;
           }
           else
           {
               stream<<v<<endl<<count<<endl;
               count=1;
               v=t;
           }
           if(i==(this->image->width()-1) && ii==(this->image->height()-1))
           {
               stream<<v<<endl<<count<<endl;
           }

       }

    file.close();
}

void WorkingArea::CodeRunsD()
{
    QString suffix=tr("所有文件 (*.*)");
    QString fileName=QFileDialog::getOpenFileName(this,
                                                  QString(tr("打开(Open)")),
                                                  QDir::currentPath(),
                                                  suffix);
    QFile file(fileName);
    if(fileName.isNull()||fileName.isEmpty())
        return ;
    if(!file.open(QIODevice::ReadWrite | QIODevice::Text)) {
        qDebug("Can't open the file!");
    }

    QTextStream stream(&file);
    int geshiint,wint,hint;
    stream>>geshiint>>wint>>hint;
    QImage *Img = new QImage(wint,hint,(QImage::Format)geshiint);
    int r,g;
    QColor rgb;
    QString v;
    int count=0;
    int i=0;
    int ii=0;
    while(1) {
        stream>>v;
        stream>>count;
        while(count--)
        {
            r=(v==QString("1"))?255:0;
            rgb.setRgb(r,r,r);
            Img->setPixel(i,ii,rgb.rgb());
            ii++;
            if(ii>=Img->height())
            {ii=0;i++;}
        }
        if(i>=Img->width()) break;
    }

    file.close();

    *this->image = *Img;
    this->image_label->setPixmap(QPixmap::fromImage(*Img));

    // 调整控件大小
    this->width_height = (double)this->image->width() / (double)this->image->height();
    this->AutoSize();  // 使用自适应大小模式
    this->auto_height = this->image_label->height();
    this->auto_width = this->image_label->width();
    this->is_saved = 0;
}
