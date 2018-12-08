#include "enhancementsconvolutionsetting.h"
#include "ui_enhancementsconvolutionsetting.h"

#include <QMessageBox>

EnhancementsConvolutionSetting::EnhancementsConvolutionSetting(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::EnhancementsConvolutionSetting)
{
    ui->setupUi(this);

    this->setWindowTitle(tr("Convolution Setting"));

    this->Init();

    ui->stack->setCurrentIndex(0);
}

EnhancementsConvolutionSetting::~EnhancementsConvolutionSetting()
{
    delete ui;
}

void EnhancementsConvolutionSetting::Init()
{
    for(int i = 0; i < 9; i++)
    {
        this->line_edit_33[i] = findChild<QLineEdit *>("lineEdit33_" + QString::number(i + 1));
        this->line_edit_33[i]->setText(tr("0"));
    }

    for(int i = 0; i < 25; i++)
    {
        this->line_edit_55[i] = findChild<QLineEdit *>("lineEdit55_" + QString::number(i + 1));
        this->line_edit_55[i]->setText(tr("0"));
    }
}

void EnhancementsConvolutionSetting::on_pB_33_clicked()
{
     ui->stack->setCurrentIndex(0);
}

void EnhancementsConvolutionSetting::on_pB_55_clicked()
{
    ui->stack->setCurrentIndex(1);
}

void EnhancementsConvolutionSetting::on_pB_ok_clicked()
{
    int type = ui->stack->currentIndex();

    // 3x3
    if(type == 0)
    {
        for(int i = 0; i < 3; i++)
        {
            for(int j = 0; j < 3; j++)
            {
                this->conl33[i][j] = this->line_edit_33[i*3+j]->text().toFloat();
            }
        }

        emit SendConvolutionParameter(0);
        return;
    }

    // 5x5
    if(type == 1)
    {
        for(int i = 0; i < 5; i++)
        {
            for(int j = 0; j < 5; j++)
            {
                this->conl55[i][j] = this->line_edit_55[i*5+j]->text().toFloat();
            }
        }

        emit SendConvolutionParameter(1);
        return;
    }
}

float ** EnhancementsConvolutionSetting::Get33()
{
    return (float **)conl33;
}

float ** EnhancementsConvolutionSetting::Get55()
{
    return (float **)conl55;
}
