#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <iostream>
#include <QMainWindow>
#include <QTcpServer>
#include <QTcpSocket>
#include <QString>

QT_BEGIN_NAMESPACE
namespace Ui { class TcpServer; }
QT_END_NAMESPACE

class TcpServer : public QMainWindow {
    Q_OBJECT

public:
    TcpServer(QWidget *parent = nullptr);
    ~TcpServer();

private:
    const int PORT = 5692;
    Ui::TcpServer *ui;
    QTcpServer *server;
    QTcpSocket *clientConnection;

    void readPublicKey();
    void sendMessage();
    void sendPublicKey();
    std::string sign(QString);
    QString encyptWithClientE(std::string);
    std::string decryptWithServerD(QString);
    QString unsign(std::string);

private slots:
    void startButtonPressed();
    void newConnection();
    void sendButtonPressed();
    void onMessageReceived();
    void clientDisconnected();

};
#endif // TCPSERVER_H
