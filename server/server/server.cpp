#include "server.h"

Server::Server()
{
  if(this->listen(QHostAddress::Any, 2323)){
    qDebug() << "start";
  }
  else{
    qDebug() << "error";
  }
}

void Server::SendToClient(QString str)
{
  Data.clear();
  QDataStream out(&Data, QIODevice::WriteOnly);
  out << str;
  socket->write(Data);
}

void Server::incomingConnection(qintptr socketDescriptor)
{
  socket = new QTcpSocket;
  socket->setSocketDescriptor(socketDescriptor);
  connect(socket, &QTcpSocket::readyRead, this, &Server::slotReadyRead);
  connect(socket, &QTcpSocket::disconnected, socket, &QTcpServer::deleteLater);

  Sockets.push_back(socket);
  qDebug() << "client connected" << socketDescriptor;
}

void Server::slotReadyRead()
{
  socket = (QTcpSocket*)sender();
  QDataStream in(socket);
  if(in.status() == QDataStream::Ok){
    qDebug() << "read...";
    QString str;
    in >> str;
    qDebug() << str;
  }
  else{
    qDebug() << "DataStream error";
  }
}


