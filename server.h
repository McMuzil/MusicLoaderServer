#ifndef SERVER_H
#define SERVER_H

#include <QTcpServer>
#include <QTcpSocket>
#include <QObject>
#include <QDebug>
#include <QObject>
#include <QMessageBox>
#include <mainwindow.h>
#include <QFile>
#include <QDirIterator>
#include <QList>
#include <QFileDialog>

class Server : QObject
{
    Q_OBJECT
public:
    Server();
    virtual ~Server();

    MainWindow* mainWindow;
public slots:
    void NewConnection();
    void UpdateServerProgress(qint64 numBytes);
    void CheckoutSongs(bool value);
    void CheckForNewSongs(bool value);
    void SelectMusicLocation(bool value);
    //void updateProgress(qint64 numBytes);
private:
    void StartTransmit(QList<QString>::iterator iter);
    void Init(QString songName);
    void Clean();

    QString musicDirectory;
    QList<QString>::iterator listIterator;
    QStringList songList;
    QFile* file;
    QTcpServer* server = NULL;
    QTcpSocket* socket = NULL;
    int bytesToWrite;
    quint32 totalBytes;
    int bytesWrittenServer;
    enum commandList {None,MusicTransmission};

};

#endif // SERVER_H
