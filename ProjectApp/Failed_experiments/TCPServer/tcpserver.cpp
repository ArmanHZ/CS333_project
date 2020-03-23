#include "tcpserver.h"

TcpServer::TcpServer(QObject *parent) : QObject(parent) {
    server = new QTcpServer(this);
    connect(server, SIGNAL(newConnection()), this, SLOT(createNewConnection()));
    if (!server->listen(QHostAddress::Any, 56234)) {
        std::cout << " Error while starting the server!" << std::endl;
        exit(-1);
    }
    else
        std::cout << " Server started!" << std::endl;
}

void TcpServer::createNewConnection() {
    socket = server->nextPendingConnection();
    connect(socket, SIGNAL(readyRead()), this, SLOT(readDataFromClient()));
    std::cout << "Received a connection!" << std::endl;
    writeToSocket("You are connected to the Server.");
    initWorker();
}

void TcpServer::readDataFromClient() {
    qDebug() << "Reading data...";
    auto data = socket->readAll();
    std::cout << "[Client] <-- " << data.toStdString() << std::endl;
}

void TcpServer::writeToSocket(QByteArray message) {
    std::cout << "[Host] --> " << message.toStdString() << std::endl;
    socket->write(message);
    socket->flush();
    socket->waitForBytesWritten();
}

void TcpServer::initWorker() {
    QThread* thread = new QThread;
    worker = new Worker(socket);
    worker->moveToThread(thread);
    connect(thread, SIGNAL (started()), worker, SLOT (process()));
    connect(worker, SIGNAL (finished()), thread, SLOT (quit()));
    connect(worker, SIGNAL (finished()), worker, SLOT (deleteLater()));
    connect(thread, SIGNAL (finished()), thread, SLOT (deleteLater()));
    thread->start();
}

