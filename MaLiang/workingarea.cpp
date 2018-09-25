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
    if(this->is_saved == 0)
        return 0;

    this->image->save(save_path);
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
