#ifndef ENHANCEMENTSCONVOLUTIONSETTING_H
#define ENHANCEMENTSCONVOLUTIONSETTING_H

#include <QDialog>
#include <QLineEdit>

namespace Ui {
class EnhancementsConvolutionSetting;
}

class EnhancementsConvolutionSetting : public QDialog
{
    Q_OBJECT

public:
    explicit EnhancementsConvolutionSetting(QWidget *parent = 0);
    ~EnhancementsConvolutionSetting();

    void Init();
    float ** Get33();
    float ** Get55();

    float conl33[3][3];
    float conl55[5][5];

private slots:
    void on_pB_33_clicked();
    void on_pB_55_clicked();
    void on_pB_ok_clicked();

signals:
    void SendConvolutionParameter(int);

private:
    Ui::EnhancementsConvolutionSetting *ui;

    QLineEdit *line_edit_33[9];


    QLineEdit *line_edit_55[25];

};

#endif // ENHANCEMENTSCONVOLUTIONSETTING_H
