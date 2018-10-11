#ifndef PARAMETERSETTING_H
#define PARAMETERSETTING_H

#include <QWidget>

namespace Ui {
class ParameterSetting;
}

class ParameterSetting : public QWidget
{
    Q_OBJECT

public:
    explicit ParameterSetting(QWidget *parent = 0);
    ~ParameterSetting();

    void LinearParameterInit();
    void LaplaceParameterInit();
    void LogParameterInit();
    void GammaParameterInit();

private slots:
    void on_pushButton_parameter_OK_clicked();

signals:
    void SendLinearParameter(float, float);
    void SendLaplaceParameter(float, float);
    void SendLogParameter(float, float, float);
    void SendGammaParameter(float, float, float);

private:
    void InitAllParameterSetting();

    Ui::ParameterSetting *ui;

    int type;
};

#endif // PARAMETERSETTING_H
