#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#include <iostream>
#include <QObject>
#include <QTcpSocket>
#include <QAbstractSocket>
#include <QString>
#include <QDebug>
#include <QHostAddress>
#include <QDataStream>
#include <QThread>

class TcpClient : public QObject {
    Q_OBJECT
public:
    explicit TcpClient(QObject *parent = nullptr);

public slots:
    void readingData();

private:
    QTcpSocket *socket;
    void connectToHost();
};

#endif // TCPCLIENT_H
