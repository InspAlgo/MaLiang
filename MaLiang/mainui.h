#ifndef MAINUI_H
#define MAINUI_H

#include <QMainWindow>
#include <QMenu>
#include <QMenuBar>
#include <QAction>
#include <QToolBar>

#include "workingarea.h"

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

    void ToolMagnify();
    void ToolReduce();

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

    QAction *tool_magnify;  // 工具-放大图片
    QAction *tool_reduce;  // 工具-缩小图片

    WorkingArea *working_area;  // 工作区，用来显示图片，也是编辑操作窗口
};

#endif // MAINUI_H
