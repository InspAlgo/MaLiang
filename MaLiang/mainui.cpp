#include "mainui.h"
#include "ui_mainui.h"

#include <QMessageBox>
#include <QFileDialog>
#include <QImage>

#include <iostream>
#include <string>

MainUI::MainUI(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainUI)
{
    ui->setupUi(this);
    this->setWindowTitle(tr("MALIANG"));   // 设置标题框文本

    this->working_area = NULL;

    this->menu = menuBar();  // 菜单栏

    this->menu_file = this->menu->addMenu(tr("文件"));  // 添加菜单
    this->file_create = this->menu_file->addAction(tr("新建"));  // 添加菜单项，添加动作
    this->file_open = this->menu_file->addAction(tr("打开"));
    this->file_save = this->menu_file->addAction(tr("保存"));
    this->file_save_as = this->menu_file->addAction(tr("另存为"));
    this->file_close = this->menu_file->addAction(tr("关闭"));
    this->file_close_all = this->menu_file->addAction(tr("关闭所有"));
    connect(this->file_open, SIGNAL(triggered()), this, SLOT(FileOpen()));  // Action 应使用 triggered() 信号
    connect(this->file_close, SIGNAL(triggered()), this, SLOT(FileClose()));

    this->tool = addToolBar(tr("工具"));
    this->tool_magnify = this->tool->addAction(tr("放大"));
    this->tool_reduce = this->tool->addAction(tr("缩小"));
    connect(this->tool_magnify, SIGNAL(triggered()), this, SLOT(ToolMagnify()));
    connect(this->tool_reduce, SIGNAL(triggered()), this, SLOT(ToolReduce()));
}

MainUI::~MainUI()
{
    delete ui;
}

void MainUI::FileOpen()
{
    if(this->working_area != NULL)
    {
        QMessageBox::about(NULL,tr("Error"),tr("已打开图片！"));
        return;
    }

    // 选择图片
    QString select_image = QFileDialog::getOpenFileName(
           this, tr("Open File"), "",
           tr("File (*.bmp *jpg *png)")
       );

    // 如果没有选择则直接返回
    if(select_image.isEmpty())
        return;

    // 注意 Qt 中使用 c++ 打开含中文路径文件时要使用 toLocal8Bit()
    FILE *open_image = fopen((char *)select_image.toLocal8Bit().data(), "rb");
    if (!open_image)
    {
        QMessageBox::about(NULL,tr("Error"),tr("打开图片错误！可能是非法路径！"));
        return;
    }

    fseek(open_image, 0, SEEK_END);

    // 成功则返回当前的读写位置，失败返回 -1；对于二进制文件，则返回从文件开头到结尾的字节数
    unsigned long fsize = ftell(open_image);

    rewind(open_image);

    this->working_area = new WorkingArea(this);  // 创建工作区
    this->working_area->image_path = select_image;  // 传递路径

    // 将图片文件导入到缓存
    this->working_area->buffer = new unsigned char[fsize];
    if (fread(this->working_area->buffer, 1, fsize, open_image) != fsize)
    {
        delete this->working_area;
        QMessageBox::about(NULL,tr("Error"),tr("不能读取图片！"));
        return;
    }

    fclose(open_image);

    int image_type = 0;
    if(this->working_area->buffer[0] == 0x42 && this->working_area->buffer[1] == 0x4D)  // bmp 图片
        image_type = 1;
    else if(this->working_area->buffer[0] == 0xFF && this->working_area->buffer[1] == 0xD8)  // jpg 图片
        image_type = 2;
    else if(this->working_area->buffer[0] == 0x89 && this->working_area->buffer[1] == 0x50  // png 图片
            && this->working_area->buffer[2] == 0x4E && this->working_area->buffer[3] == 0x47
            && this->working_area->buffer[4] == 0x0D && this->working_area->buffer[5] == 0x0A
            && this->working_area->buffer[6] == 0x1A && this->working_area->buffer[7] == 0x0A)
        image_type = 3;
    else
        image_type = 0;

    if(!image_type)
    {
        delete this->working_area;
        QMessageBox::about(NULL,tr("Error"),tr("这不是标准的 bmp/jpg/png 图片！"));
        return;
    }

    QImage image;
    image.loadFromData(this->working_area->buffer, fsize);  // 将图片文件的二进制流导入
    this->working_area->setMode(this->working_area->AUTO_ZOOM);
    this->working_area->setImage(image);

    this->working_area->move(50, 50);
    this->working_area->show();
}

void MainUI::FileClose()
{
    if(this->working_area != NULL)
    {
        delete this->working_area;
        this->working_area = NULL;
    }
}

void MainUI::ToolMagnify()
{
    if(this->working_area == NULL)
        return;

    QMessageBox::about(NULL,tr("Error"),tr("cc！"));
    this->working_area->changeScale(0.1);
}

void MainUI::ToolReduce()
{
    if(this->working_area == NULL)
        return;

    QMessageBox::about(NULL,tr("Error"),tr("cc！"));
    this->working_area->changeScale(-0.1);
}
