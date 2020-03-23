#include "tcpserver.h"
#include "ui_tcpserver.h"

TcpServer::TcpServer(QWidget *parent) : QMainWindow(parent), ui(new Ui::TcpServer) {
    ui->setupUi(this);
    connect(ui->startServerButton, SIGNAL(released()), this, SLOT(startButtonPressed()));
    connect(ui->sendButton, SIGNAL(released()), this, SLOT(sendButtonPressed()));
    connect(ui->lineEdit, SIGNAL(returnPressed()), this, SLOT(sendButtonPressed()));
}

TcpServer::~TcpServer() {
    delete ui;
    delete server;
    delete clientConnection;
}

void TcpServer::startButtonPressed() {
    ui->startServerButton->setEnabled(false);
    server = new QTcpServer(this);
    connect(server, SIGNAL(newConnection()), this, SLOT(newConnection()));

    if (!server->listen(QHostAddress::LocalHost, 5692)) {
        ui->plainTextEdit->insertPlainText("Error creating the server. Use an other port!\n");
        exit(-1);
    } else
        ui->plainTextEdit->insertPlainText("Server created!\n");
}

void TcpServer::newConnection() {
    clientConnection = server->nextPendingConnection();
    ui->sendButton->setEnabled(true);
    ui->plainTextEdit->insertPlainText("A client has connected!\n");
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
        ui->plainTextEdit->insertPlainText("[Host] --> " + message + "\n");
        ui->lineEdit->setText("");
    }
}

void TcpServer::sendButtonPressed() {
    if (ui->sendButton->isEnabled())
        sendMessage();
}

// TODO
void TcpServer::onMessageReceived() {

}


