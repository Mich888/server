#ifndef MYTHREAD_H
#define MYTHREAD_H

#include <QThread>
#include <QTcpSocket>
#include <QDebug>
#include <QDataStream>

class Test {
public:
    Test(int number1, int number2): number1(number1), number2(number2) {
        v.push_back(number1);
        v.push_back(number2);
    }

    int number1;
    int number2;
    std::vector<int> v;
};

class MyThread : public QThread
{
    Q_OBJECT
public:
    explicit MyThread(qintptr ID, QObject *parent = 0, int number = -1);

    void run();

signals:
    void error(QTcpSocket::SocketError socketerror);
    void notifyThread();
    void notifyFirstThread();
    void notifySecondThread();

public slots:
    void readyRead();
    void disconnected();
    void sendDataToClient();

private:
    QTcpSocket *socket;
    qintptr socketDescriptor;
    int number;
};

#endif // MYTHREAD_H
