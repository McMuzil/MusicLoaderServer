#include "server.h"
#include "ui_mainwindow.h"
#include <QFileInfo>

static const int PayloadSize = 64 * 1024; // 64 KB

Server::Server()
{
    mainWindow = new MainWindow();

    server = new QTcpServer(mainWindow);

    QObject::connect(server, SIGNAL(newConnection()),this, SLOT(NewConnection()));
    QObject::connect(mainWindow->ui->CheckoutSongs,SIGNAL(clicked(bool)), this, SLOT(CheckoutSongs(bool)));
    //QObject::connect(mainWindow->ui->pushButton_2, SIGNAL(clicked(bool)),this,SLOT(sendFile(bool)));




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

void Server::NewConnection()
{
    socket = server->nextPendingConnection();
    //QObject::connect(socket, SIGNAL(readyRead()),this,SLOT(UpdateServerProgress(qint64)));
    //QObject::connect(socket,SIGNAL(bytesWritten(qint64)), this,SLOT(updateProgress(qint64)));
    //server->close();
}

void Server::CheckoutSongs(bool value)
{
    // do some verifications
    // check songs
    // check connection
    qDebug() << "CheckoutSongs clicked";
    StartTransmit();
}

void Server::StartTransmit()
{
    Init();



    // sending command number

    QByteArray commandArray; // creating temporary array for sending info
    QDataStream streamCommandArray(&commandArray, QIODevice::WriteOnly);

    quint32 command = commandList::MusicTransmission;
    streamCommandArray << command;
    qDebug() << command << "command";
    qDebug() << sizeof(command) << "size of command";
    socket->write(commandArray);
    socket->waitForBytesWritten(1000);



    // sending size of File
    QByteArray totalBytesArray; // creating temporary array for sending info
    QDataStream streamTotalBytesArray(&totalBytesArray, QIODevice::WriteOnly);

    streamTotalBytesArray << totalBytes;
    socket->write(totalBytesArray);       //totalBytes
    socket->waitForBytesWritten(1000);
    qDebug() << totalBytes << "totalBytes";


    // sending size of file name
    QByteArray fileInfoArray; // creating temporary array for sending info
    QDataStream streamFileInfoArray(&fileInfoArray, QIODevice::WriteOnly);

    QFileInfo fileInfo(file->fileName());
    streamFileInfoArray << (quint32)fileInfo.fileName().size();
    socket->write(fileInfoArray);   //size of file name
    socket->waitForBytesWritten(1000);
    qDebug() << (quint32)fileInfo.fileName().size() << "size of fileName";

    // and file name
    socket->write(fileInfo.fileName().toLatin1());
    socket->waitForBytesWritten(1000);
    qDebug() << fileInfo.fileName() << " file name";


    // starting transmiting song
    QObject::connect(socket,SIGNAL(bytesWritten(qint64)),this, SLOT(UpdateServerProgress(qint64)));

    bytesToWrite = totalBytes - (int)socket->write(file->read(qMin(bytesToWrite,PayloadSize)));
    qDebug() << "end of StartTransmit";

}

void Server::Init()
{
    bytesWrittenServer = 0;

    file = new QFile("SeabedStation.png");
    file->open(QIODevice::ReadOnly);
    bytesToWrite = totalBytes = file->size();
}

void Server::UpdateServerProgress(qint64 numBytes)
{
    qDebug() << "pishem";
    bytesWrittenServer += (int)numBytes;

    if(bytesToWrite > 0 && socket->bytesToWrite() <= 4*PayloadSize)
    {
        bytesToWrite -= (int)socket->write(file->read(qMin(bytesToWrite,PayloadSize)));
    }
    else
    {
        Clean();
    }
    mainWindow->ui->progressBar->setMaximum(totalBytes);
    mainWindow->ui->progressBar->setValue(bytesWrittenServer);
    mainWindow->ui->label->setText(tr("Sent %1MB").arg(bytesWrittenServer / (1024 * 1024)));
}

void Server::Clean()
{
    file->close();
    QObject::disconnect(socket,SIGNAL(bytesWritten(qint64)),this, SLOT(UpdateServerProgress(qint64)));
}



void Server::SendFile(bool value)
{
    QFile file("SeabedStation.png");
    qDebug() <<  file.size();
    totalBytes = file.size();
    if(file.open(QIODevice::ReadOnly))
    {
        if(socket == NULL) return;
        if(socket->state() != QTcpSocket::ConnectedState) return;

        QByteArray rawFile;
        rawFile = file.readAll();
        bytesToWrite = totalBytes - (int)socket->write(rawFile);//,PayloadSize);
        qDebug() << "sending" << rawFile.size();
    }
    file.close();
}
/*
void Server::updateProgress(qint64 numBytes)
{
    bytesWritten += (int)numBytes;

    if(bytesToWrite > 0 && socket->bytesToWrite() <= 4*PayloadSize)
        bytesToWrite -= (int)socket->write(qMin(bytesToWrite, PayloadSize), )
}
*/
