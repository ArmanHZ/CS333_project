#include <QCoreApplication>
#include "tcpclient.h"

int main(int argc, char *argv[]) {
    QCoreApplication a(argc, argv);

    TcpClient client;

    return a.exec();
}
