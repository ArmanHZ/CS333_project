#include "worker.h"

Worker::Worker(QObject *parent) : QObject(parent) { }

Worker::Worker(QTcpSocket *socketFromHost) {
    this->socket = socketFromHost;
}

Worker::~Worker() = default;

void Worker::process() { // Process. Start processing data.
    // allocate resources using new here
    qDebug("Hello World!");
    socket->write("test from worker");
    socket->flush();
    socket->waitForBytesWritten();
    emit finished();
}
