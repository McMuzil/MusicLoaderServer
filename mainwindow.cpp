#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    server = new QTcpServer(this);

    connect(server, SIGNAL(newConnection()),this, SLOT(newConnection()));

    if(!server->listen(QHostAddress::Any, 9999))
    {
        qDebug() << "Server could not start";
    }
    else
    {
        qDebug() << "Server Started!";
    }

}

void MainWindow::newConnection()
{
    socket = server->nextPendingConnection();
    connect(socket, SIGNAL(readyRead()),this,SLOT(readTcpData()));
    socket->write("Hello client\r\n");
    socket->flush();

    socket->waitForBytesWritten(3000);

}

MainWindow::~MainWindow()
{
    delete ui;
}



void MainWindow::on_pushButton_clicked()
{
    if(socket->state() == QTcpSocket::ConnectedState)
    {
        socket->write(ui->lineEdit->text().toLatin1());
        socket->flush();

        socket->waitForBytesWritten(3000);
    }
}

void MainWindow::readTcpData()
{
    QByteArray data = socket->readAll();
    QMessageBox* msg = new QMessageBox(this);
    msg->setText(data.data());
    msg->exec();
}
