#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    lineEdit = ui->lineEdit;
}


MainWindow::~MainWindow()
{
    delete ui;
    delete lineEdit;
}

QString MainWindow::getLineEditText()
{
    return ui->lineEdit->text();
}
