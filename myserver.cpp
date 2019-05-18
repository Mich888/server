// myserver.cpp

#include "myserver.h"
#include "mythread.h"

std::vector<QByteArray> players;
std::vector<QThread*> threads;

MyServer::MyServer(QObject *parent) :
    QTcpServer(parent)
{
}

void MyServer::startServer()
{
    int port = 1235;
    players.resize(2);
    players[0] = QByteArray();
    players[1] = QByteArray();
    threads.push_back(nullptr);
    threads.push_back(nullptr);

    if(!this->listen(QHostAddress::Any, port))
    {
        qDebug() << "Could not start server";
    }
    else
    {
        qDebug() << "Listening to port " << port << "...";
    }
}

// This function is called by QTcpServer when a new connection is available.
void MyServer::incomingConnection(qintptr socketDescriptor)
{
    static int player_number = 0;

    // We have a new connection
    qDebug() << socketDescriptor << " Connecting...";

    // Every new connection will be run in a newly created thread
    MyThread *thread = new MyThread(socketDescriptor, this, player_number);
    player_number++;

    // connect signal/slot
    // once a thread is not needed, it will be beleted later
    connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));

    thread->start();
}
