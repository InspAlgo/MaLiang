#ifndef WORKINGAREA_H
#define WORKINGAREA_H

#include <QWidget>
#include <QString>
#include <QImage>
#include <QPixmap>
#include <QLabel>
#include <QScrollArea>

namespace Ui {
class WorkingArea;
}

class WorkingArea : public QWidget
{
    Q_OBJECT

public:
    explicit WorkingArea(QWidget *parent = 0);
    ~WorkingArea();

    bool InitImage();
    int SaveImage(QString save_path);
    void ChangeLabelSize(double scale);

    int is_saved;  // 状态类型
    unsigned char *buffer;
    unsigned long image_size;
    QString image_path;
    int image_type;

public slots:


private:
    Ui::WorkingArea *ui;

    void AutoSize();  // 自适应大小

    QImage *image;  // 存显示的图片
    QLabel *image_label;  // 显示图片的用的载体控件
    QScrollArea *scroll_area;  // 滚动显示区，当图片过大时出现滚动条
    double width_height;  // 图片原始宽高比
    int auto_width;  // 自适应时的宽度
    int auto_height;  // 自适应时的高度
};

#endif // WORKINGAREA_H
