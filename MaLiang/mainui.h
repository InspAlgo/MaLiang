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

    void HistogramGray();
    void HistogramEqualization();

    void TransformLinear();
    void TransformNegativeFilm();
    void TransformLaplace();
    void TransformLog();
    void TransformGamma();

    void ToolChangeSize();
    void ChangeSize(int val);

    void SetThresholding(int val);

    void ReceiveLinearParameter(float linear_a, float linear_b);
    void ReceiveLaplaceParameter(float laplace_a, float laplace_b);
    void ReceiveLogParameter(float log_a, float log_b, float log_c);
    void ReceiveGammaParameter(float gamma_a, float gamma_b, float gamma_c);

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

    QMenu *menu_histogram;  // 直方图
    QAction *histogram_gray;  // 直方图-灰度直方图
    QAction *histogram_equalization;  // 直方图-均衡化

    QMenu *menu_transform;  // 变换
    QAction *transform_linear;  // 变换-线性变换
    QAction *transform_negative_film;  // 变换-负片
    QAction *transform_laplace;  // 变换-Laplace变换
    QAction *transform_log;  // 变换-log变换
    QAction *transform_gamma;  // 变换-Gamma变换

    QAction *tool_change_size;  // 改变 image_label 控件大小
    QLabel *change_widget;  // 控制 image_label 大小的控件
    QLabel *set_thresholding_widget;  // 设置二值化阈值的滑动条控件

    WorkingArea *working_area;  // 工作区，用来显示图片，也是编辑操作窗口
    ParameterSetting *parameter_setting;
};

#endif // MAINUI_H
