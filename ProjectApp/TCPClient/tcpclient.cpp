#include "tcpclient.h"

TcpClient::TcpClient(QObject *parent) : QObject(parent) {
    socket = new QTcpSocket(this);
    connect(socket, SIGNAL(connected()),this, SLOT(connectToHost()));
    connect(socket, SIGNAL(readyRead()), this, SLOT(readingData()));
    qDebug() << "Connecting to host...";
    socket->connectToHost(QHostAddress::LocalHost, 56234);
}

void TcpClient::connectToHost() {
    if (socket->waitForConnected())
        qDebug() << "Connected to host!";
    else
        qDebug() << "Connection failed!";
}

void TcpClient::readingData() {
    qDebug() << "Reading data...";
    auto data = socket->readAll();
    qDebug() << data;
}
