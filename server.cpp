#include "server.h"
#include "ui_mainwindow.h"

Server::Server()
{
    mainWindow = new MainWindow();

    server = new QTcpServer(mainWindow);

    QObject::connect(server, SIGNAL(newConnection()),this, SLOT(newConnection()));
    QObject::connect(mainWindow->ui->pushButton,SIGNAL(clicked(bool)), this, SLOT(sendMessage(bool)));

    if(!server->listen(QHostAddress::Any, 9999))
    {
        qDebug() << "Server could not start";
    }
    else
    {
        qDebug() << "Server Started!";
    }

}

Server::~Server()
{
    delete mainWindow;
    delete socket;
    delete server;
}

void Server::newConnection()
{
    socket = server->nextPendingConnection();
    QObject::connect(socket, SIGNAL(readyRead()),this,SLOT(readTcpData()));
    socket->write("Hello client\r\n");
    socket->flush();

    socket->waitForBytesWritten(3000);

}

void Server::readTcpData()
{
    QByteArray data = socket->readAll();
    QMessageBox msg(mainWindow);
    msg.setText(data.data());
    msg.exec();
}

void Server::sendMessage(bool value)
{
    if(socket == NULL) return;
    if(socket->state() == QTcpSocket::ConnectedState)
    {
        socket->write(mainWindow->getLineEditText().toLatin1());
        socket->flush();

        socket->waitForBytesWritten(3000);
    }
}

