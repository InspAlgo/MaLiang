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
    int is_saved;
    unsigned char *buffer;
    unsigned long image_size;
    QString image_path;
    int image_type;
public slots:


private:
    Ui::WorkingArea *ui;

    QImage *image;
    QLabel *image_label;
    QScrollArea *scroll_area;
};

#endif // WORKINGAREA_H
