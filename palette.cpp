#include "palette.h"
#include "mainwindow.h"
#include "ui_palette.h"

Palette::Palette(QWidget *parent, MainWindow *wnd) :
    QWidget(parent),
    ui(new Ui::Palette)
{
    ui->setupUi(this);
    connect(ui->Button,SIGNAL(clicked()),wnd->ui->actionButton,SLOT(trigger()));
    connect(ui->Gener,SIGNAL(clicked()),wnd->ui->actionOscillator,SLOT(trigger()));
    connect(ui->Indikator,SIGNAL(clicked()),wnd->ui->actionDigital_indicator,SLOT(trigger()));
    connect(ui->Lamp,SIGNAL(clicked()),wnd->ui->actionLED,SLOT(trigger()));
    connect(ui->Log,SIGNAL(clicked()),wnd->ui->actionLogic,SLOT(trigger()));
    connect(ui->Tren,SIGNAL(clicked()),wnd->ui->actionOutputs_extender,SLOT(trigger()));
    connect(ui->SEbat,SIGNAL(clicked()),wnd->ui->actionAdder,SLOT(trigger()));
    connect(ui->SRbat,SIGNAL(clicked()),wnd->ui->actionTrigger,SLOT(trigger()));
}

Palette::~Palette()
{
    delete ui;
}
