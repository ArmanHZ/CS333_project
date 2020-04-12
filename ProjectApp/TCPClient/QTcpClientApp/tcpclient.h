#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#include <QMainWindow>
#include <QTcpSocket>
#include <QHostAddress>
#include <QString>
#include <QThread>
#include <iostream>

QT_BEGIN_NAMESPACE
namespace Ui { class TcpClient; }
QT_END_NAMESPACE

class TcpClient : public QMainWindow {
    Q_OBJECT

public:
    TcpClient(QWidget *parent = nullptr);
    ~TcpClient();

private:
    const int PORT = 5692;
    Ui::TcpClient *ui;
    QTcpSocket *socket;

    void connectToHost();
    void sendPublicKey();
    void sendMessage();

private slots:
    void connectButtonPressed();
    void sendButtonPressed();
    void onMessageReceived();
    void disconnectButtonPressed();
    void disconnected();
};
#endif // TCPCLIENT_H
