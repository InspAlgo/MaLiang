#include "parametersetting.h"
#include "ui_parametersetting.h"

ParameterSetting::ParameterSetting(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ParameterSetting)
{
    ui->setupUi(this);
    this->setWindowTitle(tr("Parameter Setting"));
    this->type = 0;
    ui->label_formula->setText(tr(""));
    ui->lineEdit_parameter_a->setText(tr(""));
    ui->lineEdit_parameter_b->setText(tr(""));
    ui->lineEdit_parameter_c->setText(tr(""));
}

ParameterSetting::~ParameterSetting()
{
    delete ui;
}

void ParameterSetting::on_pushButton_parameter_OK_clicked()
{
    float parameter_a = ui->lineEdit_parameter_a->text().toFloat();
    float parameter_b = ui->lineEdit_parameter_b->text().toFloat();
    float parameter_c = ui->lineEdit_parameter_c->text().toFloat();

    switch (this->type)
    {
        case 1:
            emit SendLinearParameter(parameter_a, parameter_b);
            break;
        case 2:
            emit SendLaplaceParameter(parameter_a, parameter_b);
            break;
        case 3:
            emit SendLogParameter(parameter_a, parameter_b, parameter_c);
            break;
        case 4:
            emit SendGammaParameter(parameter_a, parameter_b, parameter_c);
            break;
        case 5:
            emit SendSpatialTransNNParameter(parameter_a, parameter_b);
            break;
        case 6:
            emit SendSpatialTransBIParameter(parameter_a, parameter_b);
            break;
        case 7:
            emit SendSpatialTransRotatingParameter(parameter_a);
            break;
        case 8:
            emit SendSpatialTransTranslationParameter(parameter_a, parameter_b);
            break;
        default:
            break;
    }
}

void ParameterSetting::InitAllParameterSetting()
{
    ui->label_formula->setText(tr(""));

    ui->label_parameter_a->setText(tr(""));
    ui->label_parameter_b->setText(tr(""));
    ui->label_parameter_c->setText(tr(""));
    ui->lineEdit_parameter_a->setText(tr(""));
    ui->lineEdit_parameter_b->setText(tr(""));
    ui->lineEdit_parameter_c->setText(tr(""));

    ui->label_parameter_a->setVisible(false);
    ui->label_parameter_b->setVisible(false);
    ui->label_parameter_c->setVisible(false);
    ui->lineEdit_parameter_a->setVisible(false);
    ui->lineEdit_parameter_b->setVisible(false);
    ui->lineEdit_parameter_c->setVisible(false);
}

void ParameterSetting::LinearParameterInit()
{
    this->type = 1;
    this->InitAllParameterSetting();

    ui->label_formula->setText(tr("y = a * x + b"));

    ui->label_parameter_a->setText(tr("斜率参数：a"));
    ui->label_parameter_b->setText(tr("截距参数：b"));
    ui->lineEdit_parameter_a->setText(tr("1"));
    ui->lineEdit_parameter_b->setText(tr("0"));

    ui->label_parameter_a->setVisible(true);
    ui->label_parameter_b->setVisible(true);
    ui->lineEdit_parameter_a->setVisible(true);
    ui->lineEdit_parameter_b->setVisible(true);
}

void ParameterSetting::LaplaceParameterInit()
{
    this->type = 2;
    this->InitAllParameterSetting();
}

void ParameterSetting::LogParameterInit()
{
    this->type = 3;
    this->InitAllParameterSetting();

    ui->label_formula->setText(tr("y = c * log(x + b) / log(a)"));

    ui->label_parameter_a->setText(tr("底数参数：a"));
    ui->label_parameter_b->setText(tr("参数：b"));
    ui->label_parameter_c->setText(tr("系数：c"));
    ui->lineEdit_parameter_a->setText(tr("1.05"));
    ui->lineEdit_parameter_b->setText(tr("0"));
    ui->lineEdit_parameter_c->setText(tr("1.0"));

    ui->label_parameter_a->setVisible(true);
    ui->label_parameter_b->setVisible(true);
    ui->label_parameter_c->setVisible(true);
    ui->lineEdit_parameter_a->setVisible(true);
    ui->lineEdit_parameter_b->setVisible(true);
    ui->lineEdit_parameter_c->setVisible(true);
}

void ParameterSetting::GammaParameterInit()
{
    this->type = 4;
    this->InitAllParameterSetting();

    ui->label_formula->setText(tr("y = c * (x + ε) ^ (γ)"));

    ui->label_parameter_a->setText(tr("参数：γ"));
    ui->label_parameter_b->setText(tr("偏移量：ε"));
    ui->label_parameter_c->setText(tr("系数：c"));
    ui->lineEdit_parameter_a->setText(tr("1"));
    ui->lineEdit_parameter_b->setText(tr("0"));
    ui->lineEdit_parameter_c->setText(tr("1"));

    ui->label_parameter_a->setVisible(true);
    ui->label_parameter_b->setVisible(true);
    ui->label_parameter_c->setVisible(true);
    ui->lineEdit_parameter_a->setVisible(true);
    ui->lineEdit_parameter_b->setVisible(true);
    ui->lineEdit_parameter_c->setVisible(true);
}

void ParameterSetting::SpatialTransNNParameterInit()
{
    this->type = 5;
    this->InitAllParameterSetting();

    ui->label_parameter_a->setText(tr("横宽缩放比例："));
    ui->label_parameter_b->setText(tr("竖高缩放比例："));
    ui->lineEdit_parameter_a->setText(tr("1"));
    ui->lineEdit_parameter_b->setText(tr("1"));

    ui->label_parameter_a->setVisible(true);
    ui->label_parameter_b->setVisible(true);
    ui->lineEdit_parameter_a->setVisible(true);
    ui->lineEdit_parameter_b->setVisible(true);
}

void ParameterSetting::SpatialTransBIParameterInit()
{
    this->type = 6;
    this->InitAllParameterSetting();

    ui->label_parameter_a->setText(tr("横宽缩放比例："));
    ui->label_parameter_b->setText(tr("竖高缩放比例："));
    ui->lineEdit_parameter_a->setText(tr("1"));
    ui->lineEdit_parameter_b->setText(tr("1"));

    ui->label_parameter_a->setVisible(true);
    ui->label_parameter_b->setVisible(true);
    ui->lineEdit_parameter_a->setVisible(true);
    ui->lineEdit_parameter_b->setVisible(true);
}

void ParameterSetting::SpatailTransRotaingParameterInit()
{
    this->type = 7;
    this->InitAllParameterSetting();

    ui->label_parameter_a->setText(tr("旋转度数："));
    ui->lineEdit_parameter_a->setText(tr("0"));

    ui->label_parameter_a->setVisible(true);
    ui->lineEdit_parameter_a->setVisible(true);
}

void ParameterSetting::SpatailTransTranslationParameterInit()
{
    this->type = 8;
    this->InitAllParameterSetting();

    ui->label_parameter_a->setText(tr("水平平移："));
    ui->label_parameter_b->setText(tr("垂直平移："));
    ui->lineEdit_parameter_a->setText(tr("0"));
    ui->lineEdit_parameter_b->setText(tr("0"));

    ui->label_parameter_a->setVisible(true);
    ui->label_parameter_b->setVisible(true);
    ui->lineEdit_parameter_a->setVisible(true);
    ui->lineEdit_parameter_b->setVisible(true);
}
