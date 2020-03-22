#include "tcpserver.h"

TcpServer::TcpServer(QObject *parent) : QObject(parent) {
    server = new QTcpServer(this);
    connect(server, SIGNAL(newConnection()), this, SLOT(createNewConnection()));

    if (!server->listen(QHostAddress::Any, 1234))
        qDebug() << " Error while starting the server!";
    else
        qDebug() << " Server started!";
}

void TcpServer::createNewConnection() {
    QTcpSocket *socket = server->nextPendingConnection();
    socket->write("Hello from the server\r\n");
    socket->flush();
    socket->waitForBytesWritten(3000);
    socket->close();
}

