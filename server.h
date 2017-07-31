#ifndef SERVER_H
#define SERVER_H

#include <QTcpServer>
#include <QTcpSocket>
#include <QObject>
#include <QDebug>
#include <QObject>
#include <QMessageBox>
#include <mainwindow.h>

class Server : QObject
{
    Q_OBJECT
public:
    Server();
    virtual ~Server();

    MainWindow* mainWindow;
public slots:
    void newConnection();
    void readTcpData();
    void sendMessage(bool value);
private:
    QTcpServer* server = NULL;
    QTcpSocket* socket = NULL;

};

#endif // SERVER_H
