#include "tcpserver.h"
#include "ui_tcpserver.h"

TcpServer::TcpServer(QWidget *parent) : QMainWindow(parent), ui(new Ui::TcpServer) {
    ui->setupUi(this);
    connect(ui->startServerButton, SIGNAL(released()), this, SLOT(startButtonPressed()));
    connect(ui->sendButton, SIGNAL(released()), this, SLOT(sendButtonPressed()));
    connect(ui->lineEdit, SIGNAL(returnPressed()), this, SLOT(sendButtonPressed()));
}

TcpServer::~TcpServer() {
    server->close();
    clientConnection->close();
    delete ui;
    delete server;
    delete clientConnection;
}

void TcpServer::startButtonPressed() {
    ui->startServerButton->setEnabled(false);
    server = new QTcpServer(this);
    connect(server, SIGNAL(newConnection()), this, SLOT(newConnection()));

    if (!server->listen(QHostAddress::LocalHost, this->PORT)) {
        ui->plainTextEdit->appendPlainText("Error creating the server. Use an other port!");
        exit(-1);
    } else
        ui->plainTextEdit->appendPlainText("Server created!");
}

void TcpServer::newConnection() {
    clientConnection = server->nextPendingConnection();
    connect(clientConnection, SIGNAL(readyRead()), this, SLOT(onMessageReceived()));
    connect(clientConnection, SIGNAL(disconnected()), this, SLOT(clientDisconnected()));
    ui->sendButton->setEnabled(true);
    ui->plainTextEdit->appendPlainText("A client has connected!");
    clientConnection->write("You are connected to the server.");
    clientConnection->flush();
    clientConnection->waitForBytesWritten();
}

void TcpServer::sendMessage() {
    if (!ui->lineEdit->text().isEmpty()) {
        QString message = ui->lineEdit->text();
        clientConnection->write(message.toLocal8Bit());
        clientConnection->flush();
        clientConnection->waitForBytesWritten();
        ui->plainTextEdit->appendPlainText("[Host] --> " + message);
        ui->lineEdit->setText("");
    }
}

void TcpServer::sendButtonPressed() {
    if (ui->sendButton->isEnabled())
        sendMessage();
}

void TcpServer::onMessageReceived() {
    auto receivedMessage = clientConnection->readAll();
    ui->plainTextEdit->appendPlainText("[Client] <-- " + receivedMessage);
}

void TcpServer::clientDisconnected() {
    ui->plainTextEdit->appendPlainText("Client has disconnected!");
}
