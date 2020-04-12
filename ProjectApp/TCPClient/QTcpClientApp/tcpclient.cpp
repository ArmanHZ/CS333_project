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
        if (ui->encryptCheckBox->isChecked())
            message = QString(crypto::encryptMessage(message.toStdString(), values::d, values::n).c_str());
        socket->write(message.toLocal8Bit());
        socket->flush();
        socket->waitForBytesWritten();
        ui->plainTextEdit->appendPlainText("[Client] --> " + message);
        ui->lineEdit->setText("");
    }
}

void TcpClient::onMessageReceived() {
    auto receivedMessage = socket->readAll();
    ui->plainTextEdit->appendPlainText("[Host] <-- " + receivedMessage);
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
