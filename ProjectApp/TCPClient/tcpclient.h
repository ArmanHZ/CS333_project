#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#include <QObject>
#include <QTcpSocket>
#include <QAbstractSocket>
#include <QString>
#include <QDebug>
#include <QHostAddress>
#include <QDataStream>

class TcpClient : public QObject {
    Q_OBJECT
public:
    explicit TcpClient(QObject *parent = nullptr);

public slots:
    void connectToHost();
    void readingData();

private:
    QTcpSocket *socket;

};

#endif // TCPCLIENT_H
