#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <iostream>
#include <QObject>
#include <QDebug>
#include <QTcpServer>
#include <QTcpSocket>
#include <QString>
#include <QByteArray>
#include <QTextStream>
#include "worker.h"
#include <QThread>

class TcpServer : public QObject {
    Q_OBJECT

public:
    explicit TcpServer(QObject *parent = nullptr);

private:
    QTcpServer *server;
    QTcpSocket *socket;
    Worker *worker;

    void writeToSocket(QByteArray);
    void initWorker();

private slots:
    void createNewConnection();
    void readDataFromClient();
};

#endif // TCPSERVER_H
