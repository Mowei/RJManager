#include "rjmanager.h"
#include "ui_rjmanager.h"

RJManager::RJManager(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::RJManager)
{
    ui->setupUi(this);
}

RJManager::~RJManager()
{
    delete ui;
}
