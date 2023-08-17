#include "server.h"

Server::Server()
{
  if(this->listen(QHostAddress::Any, 2323)){
    qDebug() << "start";
  }
  else{
    qDebug() << "error";
  }
  nextBlockSize = 0;
}

void Server::SendToClient(QString str)
{
  Data.clear();
  QDataStream out(&Data, QIODevice::WriteOnly);
  out << quint16(0) << QTime::currentTime() << str;
  out.device()->seek(0);
  out << quint16(Data.size() - sizeof(quint16));
  //socket->write(Data);
  for(int i=0; i<Sockets.size(); ++i){
    Sockets[i]->write(Data);
  }
}

void doYourStuff(const QByteArray &page)
{
  QTcpSocket *socket = new QTcpSocket;
  socket->connectToHost("qt.io", 80);
  QObject::connect(socket, &QTcpSocket::connected, [socket, page] () {
    socket->write(QByteArray("GET " + page + ""));
  });
  QObject::connect(socket, &QTcpSocket::readyRead, [socket] () {
    qDebug()<< "GOT DATA "<< socket->readAll();
  });
  QObject::connect(socket, &QTcpSocket::disconnected, [socket] () {
    qDebug()<< "DISCONNECTED ";
    socket->deleteLater();
  });
}

void Server::incomingConnection(qintptr socketDescriptor)
{
  socket = new QTcpSocket;
  socket->setSocketDescriptor(socketDescriptor);
  QTcpSocket *socketTo = socket;
  connect(socket, &QTcpSocket::readyRead, this, &Server::slotReadyRead);
  //connect(socket, &QTcpSocket::disconnected, this, &Server::disconnectConnection);
  QObject::connect(socket, &QTcpSocket::disconnected, [socketTo, socketDescriptor] () {
    qDebug()<< "DISCONNECTED " << socketDescriptor;
    socketTo->deleteLater();
  });

  Sockets.push_back(socket);
  qDebug() << "client connected" << socketDescriptor;
}

void Server::disconnectConnection()
{
  Sockets.erase(std::remove(Sockets.begin(), Sockets.end(), socket), Sockets.end());
  Sockets.removeLast();
}

void Server::slotReadyRead()
{
  socket = (QTcpSocket*)sender();
  QDataStream in(socket);
  if(in.status() == QDataStream::Ok){
    qDebug() << "read...";
    /*QString str;
    in >> str;
    qDebug() << str;
    SendToClient(str);*/
    while(true){
      if(nextBlockSize == 0){
        qDebug() << "nextBlockSize == 0";
        if(socket->bytesAvailable() < 2){
          qDebug() << "Data < 2, break";
          break;
        }
        in >> nextBlockSize;
        qDebug() << "nextBlockSize = " << nextBlockSize;
      }
      if(socket->bytesAvailable() < nextBlockSize){
        qDebug() << "Data not full, break";
        break;
      }

      QString str;
      QTime time;
      in >> time >> str;
      nextBlockSize = 0;
      qDebug() << str;
      SendToClient(str);
      break;
    }
  }
  else{
    qDebug() << "DataStream error";
  }
}


