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
    clientConnection->write("You are connected to the server.");
    clientConnection->flush();
    clientConnection->waitForBytesWritten();
}

void TcpServer::sendMessage() {
    if (!ui->lineEdit->text().isEmpty()) {
        QString message = ui->lineEdit->text();
        if (ui->encryptCheckBox->isChecked())
            message = QString(crypto::encryptMessage(message.toStdString(), values::e, values::n).c_str());
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
    static bool firstMessage = true;
    if (firstMessage) {
        auto receivedMessage = clientConnection->readAll();
        auto publicKeyPair = receivedMessage.toStdString().substr(11);
        QString publicKeyPairAsQString = publicKeyPair.c_str();
        auto qPair = publicKeyPairAsQString.split(",");
        values::clientN = qPair.at(0).toStdString();
        values::clientE = qPair.at(1).toStdString();
//        ui->plainTextEdit->appendPlainText("[Client] <-- " + receivedMessage);
//        std::cout << "Client N: " << values::clientN << "\nClient E: " << values::clientE << "\n";
        firstMessage = false;
        return;
    }
    if (ui->decryptCheckBox->isChecked()) {
        std::cout << "In decrypt if" << std::endl;
        auto receivedMessage = clientConnection->readAll();
        std::cout << "Received Message: " << receivedMessage.toStdString() << std::endl;
        auto decryptedMessage = crypto::decryptMessage(receivedMessage.toStdString(), values::clientE, values::clientN);
        std::cout << "Decrypted message: " << decryptedMessage << std::endl;
        ui->plainTextEdit->appendPlainText("[Client] <-- " + QString(decryptedMessage.c_str()));
        std::cout << "print to screen" << std::endl;
    } else {
        auto receivedMessage = clientConnection->readAll();
        ui->plainTextEdit->appendPlainText("[Client] <-- " + receivedMessage);
    }
}

void TcpServer::clientDisconnected() {
    ui->plainTextEdit->appendPlainText("Client has disconnected!");
}
