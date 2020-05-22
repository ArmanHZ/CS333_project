#include "tcpserver.h"
#include "ui_tcpserver.h"
#include "Crypto_utils.hpp"

namespace values {
    using namespace crypto;
    const auto p = getNDigitRandomPrimeNumber(10);
    const auto q = getNDigitRandomPrimeNumber(10);
    const auto n = p * q;
    const auto [d, e] = calculateD_E(p, q, n);
    InfInt clientE;
    InfInt clientN;
}

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
    sendPublicKey();
}

void TcpServer::sendPublicKey() {
    QString message = "Public key: ";
    message.append(values::n.toString().c_str()).append(",").append(values::e.toString().c_str());
    clientConnection->write(message.toLocal8Bit());
    clientConnection->flush();
    clientConnection->waitForBytesWritten();
    ui->plainTextEdit->appendPlainText("[Server] --> " + message);
}

void TcpServer::sendMessage() {
    if (!ui->lineEdit->text().isEmpty()) {
        QString message = ui->lineEdit->text();
        QString displayMessage = message;
        if (ui->encryptCheckBox->isChecked()) {
            auto signedMessage = sign(message);
            message = encyptWithClientE(signedMessage);
        }
        clientConnection->write(message.toLocal8Bit());
        clientConnection->flush();
        clientConnection->waitForBytesWritten();
        if (ui->debugInfo->isChecked())
            ui->plainTextEdit->appendPlainText("[Host] --> " + message);
        else
            ui->plainTextEdit->appendPlainText("[Host] --> " + displayMessage);
        ui->lineEdit->setText("");
    }
}

std::string TcpServer::sign(QString initialMessage) {
    return crypto::encryptMessage(initialMessage.toStdString(), values::d, values::n);
}

QString TcpServer::encyptWithClientE(std::string signedMessage) {
    return QString(crypto::encryptMessage(signedMessage, values::clientE, values::clientN).c_str());
}

void TcpServer::sendButtonPressed() {
    if (ui->sendButton->isEnabled())
        sendMessage();
}

void TcpServer::onMessageReceived() {
    static bool firstMessage = true;
    if (firstMessage) {
        auto receivedMessage = clientConnection->readAll();
        auto publicKeyPair = receivedMessage.toStdString().substr(11);
        QString publicKeyPairAsQString = publicKeyPair.c_str();
        auto qPair = publicKeyPairAsQString.split(",");
        values::clientN = qPair.at(0).toStdString();
        values::clientE = qPair.at(1).toStdString();
        firstMessage = false;
        ui->plainTextEdit->appendPlainText("[Client] <-- " + receivedMessage);
        return;
    }
    if (ui->decryptCheckBox->isChecked()) {
        auto receivedMessage = clientConnection->readAll();
        auto decryptedMessage = decryptWithServerD(receivedMessage);
        auto unsignedMessage = unsign(decryptedMessage);
        ui->plainTextEdit->appendPlainText("[Client] <-- " + unsignedMessage);
    } else {
        auto receivedMessage = clientConnection->readAll();
        ui->plainTextEdit->appendPlainText("[Client] <-- " + receivedMessage);
    }
}

std::string TcpServer::decryptWithServerD(QString receivedMessage) {
    return crypto::decryptMessage(receivedMessage.toStdString(), values::d, values::n);
}

QString TcpServer::unsign(std::string decryptedMessage) {
    return QString(crypto::decryptMessage(decryptedMessage, values::clientE, values::clientN).c_str());
}

void TcpServer::clientDisconnected() {
    ui->plainTextEdit->appendPlainText("Client has disconnected!");
}
