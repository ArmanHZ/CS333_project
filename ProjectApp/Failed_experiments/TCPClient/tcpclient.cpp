#include "tcpclient.h"

TcpClient::TcpClient(QObject *parent) : QObject(parent) {
    socket = new QTcpSocket(this);
    connect(socket, SIGNAL(readyRead()), this, SLOT(readingData()));
    connectToHost();

    socket->write("test");
    socket->flush();
    socket->waitForBytesWritten();
    std::cout << "Written." << std::endl;
}

void TcpClient::connectToHost() {
    while (!socket->waitForConnected()) {
        std::cout << "Trying to establish connection to host..." << std::endl;
        socket->connectToHost(QHostAddress::LocalHost, 56234);
        QThread::sleep(3);
    }
    std::cout << "Connected to host!" << std::endl;
}

void TcpClient::readingData() {
    auto data = socket->readAll();
    std::cout << "[Host] <-- " << data.toStdString() << std::endl;
}
