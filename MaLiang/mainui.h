#ifndef MAINUI_H
#define MAINUI_H

#include <QMainWindow>
#include <QMenu>
#include <QMenuBar>
#include <QAction>
#include <QToolBar>
#include <QLabel>
#include <QSpinBox>
#include <QSlider>

#include "workingarea.h"
#include "parametersetting.h"
#include "enhancementsconvolutionsetting.h"

namespace Ui {
class MainUI;
}

class MainUI : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainUI(QWidget *parent = 0);
    ~MainUI();

public slots:
    void FileCreate();
    void FileOpen();
    void FileSave();
    void FileSaveAs();
    void FileClose();
    void FileCloseAll();

    void PatternGray();
    void Pattern8BitSlice();
    void PatternSelectThresholding();
    void PatternFloydSteinberg();
    void PatternToTxt();
    void Pattern24BitTo256();

    void HistogramGray();
    void HistogramEqualization();

    void TransformLinear();
    void TransformNegativeFilm();
    void TransformLaplace();
    void TransformLog();
    void TransformGamma();

    void SpatialTransNN();
    void SpatialTransBI();
    void SpatialTransRotating();
    void SpatialTransTranslation();

    void EnhancementsMean();
    void EnhancementsMedian();
    void EnhancementsKNeighborhood();
    void EnhancementsMinimumMeanVariance();
    void EnhancementsRoberts();
    void EnhancementsSobel();
    void EnhancementsLaplace();
    void EnhancementsConvolution();

    void EdgeLaplace();
    void EdgeKirsch();
    void EdgeCanny();
    void EdgeTracking();
    void EdgeHuogh();

    void CodeHuffmanC();
    void CodeHuffmanD();
    void CodeRunsC();
    void CodeRunsD();

    void ToolChangeSize();
    void ChangeSize(int val);

    void SetThresholding(int val);

    void ReceiveLinearParameter(float linear_a, float linear_b);
    void ReceiveLaplaceParameter(float laplace_a, float laplace_b);
    void ReceiveLogParameter(float log_a, float log_b, float log_c);
    void ReceiveGammaParameter(float gamma_a, float gamma_b, float gamma_c);

    void ReceiveSpatialTransNNParameter(float size_a, float size_b);
    void ReceiveSpatialTransBIParameter(float size_a, float size_b);
    void ReceiveSpatialTransRotaingParameter(float size_a);
    void ReceiveSpatailTransTranslationParameter(float size_a, float size_b);

    void ReceiveConvolutionParameter(int type);

private:


    Ui::MainUI *ui;

    QMenuBar *menu;  // 菜单栏
    QToolBar *tool;  // 工具栏

    QMenu *menu_file;  // 文件
    QAction *file_create;  // 文件-新建
    QAction *file_open;  // 文件-打开
    QAction *file_save;  // 文件-保存
    QAction *file_save_as;  // 文件-另存为
    QAction *file_close;  // 文件-关闭
    QAction *file_close_all;  // 文件-关闭所有

    QMenu *menu_pattern;  // 模式
    QAction *pattern_gray;  // 模式-灰度
    QAction *pattern_8bit_slice;  // 模式-切片
    QAction *pattern_select_thresholding;  // 模式-阈值选择二值化
    QAction *pattern_floyd_steinberg;  // 模式 - Floyd-Steinberg抖动
    QAction *pattern_to_txt;  // 模式-txt字符画
    QAction *pattern_24bit_to_256;  // 模式-24位真彩图转256色

    QMenu *menu_histogram;  // 直方图
    QAction *histogram_gray;  // 直方图-灰度直方图
    QAction *histogram_equalization;  // 直方图-均衡化

    QMenu *menu_transform;  // 变换
    QAction *transform_linear;  // 变换-线性变换
    QAction *transform_negative_film;  // 变换-负片
    QAction *transform_laplace;  // 变换-Laplace变换
    QAction *transform_log;  // 变换-log变换
    QAction *transform_gamma;  // 变换-Gamma变换

    QMenu *menu_spatial_trans;  // 空间变换
    QAction *spatial_trans_nearest_neighbor;  // 空间变换-最近邻插值缩放
    QAction *spatial_trans_bilinear_interpolation;  // 空间变换-双线性插值缩放
    QAction *spatial_trans_rotating;  // 空间变换-旋转
    QAction *spatial_trans_translaton;  // 空间变换-平移

    QMenu *menu_enhancements; // 图像增强
    QAction *enhancements_mean;  // 图像增强-均值平滑
    QAction *enhancements_median;  // 图像增强-中值平滑
    QAction *enhancements_k_neighborhood;  // 图像增强-K邻域平滑
    QAction *enhancements_minimum_mean_variance;  // 图像增强-最小均方差平滑
    QAction *enhancements_roberts;  // 图像增强-Roberts锐化
    QAction *enhancements_sobel;  // 图像增强-Sobel锐化
    QAction *enhancements_laplace;  // 图像增强-Laplace锐化
    QAction *enhancements_convolution;  // 图像增强-自定义卷积模板

    QMenu *menu_edge;  // 边缘检测
    QAction *edge_laplace;  // 边缘检测-Laplace算子
    QAction *edge_kirsch;  // 边缘检测-Kirsch方向算子
    QAction *edge_canny;  // 边缘检测-Canny算子
    QAction *edge_tracking;  // 边缘检测-边缘跟踪
    QAction *edge_huogh;  // 边缘检测-霍夫变换

    QMenu *menu_code;  // 编码解码
    QAction *code_huffman_c;  // Huffman编码
    QAction *code_huffman_d;  //  Huffman解码
    QAction *code_runs_c;  // 游程编码
    QAction *code_runs_d;  // 游程解码

    QAction *tool_change_size;  // 改变 image_label 控件大小
    QLabel *change_widget;  // 控制 image_label 大小的控件
    QLabel *set_thresholding_widget;  // 设置二值化阈值的滑动条控件

    WorkingArea *working_area;  // 工作区，用来显示图片，也是编辑操作窗口
    ParameterSetting *parameter_setting;
    EnhancementsConvolutionSetting *convolution_setting;

    int thresholding_flag;  // 模式-阈值选择二值化 的标志符；0-未初始化，1-启动，2-关闭
};

#endif // MAINUI_H
