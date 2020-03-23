#ifndef WORKER_H
#define WORKER_H

#include <QObject>
#include <QTcpSocket>

class Worker : public QObject {
    Q_OBJECT
public:
    explicit Worker(QObject *parent = nullptr);
public:
    Worker(QTcpSocket *);
    ~Worker();
public slots:
    void process();
signals:
    void finished();
    void error(QString err);
private:
    QTcpSocket *socket;

};

#endif // WORKER_H
