#include "mainui.h"
#include <QtGui>
#include <QApplication>
#include <QTextCodec>
#include <QSplashScreen>
#include <QDesktopWidget>
#include <ActiveQt/QAxWidget>
#include <QTime>
#include <QLabel>
#include <QPixmap>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QPixmap pix("D:/GitWorkData/MaLiang/MaLiang/res/start.gif");
    QSplashScreen splash(pix);
    //splash.setWindowOpacity(0.5);
    QLabel label(&splash);
    QMovie mv("D:/GitWorkData/MaLiang/MaLiang/res/start.gif");
    label.setMovie(&mv);
    mv.start();
    splash.show();
    splash.setCursor(Qt::BlankCursor);
    for(int i = 0; i < 100; i += mv.speed())
    {
        QCoreApplication::processEvents();
        Sleep(mv.speed());
    }

    MainUI w;
    w.show();

    splash.finish(&w);
    return a.exec();
}
