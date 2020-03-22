#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#include <QObject>
#include <QTcpSocket>
#include <QString>
#

class TcpClient : public QObject {
    Q_OBJECT
public:
    explicit TcpClient(QObject *parent = nullptr);

private slots:
    void clientConnect();

private:
    QTcpSocket *socket;

};

#endif // TCPCLIENT_H
