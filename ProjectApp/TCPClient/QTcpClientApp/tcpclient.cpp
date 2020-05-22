#include "tcpclient.h"
#include "ui_tcpclient.h"
#include "Crypto_utils.hpp"

namespace values {
    using namespace crypto;
    const auto p = getNDigitRandomPrimeNumber(10);
    const auto q = getNDigitRandomPrimeNumber(10);
    const auto n = p * q;
    const auto [d, e] = calculateD_E(p, q, n);
    InfInt serverE;
    InfInt serverN;
}

TcpClient::TcpClient(QWidget *parent) : QMainWindow(parent), ui(new Ui::TcpClient) {
    ui->setupUi(this);
    socket = new QTcpSocket(this);
    connect(ui->connectButton, SIGNAL(released()), this, SLOT(connectButtonPressed()));
    connect(ui->sendButton, SIGNAL(released()), this, SLOT(sendButtonPressed()));
    connect(ui->lineEdit, SIGNAL(returnPressed()), this, SLOT(sendButtonPressed()));
    connect(ui->disconnectButton, SIGNAL(released()), this, SLOT(disconnectButtonPressed()));
}

TcpClient::~TcpClient() {
    socket->close();
    delete ui;
    delete socket;
}

void TcpClient::connectButtonPressed() {
    ui->connectButton->setEnabled(false);
    ui->disconnectButton->setEnabled(true);
    connectToHost();
}

void TcpClient::connectToHost() {
    QHostAddress hostAddress = QHostAddress::LocalHost;
    QString connectingMessage = "Connecting to host on " + hostAddress.toString() + ":" + QString::number(PORT);
    ui->plainTextEdit->appendPlainText(connectingMessage);
    while (!socket->waitForConnected())
        socket->connectToHost(hostAddress, PORT);
    ui->plainTextEdit->appendPlainText("Connected!");
    ui->sendButton->setEnabled(true);
    connect(socket, SIGNAL(readyRead()), this, SLOT(onMessageReceived()));
    connect(socket, SIGNAL(disconnected()), this, SLOT(disconnected()));
    sendPublicKey();
}

void TcpClient::sendPublicKey() {
    QString message = "Public key: ";
    message.append(values::n.toString().c_str()).append(",").append(values::e.toString().c_str());
    socket->write(message.toLocal8Bit());
    socket->flush();
    socket->waitForBytesWritten();
    ui->plainTextEdit->appendPlainText("[Client] --> " + message);
}

void TcpClient::sendButtonPressed() {
    if (ui->sendButton->isEnabled())
        sendMessage();
}

void TcpClient::sendMessage() {
    if (!ui->lineEdit->text().isEmpty()) {
        QString message = ui->lineEdit->text();
        QString displayMessage = message;
        if (ui->encryptCheckBox->isChecked()) {
            auto signedMessage = sign(message);
            message = encyptWithServerE(signedMessage);
        }
        socket->write(message.toLocal8Bit());
        socket->flush();
        socket->waitForBytesWritten();
        if (ui->debugInfo->isChecked())
            ui->plainTextEdit->appendPlainText("[Client] --> " + message);
        else
            ui->plainTextEdit->appendPlainText("[Client] --> " + displayMessage);
        ui->lineEdit->setText("");
    }
}

std::string TcpClient::sign(QString initialMessage) {
    return crypto::encryptMessage(initialMessage.toStdString(), values::d, values::n);
}

QString TcpClient::encyptWithServerE(std::string signedMessage) {
    return QString(crypto::encryptMessage(signedMessage, values::serverE, values::serverN).c_str());
}

void TcpClient::onMessageReceived() {
    static bool firstMessage = true;
    if (firstMessage) {
        auto receivedMessage = socket->readAll();
        auto publicKeyPair = receivedMessage.toStdString().substr(11);
        QString publicKeyPairAsQString = publicKeyPair.c_str();
        auto qPair = publicKeyPairAsQString.split(",");
        values::serverN = qPair.at(0).toStdString();
        values::serverE = qPair.at(1).toStdString();
        firstMessage = false;
        ui->plainTextEdit->appendPlainText("[Server] <-- " + QString(receivedMessage));
        return;
    }
    if (ui->decryptCheckBox->isChecked()) {
        auto receivedMessage = socket->readAll();
        auto decryptedMessage = decryptWithClientD(receivedMessage);
        auto unsignedMessage = unsign(decryptedMessage);
        ui->plainTextEdit->appendPlainText("[Server] <-- " + unsignedMessage);
    } else {
        auto receivedMessage = socket->readAll();
        ui->plainTextEdit->appendPlainText("[Server] <-- " + receivedMessage);
    }
}

std::string TcpClient::decryptWithClientD(QString receivedMessage) {
    return crypto::decryptMessage(receivedMessage.toStdString(), values::d, values::n);
}

QString TcpClient::unsign(std::string decryptedMessage) {
    return QString(crypto::decryptMessage(decryptedMessage, values::serverE, values::serverN).c_str());
}

void TcpClient::disconnectButtonPressed() {
    socket->disconnect();
    socket->disconnectFromHost();
    ui->connectButton->setEnabled(true);
    ui->disconnectButton->setEnabled(false);
    ui->sendButton->setEnabled(false);
    ui->plainTextEdit->appendPlainText("Disconnected.");
}

void TcpClient::disconnected() {
    ui->plainTextEdit->appendPlainText("Connection to Host lost!");
}
