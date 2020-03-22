#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <QObject>
#include <QDebug>
#include <QTcpServer>
#include <QTcpSocket>

class TcpServer : public QObject {
    Q_OBJECT

public:
    explicit TcpServer(QObject *parent = nullptr);

private:
    QTcpServer *server;

private slots:
    void createNewConnection();
};

#endif // TCPSERVER_H
