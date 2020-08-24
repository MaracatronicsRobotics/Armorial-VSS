#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::updateDetection(VSSTeam *ourTeam, VSSTeam *theirTeam){
    ui->openGLWidget->updateDetection(ourTeam, theirTeam);
}
