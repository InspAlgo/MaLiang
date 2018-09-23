#include "workingarea.h"
#include "ui_workingarea.h"

#include <QPainter>
#include <QDebug>

static const int IMAGE_WIDTH = 400;
static const int IMAGE_HEIGHT = 300;
static const QSize IMAGE_SIZE = QSize(IMAGE_WIDTH, IMAGE_HEIGHT);

WorkingArea::WorkingArea(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WorkingArea)
{
    ui->setupUi(this);

    this->m_pixmap = QPixmap(IMAGE_SIZE);
    this->m_pixmap.fill();
    this->m_scale = 1.0;
    this->m_mode = FIXED_SIZE;
    this->m_brush = QBrush(Qt::white);
}

WorkingArea::~WorkingArea()
{
    delete[] this->buffer;
    delete ui;
}

void WorkingArea::setBackground(QBrush brush)
{
    this->m_brush = brush;
    update();
}

void WorkingArea::setMode(PB_MODE mode)
{
    this->m_mode = mode;
    if(this->m_mode == AUTO_SIZE)
        this->setFixedSize(this->m_pixmap.size() * this->m_scale);
    else
    {
        this->setMaximumSize(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX);
        this->setMinimumSize(0, 0);
    }
    update();
}

bool WorkingArea::setImage(QImage &image, double scale)
{
    if(image.isNull())
        return false;
    this->m_pixmap = QPixmap::fromImage(image);
    this->m_scale = qBound(0.01, scale, 100.0);
    if(this->m_mode == AUTO_SIZE)
        this->setFixedSize(this->m_pixmap.size() * this->m_scale);
    update();
    return true;
}

void WorkingArea::changeScale(double change)
{
    this->m_scale += change;

    if(this->m_scale <= 0)
        this->m_scale = 0;

    QPainter painter(this);
    painter.setBackground(this->m_brush);
    painter.eraseRect(rect());
    painter.scale(this->m_scale, this->m_scale);
    painter.drawPixmap(0, 0, this->m_pixmap);

    update();
}

void WorkingArea::paintEvent(QPaintEvent * event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setBackground(this->m_brush);
    painter.eraseRect(rect());

    double window_width, window_height;
    double image_width, image_height;
    double r1, r2, r;
    int offset_x, offset_y;

    switch (this->m_mode)
    {
        case FIXED_SIZE:
        case AUTO_SIZE:
            painter.scale(this->m_scale, this->m_scale);
            painter.drawPixmap(0, 0, this->m_pixmap);
            break;

        case FIX_SIZE_CENTRED:
            window_width = width();
            window_height = height();
            image_width = this->m_pixmap.width();
            image_height = this->m_pixmap.height();
            offset_x = (window_width - this->m_scale * image_width) / 2;
            offset_y = (window_height - this->m_scale * image_height) / 2;
            painter.translate(offset_x, offset_y);
            painter.scale(this->m_scale, this->m_scale);
            painter.drawPixmap(0, 0, this->m_pixmap);
            break;

        case AUTO_ZOOM:
            window_width = width();
            window_height = height();
            image_width = this->m_pixmap.width();
            image_height = this->m_pixmap.height();
            r1 = window_width / image_width;
            r2 = window_height / image_height;
            r = qMin(r1, r2);
            offset_x = (window_width - r * image_width) / 2;
            offset_y = (window_height - r * image_height) / 2;
            painter.translate(offset_x, offset_y);
            painter.scale(r, r);
            painter.drawPixmap(0, 0, this->m_pixmap);
            break;

        default:
            break;
    }
}
