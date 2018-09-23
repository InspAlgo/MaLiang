#ifndef WORKINGAREA_H
#define WORKINGAREA_H

#include <QWidget>
#include <QString>
#include <QImage>
#include <QPixmap>

namespace Ui {
class WorkingArea;
}

class WorkingArea : public QWidget
{
    Q_OBJECT

public:
    explicit WorkingArea(QWidget *parent = 0);
    ~WorkingArea();

    enum PB_MODE {FIXED_SIZE, FIX_SIZE_CENTRED, AUTO_ZOOM, AUTO_SIZE};
    QString image_path;
    unsigned char *buffer;

    void setMode(PB_MODE mode);
    void changeScale(double change);

public slots:
    bool setImage(QImage &image, double scale = 1.0);
    void setBackground(QBrush brush);

protected:
        void paintEvent(QPaintEvent * event);
private:
    Ui::WorkingArea *ui;

    QPixmap m_pixmap;
    double m_scale;
    PB_MODE m_mode;
    QBrush m_brush;
};

#endif // WORKINGAREA_H
