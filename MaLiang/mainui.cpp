#include "mainui.h"
#include "ui_mainui.h"

#include <QMessageBox>
#include <QFileDialog>
#include <QImage>
#include <QSpinBox>
#include <QWidget>


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
    connect(this->file_create, SIGNAL(triggered(bool)), this, SLOT(FileCreate()));
    connect(this->file_open, SIGNAL(triggered(bool)), this, SLOT(FileOpen()));  // Action 应使用 triggered() 信号
    connect(this->file_save, SIGNAL(triggered(bool)), this, SLOT(FileSave()));
    connect(this->file_save_as, SIGNAL(triggered(bool)), this, SLOT(FileSaveAs()));
    connect(this->file_close, SIGNAL(triggered(bool)), this, SLOT(FileClose()));
    connect(this->file_close_all, SIGNAL(triggered(bool)), this, SLOT(FileCloseAll()));

    this->menu_pattern = this->menu->addMenu(tr("模式"));
    this->pattern_gray = this->menu_pattern->addAction(tr("灰度"));
    this->pattern_8bit_slice = this->menu_pattern->addAction(tr("8位切片"));
    this->pattern_select_thresholding = this->menu_pattern->addAction(tr("二值化"));
    this->pattern_floyd_steinberg = this->menu_pattern->addAction(tr("Floyd-Steinberg抖动"));
    this->pattern_to_txt = this->menu_pattern->addAction(tr("txt字符画"));
    this->pattern_24bit_to_256 = this->menu_pattern->addAction(tr("24位真彩图转256色"));
    connect(this->pattern_gray, SIGNAL(triggered(bool)), this, SLOT(PatternGray()));
    connect(this->pattern_8bit_slice, SIGNAL(triggered(bool)), this, SLOT(Pattern8BitSlice()));
    connect(this->pattern_select_thresholding, SIGNAL(triggered(bool)), this, SLOT(PatternSelectThresholding()));
    connect(this->pattern_floyd_steinberg, SIGNAL(triggered(bool)), this, SLOT(PatternFloydSteinberg()));
    connect(this->pattern_to_txt, SIGNAL(triggered(bool)), this, SLOT(PatternToTxt()));
    connect(this->pattern_24bit_to_256, SIGNAL(triggered(bool)), this, SLOT(Pattern24BitTo256()));

    this->menu_histogram = this->menu->addMenu(tr("直方图"));
    this->histogram_gray = this->menu_histogram->addAction(tr("灰度直方图"));
    this->histogram_equalization = this->menu_histogram->addAction(tr("均衡化"));
    connect(this->histogram_gray, SIGNAL(triggered(bool)), this, SLOT(HistogramGray()));
    connect(this->histogram_equalization, SIGNAL(triggered(bool)), this, SLOT(HistogramEqualization()));

    this->menu_transform = this->menu->addMenu(tr("变换"));
    this->transform_linear = this->menu_transform->addAction(tr("线性变换"));
    this->transform_negative_film = this->menu_transform->addAction(tr("负片"));
    this->transform_laplace = this->menu_transform->addAction(tr("拉普拉斯变换"));
    this->transform_log = this->menu_transform->addAction(tr("对数变换"));
    this->transform_gamma = this->menu_transform->addAction(tr("伽马变换"));
    connect(this->transform_linear, SIGNAL(triggered(bool)), this, SLOT(TransformLinear()));
    connect(this->transform_negative_film, SIGNAL(triggered(bool)), this, SLOT(TransformNegativeFilm()));
    connect(this->transform_laplace, SIGNAL(triggered(bool)), this, SLOT(TransformLaplace()));
    connect(this->transform_log, SIGNAL(triggered(bool)), this, SLOT(TransformLog()));
    connect(this->transform_gamma, SIGNAL(triggered(bool)), this, SLOT(TransformGamma()));

    this->menu_spatial_trans = this->menu->addMenu(tr("空间变换"));
    this->spatial_trans_nearest_neighbor = this->menu_spatial_trans->addAction(tr("最近邻插值缩放"));
    this->spatial_trans_bilinear_interpolation = this->menu_spatial_trans->addAction(tr("双线性插值缩放"));
    this->spatial_trans_rotating = this->menu_spatial_trans->addAction(tr("旋转"));
    this->spatial_trans_translaton = this->menu_spatial_trans->addAction(tr("平移"));
    connect(this->spatial_trans_nearest_neighbor, SIGNAL(triggered(bool)), this, SLOT(SpatialTransNN()));
    connect(this->spatial_trans_bilinear_interpolation, SIGNAL(triggered(bool)), this, SLOT(SpatialTransBI()));
    connect(this->spatial_trans_rotating, SIGNAL(triggered(bool)), this, SLOT(SpatialTransRotating()));
    connect(this->spatial_trans_translaton, SIGNAL(triggered(bool)), this, SLOT(SpatialTransTranslation()));

    this->menu_enhancements = this->menu->addMenu(tr("图像增强"));
    this->enhancements_mean = this->menu_enhancements->addAction(tr("均值平滑"));
    this->enhancements_median = this->menu_enhancements->addAction(tr("中值平滑"));
    this->enhancements_k_neighborhood = this->menu_enhancements->addAction(tr("K邻域平滑"));
    this->enhancements_minimum_mean_variance = this->menu_enhancements->addAction(tr("最小均方差平滑"));
    this->enhancements_roberts = this->menu_enhancements->addAction(tr("Roberts锐化"));
    this->enhancements_sobel = this->menu_enhancements->addAction(tr("Sobel锐化"));
    this->enhancements_laplace = this->menu_enhancements->addAction(tr("Laplace锐化"));
    this->enhancements_convolution = this->menu_enhancements->addAction(tr("自定义卷积模板"));
    connect(this->enhancements_mean, SIGNAL(triggered(bool)), this, SLOT(EnhancementsMean()));
    connect(this->enhancements_median, SIGNAL(triggered(bool)), this, SLOT(EnhancementsMedian()));
    connect(this->enhancements_k_neighborhood, SIGNAL(triggered(bool)), this, SLOT(EnhancementsKNeighborhood()));
    connect(this->enhancements_minimum_mean_variance, SIGNAL(triggered(bool)), this, SLOT(EnhancementsMinimumMeanVariance()));
    connect(this->enhancements_roberts, SIGNAL(triggered(bool)), this, SLOT(EnhancementsRoberts()));
    connect(this->enhancements_sobel, SIGNAL(triggered(bool)), this, SLOT(EnhancementsSobel()));
    connect(this->enhancements_laplace, SIGNAL(triggered(bool)), this, SLOT(EnhancementsLaplace()));
    connect(this->enhancements_convolution, SIGNAL(triggered(bool)), this, SLOT(EnhancementsConvolution()));

    this->menu_edge = this->menu->addMenu(tr("边缘检测"));
    this->edge_laplace = this->menu_edge->addAction(tr("Laplace算子"));
    this->edge_kirsch = this->menu_edge->addAction(tr("Kirsch方向算子"));
//    this->edge_canny = this->menu_edge->addAction(tr("Canny算子"));
    this->edge_tracking = this->menu_edge->addAction(tr("边缘跟踪"));
    this->edge_huogh = this->menu_edge->addAction(tr("霍夫变换检测直线"));
    connect(this->edge_laplace, SIGNAL(triggered(bool)), this, SLOT(EdgeLaplace()));
    connect(this->edge_kirsch, SIGNAL(triggered(bool)), this, SLOT(EdgeKirsch()));
//    connect(this->edge_canny, SIGNAL(triggered(bool)), this, SLOT(EdgeCanny()));
    connect(this->edge_tracking, SIGNAL(triggered(bool)), this, SLOT(EdgeTracking()));
    connect(this->edge_huogh, SIGNAL(triggered(bool)), this, SLOT(EdgeHuogh()));

    this->menu_code = this->menu->addMenu((tr("编码解码")));
    this->code_huffman_c = this->menu_code->addAction(tr("Huffman编码"));
    this->code_huffman_d = this->menu_code->addAction(tr("Huffman解码"));
    this->code_runs_c = this->menu_code->addAction(tr("游程长度编码"));
    this->code_runs_d = this->menu_code->addAction(tr("游程长度解码"));
    connect(this->code_huffman_c, SIGNAL(triggered(bool)), this, SLOT(CodeHuffmanC()));
    connect(this->code_huffman_d, SIGNAL(triggered(bool)), this, SLOT(CodeHuffmanD()));
    connect(this->code_runs_c, SIGNAL(triggered(bool)), this, SLOT(CodeRunsC()));
    connect(this->code_runs_d, SIGNAL(triggered(bool)), this, SLOT(CodeRunsD()));

    this->tool = addToolBar(tr("工具"));
    this->change_widget = NULL;
    this->tool_change_size = this->tool->addAction(QIcon(":/res/change_size1.png"), tr("调整大小"));
    connect(this->tool_change_size, SIGNAL(triggered()), this, SLOT(ToolChangeSize()));

    // 参数设置弹窗
    this->parameter_setting = new ParameterSetting(NULL);
    connect(this->parameter_setting, SIGNAL(SendLinearParameter(float,float)), this, SLOT(ReceiveLinearParameter(float,float)));
    connect(this->parameter_setting, SIGNAL(SendLaplaceParameter(float,float)), this, SLOT(ReceiveLaplaceParameter(float,float)));
    connect(this->parameter_setting, SIGNAL(SendLogParameter(float,float,float)), this, SLOT(ReceiveLogParameter(float,float,float)));
    connect(this->parameter_setting, SIGNAL(SendGammaParameter(float,float,float)), this, SLOT(ReceiveGammaParameter(float,float,float)));

    connect(this->parameter_setting, SIGNAL(SendSpatialTransNNParameter(float,float)), this, SLOT(ReceiveSpatialTransNNParameter(float,float)));
    connect(this->parameter_setting, SIGNAL(SendSpatialTransBIParameter(float,float)), this, SLOT(ReceiveSpatialTransBIParameter(float,float)));
    connect(this->parameter_setting, SIGNAL(SendSpatialTransRotatingParameter(float)), this, SLOT(ReceiveSpatialTransRotaingParameter(float)));
    connect(this->parameter_setting, SIGNAL(SendSpatialTransTranslationParameter(float,float)), this, SLOT(ReceiveSpatailTransTranslationParameter(float,float)));

    // 自定义卷积模板的弹窗
    this->convolution_setting = new EnhancementsConvolutionSetting(NULL);
    connect(this->convolution_setting, SIGNAL(SendConvolutionParameter(int)), this, SLOT(ReceiveConvolutionParameter(int)));

    this->thresholding_flag = 0;
}

MainUI::~MainUI()
{
    delete ui;
}

void MainUI::FileCreate()
{

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
    this->working_area->image_size = fsize;  // 传递图片大小

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

    this->working_area->image_type = image_type;
    // 导入图片的二进制流
    this->working_area->InitImage();

    // 工作区居中显示
    int working_area_x = (this->width() - this->working_area->width()) / 2;
    int working_area_y = (this->height() - this->menu->height() - this->tool->height() - this->working_area->height()) / 2;
    this->working_area->move(working_area_x, this->menu->height() + this->tool->height() + working_area_y);
    this->working_area->show();
}

void MainUI::FileSave()
{
    if(this->working_area == NULL)
        return;

    if(this->working_area->image_type != 4)
        this->working_area->SaveImage(this->working_area->image_path);
    else
        this->FileSaveAs();
}

void MainUI::FileSaveAs()
{
    if(this->working_area == NULL)
        return;

    QString save_path;
    save_path = QFileDialog::getSaveFileName(this,
        tr("Save Image"), "", tr("Image Files (*.bmp *.jpg *.png);;BMP(*.bmp);;JPG(*.jpg *jpeg);;PNG(*.png)"));

    if (!save_path.isNull() && !save_path.isEmpty())
    {
        QFileInfo file_info(save_path);
        QString file_type = file_info.suffix();

        if(file_type == "bmp")
            this->working_area->image_type = 1;
        else if(file_type == "jpg")
            this->working_area->image_type = 2;
        else if(file_type == "png")
            this->working_area->image_type = 3;
        else
            this->working_area->image_type = 1;

        this->working_area->SaveImage(save_path);
        this->working_area->image_path = save_path;
    }
}

void MainUI::FileClose()
{
    if(this->working_area == NULL)
        return;

    if(this->change_widget != NULL)
    {
        delete this->change_widget;
        this->change_widget = NULL;
    }

    if(this->working_area->is_saved != 0)
    {
        QMessageBox::about(NULL,tr("Warning"), tr("请先保存图片！"));
        return;
    }

    delete this->working_area;
    this->working_area = NULL;
}

void MainUI::FileCloseAll()
{
    // 遍历工作区列表，关闭所有打开的工作区
}

void MainUI::ToolChangeSize()
{
    if(this->working_area == NULL)
    {
        QMessageBox::about(NULL,tr("Error"), tr("尚未打开图片！"));
        return;
    }

    if(this->change_widget != NULL)
    {
        delete this->change_widget;
        this->change_widget = NULL;
        return;
    }

    QString slider_qss = "QSlider::groove:horizontal {"
                            "height: 6px;"
                            "background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 rgb(124, 124, 124), stop: 1.0 rgb(72, 71, 71));"
                          "}"
                           "QSlider::handle:horizontal {"
                                  "width: 10px;"
                                  "background: rgb(0, 160, 230);"
                                  "margin: -6px 0px -6px 0px;"
                                  "border-radius: 15px;"
                            "}";
    int nMin = 10;  // 最小值
    int nMax = 1000;  // 最大值
    int nSingleStep = 5;  // 步长

    this->change_widget = new QLabel(this);
    this->change_widget->setFixedSize(300, 30);  // 禁止改变窗口大小
    QSpinBox *pSpinBox = new QSpinBox(this->change_widget);
    pSpinBox->setMinimum(nMin);  // 最小值
    pSpinBox->setMaximum(nMax);  // 最大值
    pSpinBox->setSingleStep(nSingleStep);  // 步长
    pSpinBox->setSuffix("%");

    QSlider *pSlider = new QSlider(this->change_widget);
    pSlider->setOrientation(Qt::Horizontal);  // 水平方向
    pSlider->setMinimum(nMin);  // 最小值
    pSlider->setMaximum(nMax);  // 最大值
    pSlider->setSingleStep(nSingleStep);  // 步长

    connect(pSpinBox, SIGNAL(valueChanged(int)), pSlider, SLOT(setValue(int)));
    connect(pSlider, SIGNAL(valueChanged(int)), pSpinBox, SLOT(setValue(int)));
    connect(pSpinBox, SIGNAL(valueChanged(int)), this, SLOT(ChangeSize(int)));

    pSpinBox->setValue(100);
    pSlider->setStyleSheet(slider_qss);  // 加载样式

    pSpinBox->resize(80,30);
    pSpinBox->move(0, 0);
    pSlider->resize(200, 30);
    pSlider->move(90, 0);

    change_widget->move(0, this->menu->height() + this->tool->height());
    change_widget->show();
}

void MainUI::ChangeSize(int val)
{
    double scale = double(val) / double(100);
    this->working_area->ChangeLabelSize(scale);
}

void MainUI::PatternGray()
{
    if(this->working_area == NULL)
        return;

    this->working_area->RGB2Gray();
}

void MainUI::Pattern8BitSlice()
{
    if(this->working_area == NULL)
        return;

    this->working_area->Bit8Slice();
}

void MainUI::PatternSelectThresholding()
{
    if(this->working_area == NULL)
        return;

    if(this->thresholding_flag == 1)  // 二值化滑动条已开启
    {
        delete this->set_thresholding_widget;
        this->set_thresholding_widget = NULL;
        this->thresholding_flag = 0;
        return;
    }

    if(this->thresholding_flag == 0)  // 二值化滑动条未开启
    {
        QString slider_qss = "QSlider::groove:horizontal {"
                                "height: 6px;"
                                "background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 rgb(124, 124, 124), stop: 1.0 rgb(72, 71, 71));"
                              "}"
                               "QSlider::handle:horizontal {"
                                      "width: 10px;"
                                      "background: rgb(0, 160, 230);"
                                      "margin: -6px 0px -6px 0px;"
                                      "border-radius: 15px;"
                                "}";
        int nMin = 0;  // 最小值
        int nMax = 255;  // 最大值
        int nSingleStep = 1;  // 步长

        this->set_thresholding_widget = new QLabel(this);
        this->set_thresholding_widget->setFixedSize(300, 30);  // 禁止改变窗口大小
        QSpinBox *pSpinBox = new QSpinBox(this->set_thresholding_widget);
        pSpinBox->setMinimum(nMin);  // 最小值
        pSpinBox->setMaximum(nMax);  // 最大值
        pSpinBox->setSingleStep(nSingleStep);  // 步长

        QSlider *pSlider = new QSlider(this->set_thresholding_widget);
        pSlider->setOrientation(Qt::Horizontal);  // 水平方向
        pSlider->setMinimum(nMin);  // 最小值
        pSlider->setMaximum(nMax);  // 最大值
        pSlider->setSingleStep(nSingleStep);  // 步长

        connect(pSpinBox, SIGNAL(valueChanged(int)), pSlider, SLOT(setValue(int)));
        connect(pSlider, SIGNAL(valueChanged(int)), pSpinBox, SLOT(setValue(int)));
        connect(pSpinBox, SIGNAL(valueChanged(int)), this, SLOT(SetThresholding(int)));

        pSpinBox->setValue(128);
        pSlider->setStyleSheet(slider_qss);  // 加载样式

        pSpinBox->resize(80,30);
        pSpinBox->move(0, 0);
        pSlider->resize(200, 30);
        pSlider->move(90, 0);

        this->set_thresholding_widget->move(320, this->menu->height() + this->tool->height());
        this->set_thresholding_widget->show();
        this->thresholding_flag = 1;
    }
}

void MainUI::SetThresholding(int val)
{
    this->working_area->SelectThresholding(val);
}

void MainUI::PatternFloydSteinberg()
{
    if(this->working_area == NULL)
        return;

    this->working_area->FloydSteinberg();
}

void MainUI::PatternToTxt()
{
    if(this->working_area == NULL)
        return;

    this->working_area->ToTxt();
}

void MainUI::Pattern24BitTo256()
{
    if(this->working_area == NULL)
        return;

    this->working_area->RGB24bitTo256();
}

void MainUI::HistogramGray()
{
    if(this->working_area == NULL)
        return;

    this->working_area->GetGrayHistogram();
}

void MainUI::HistogramEqualization()
{
    if(this->working_area == NULL)
        return;

    this->working_area->HistogramEqualization();
}

void MainUI::ReceiveLinearParameter(float linear_a, float linear_b)
{
    this->working_area->TransformLinear(linear_a, linear_b);
}

void MainUI::TransformLinear()
{
    if(this->working_area == NULL)
        return;

    this->parameter_setting->LinearParameterInit();
    this->parameter_setting->show();
}

void MainUI::TransformNegativeFilm()
{
    if(this->working_area == NULL)
        return;

    this->working_area->TransformLinear(-1, 255);
}

void MainUI::ReceiveLaplaceParameter(float laplace_a, float laplace_b)
{
    laplace_a = 0.0f;
    laplace_b = 0.0f;
}

void MainUI::TransformLaplace()
{
    if(this->working_area == NULL)
        return;

    this->working_area->TransformLaplace(0,0);
}

void MainUI::ReceiveLogParameter(float log_a, float log_b, float log_c)
{
    this->working_area->TransformLog(log_a, log_b, log_c);
}

void MainUI::TransformLog()
{
    if(this->working_area == NULL)
        return;

    this->parameter_setting->LogParameterInit();
    this->parameter_setting->show();
}

void MainUI::ReceiveGammaParameter(float gamma_a, float gamma_b, float gamma_c)
{
    this->working_area->TransformGamma(gamma_a, gamma_b, gamma_c);
}

void MainUI::TransformGamma()
{
    if(this->working_area == NULL)
        return;

    this->parameter_setting->GammaParameterInit();
    this->parameter_setting->show();
}

void MainUI::ReceiveSpatialTransNNParameter(float size_a, float size_b)
{
    this->working_area->SpatialTransNN(size_a, size_b);
}

void MainUI::SpatialTransNN()
{
    if(this->working_area == NULL)
        return;

    this->parameter_setting->SpatialTransNNParameterInit();
    this->parameter_setting->show();
}

void MainUI::ReceiveSpatialTransBIParameter(float size_a, float size_b)
{
    this->working_area->SpatialTransBI(size_a, size_b);
}

void MainUI::SpatialTransBI()
{
    if(this->working_area == NULL)
        return;

    this->parameter_setting->SpatialTransBIParameterInit();
    this->parameter_setting->show();
}

void MainUI::ReceiveSpatialTransRotaingParameter(float size_a)
{
    this->working_area->SpatialTransRotaing(size_a);
}

void MainUI::SpatialTransRotating()
{
    if(this->working_area == NULL)
        return;

    this->parameter_setting->SpatailTransRotaingParameterInit();
    this->parameter_setting->show();
}

void MainUI::ReceiveSpatailTransTranslationParameter(float size_a, float size_b)
{
    this->working_area->SpatialTransTranslation(size_a, size_b);
}

void MainUI::SpatialTransTranslation()
{
    if(this->working_area == NULL)
        return;

    this->parameter_setting->SpatailTransTranslationParameterInit();
    this->parameter_setting->show();
}

void MainUI::EnhancementsMean()
{
    if(this->working_area == NULL)
        return;

    this->working_area->EnhancementsMean();
}

void MainUI::EnhancementsMedian()
{
    if(this->working_area == NULL)
        return;

    this->working_area->EnhancementsMedian();
}

void MainUI::EnhancementsKNeighborhood()
{
    if(this->working_area == NULL)
        return;

    this->working_area->EnhancementsKNeighborhood();
}

void MainUI::EnhancementsMinimumMeanVariance()
{
    if(this->working_area == NULL)
        return;

    this->working_area->EnhancementsMinimumMeanVariance();
}

void MainUI::EnhancementsRoberts()
{
    if(this->working_area == NULL)
        return;

    this->working_area->EnhancementsRoberts();
}

void MainUI::EnhancementsSobel()
{
    if(this->working_area == NULL)
        return;

    this->working_area->EnhancementsSobel();
}

void MainUI::EnhancementsLaplace()
{
    if(this->working_area == NULL)
        return;

    this->working_area->EnhancementsLaplace();
}

void MainUI::ReceiveConvolutionParameter(int type)
{
    if(type == 0)
        this->working_area->EnhancementsConvolution(this->convolution_setting, type);

    if(type == 1)
        this->working_area->EnhancementsConvolution(this->convolution_setting, type);
}

void MainUI::EnhancementsConvolution()
{
    if(this->working_area == NULL)
        return;

    this->convolution_setting->Init();
    this->convolution_setting->show();
}

void MainUI::EdgeLaplace()
{
    if(this->working_area == NULL)
        return;

    this->working_area->EdgeLaplace();
}

void MainUI::EdgeKirsch()
{
    if(this->working_area == NULL)
        return;

    this->working_area->EdgeKirsch();
}

void MainUI::EdgeCanny()
{
    if(this->working_area == NULL)
        return;
}

void MainUI::EdgeTracking()
{
    if(this->working_area == NULL)
        return;

    this->working_area->EdgeTracking();
}

void MainUI::EdgeHuogh()
{
    if(this->working_area == NULL)
        return;

    this->working_area->EdgeHuogh();
}

void MainUI::CodeHuffmanC()
{
    if(this->working_area == NULL)
        return;

    this->working_area->CodeHuffmanC();
}

void MainUI::CodeHuffmanD()
{
    if(this->working_area == NULL)
    {
        this->working_area = new WorkingArea(this);  // 创建工作区
        this->working_area->image_type = 4;  // 4 为解码显示类型
        // 工作区居中显示
        int working_area_x = (this->width() - this->working_area->width()) / 2;
        int working_area_y = (this->height() - this->menu->height() - this->tool->height() - this->working_area->height()) / 2;
        this->working_area->move(working_area_x, this->menu->height() + this->tool->height() + working_area_y);
        this->working_area->show();

        this->working_area->CodeHuffmanD();
    }
    else
    {
        this->working_area->image_type = 4;  // 4 为解码显示类型
        this->working_area->CodeHuffmanD();
    }
}

void MainUI::CodeRunsC()
{
    if(this->working_area == NULL)
        return;

    this->working_area->CodeRunsC();
}

void MainUI::CodeRunsD()
{
    if(this->working_area == NULL)
    {
        this->working_area = new WorkingArea(this);  // 创建工作区
        this->working_area->image_type = 4;  // 4 为解码显示类型
        // 工作区居中显示
        int working_area_x = (this->width() - this->working_area->width()) / 2;
        int working_area_y = (this->height() - this->menu->height() - this->tool->height() - this->working_area->height()) / 2;
        this->working_area->move(working_area_x, this->menu->height() + this->tool->height() + working_area_y);
        this->working_area->show();

        this->working_area->CodeRunsD();
    }
    else
    {
        this->working_area->image_type = 4;  // 4 为解码显示类型
        this->working_area->CodeRunsD();
    }
}

