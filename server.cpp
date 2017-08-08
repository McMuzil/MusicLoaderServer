#include "server.h"
#include "ui_mainwindow.h"
#include <QFileInfo>
#include <QTimer>
#include <windows.h>
#include <QSettings>

static const int PayloadSize = 64 * 1024; // 64 KB

Server::Server()
{
    mainWindow = new MainWindow();



    server = new QTcpServer(mainWindow);

    QObject::connect(server, SIGNAL(newConnection()),this, SLOT(NewConnection()));
    QObject::connect(mainWindow->ui->CheckoutSongs,SIGNAL(clicked(bool)), this, SLOT(CheckoutSongs(bool)));
    QObject::connect(mainWindow->ui->actionCheck_for_new_songs,SIGNAL(triggered(bool)),this,SLOT(CheckForNewSongs(bool)));
    QObject::connect(mainWindow->ui->actionSelect_music_location,SIGNAL(triggered(bool)),this,SLOT(SelectMusicLocation(bool)));
    if(!server->listen(QHostAddress::Any, 9999))
    {
        qDebug() << "Server could not start";
    }
    else
    {
        qDebug() << "Server Started!";
    }

    QSettings settings("Settings.ini", QSettings::IniFormat);
    settings.beginGroup("server");
    musicDirectory = settings.value("musicDirectory", "").toString();
    mainWindow->ui->directoryLabel->setText(musicDirectory);

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

    if(socket == NULL) return;

    CheckForNewSongs(value);

    qDebug() << "CheckoutSongs clicked";

    listIterator = songList.begin();

    StartTransmit(listIterator);
    //QTimer *timer = new QTimer;
    //timer->setSingleShot(true);
    //connect(timer, SIGNAL(timeout()), this,SLOT(timeout()));

    //timer->start(3000);


    //socket->waitForBytesWritten(1000);
    //StartTransmit("2.txt");
    //socket->waitForBytesWritten(1000);
    //StartTransmit("3.txt");
    //socket->waitForBytesWritten(1000);

}

void Server::StartTransmit(QList<QString>::iterator iter)
{
    if(iter == songList.end()) return;



    Init(*iter);



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
    qDebug() << fileInfo.fileName().toLatin1().data() << "Latin1";
    qDebug() << fileInfo.fileName().toUtf8() << "Latin1";
    qDebug() << fileInfo.fileName().toLocal8Bit().data() << "Latin1";
    socket->waitForBytesWritten(1000);
    qDebug() << fileInfo.fileName() << " file name";


    // starting transmiting song
    QObject::connect(socket,SIGNAL(bytesWritten(qint64)),this, SLOT(UpdateServerProgress(qint64)));

    bytesToWrite = totalBytes - (int)socket->write(file->read(qMin(bytesToWrite,PayloadSize)));
    qDebug() << "end of StartTransmit " << bytesToWrite;

}

void Server::Init(QString songName)
{
    bytesWrittenServer = 0;

    file = new QFile(songName);
    file->open(QIODevice::ReadOnly);
    bytesToWrite = totalBytes = file->size();
}

void Server::UpdateServerProgress(qint64 numBytes)
{
    bytesWrittenServer += (int)numBytes;


    mainWindow->ui->progressBar->setMaximum(totalBytes);
    mainWindow->ui->progressBar->setValue(bytesWrittenServer);
    mainWindow->ui->label->setText(tr("Sent %1MB").arg(bytesWrittenServer / (1024 * 1024)));

    if(bytesToWrite > 0 && socket->bytesToWrite() <= 4*PayloadSize)
    {
        bytesToWrite -= (int)socket->write(file->read(qMin(bytesToWrite,PayloadSize)));
    }
    else
    {
        Clean();

    }
}

void Server::Clean()
{

    file->close();
    QObject::disconnect(socket,SIGNAL(bytesWritten(qint64)),this, SLOT(UpdateServerProgress(qint64)));
    Sleep(1000);
    StartTransmit(++listIterator);
}


void Server::CheckForNewSongs(bool value)
{
    songList.clear();
    QDirIterator it(musicDirectory, QStringList() << "*.mp3", QDir::Files);
    while (it.hasNext())
    {
        QString songName = it.next();
        songList.append(songName);
        //qDebug() << songList.last();
        QFile file;
        file.setFileName(songName);
        //file.open(QIODevice::ReadOnly);
        QFileInfo fileInfo(file);

        qDebug() << fileInfo.fileName();
        //file.close();
    }
}

void Server::SelectMusicLocation(bool value)
{
    musicDirectory = QFileDialog::getExistingDirectory(mainWindow, tr("Open Directory"), "/home",QFileDialog::DontResolveSymlinks);
    if(musicDirectory != "")
    {
        mainWindow->ui->directoryLabel->setText(musicDirectory);
        QSettings settings("Settings.ini", QSettings::IniFormat);
        settings.beginGroup("server");
        settings.setValue("musicDirectory", musicDirectory);
        qDebug() << musicDirectory;
    }
}
