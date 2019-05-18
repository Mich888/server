#include "mythread.h"

extern std::vector<QByteArray> players;
extern std::vector<QThread*> threads;

MyThread::MyThread(qintptr ID, QObject *parent, int number) :
    QThread(parent)
{
    this->socketDescriptor = ID;
    this->number = number;
}

void MyThread::run()
{
    // thread starts here
    qDebug() << " Thread started";

    socket = new QTcpSocket();

    // set the ID
    if(!socket->setSocketDescriptor(this->socketDescriptor))
    {
        // something's wrong, we just emit a signal
        emit error(socket->error());
        return;
    }

    // connect socket and signal
    // note - Qt::DirectConnection is used because it's multithreaded
    //        This makes the slot to be invoked immediately, when the signal is emitted.

    connect(socket, SIGNAL(readyRead()), this, SLOT(readyRead()), Qt::DirectConnection);
    connect(socket, SIGNAL(disconnected()), this, SLOT(disconnected()));

    threads[number] = this;

    if (number == 0) {
        qDebug() << "waiting for second thread pointer";
        while (threads[1] == nullptr) {
            ;
        }
    }

    connect(this, SIGNAL(notifyThread()), threads[1 - number], SLOT(sendDataToClient()), Qt::AutoConnection);

    // We'll have multiple clients, we want to know which is which
    qDebug() << socketDescriptor << " Client connected";

    // make this thread a loop,
    // thread will stay alive so that signal/slot to function properly
    // not dropped out in the middle when thread dies
    QByteArray player_number;
    QDataStream stream(&player_number, QIODevice::WriteOnly);
    stream << number;
    socket->write(player_number);
    exec();
}

void MyThread::readyRead()
{
    // get the information
    QByteArray Data = socket->readAll();

    // will write on server side window
    qDebug() << socketDescriptor << " Data in: " << Data;

    // Append data to other player's cell.
    players[1 - number].clear();
    players[1 - number].append(Data);

    //socket->write(players[number]);
    emit(notifyThread());
}

void MyThread::disconnected()
{
    qDebug() << socketDescriptor << " Disconnected";
    socket->deleteLater();
    exit(0);
}

void MyThread::sendDataToClient() {
    qDebug() << "send data to client slot";
    qDebug() << "my number: " << number;
    qDebug() << "socket descriptor: " << socketDescriptor;
    qDebug() << "players: " << players[number];
    socket->write(players[number]);
}
