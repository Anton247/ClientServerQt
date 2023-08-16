#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
  ui->setupUi(this);
  socket = new QTcpSocket(this);
  connect(socket, &QTcpSocket::readyRead, this, &MainWindow::slotReadyRead);
  connect(socket, &QTcpSocket::disconnected, socket, &QTcpSocket::deleteLater);
}

MainWindow::~MainWindow()
{
  delete ui;
}


void MainWindow::on_pushButton_clicked()
{
  socket->connectToHost("127.0.0.1", 2323);
}

void MainWindow::SendToServer(QString str)
{
  Data.clear();
  QDataStream out(&Data, QIODevice::WriteOnly);
  out << str;
  socket->write(Data);
}

void MainWindow::slotReadyRead()
{
  QDataStream in(socket);
  if(in.status() == QDataStream::Ok){
    QString str;
    in >> str;
    ui->textBrowser->append(str);
  }
  else{
    ui->textBrowser->append("read error");
  }
}


void MainWindow::on_pushButton_2_clicked()
{
  SendToServer(ui->lineEdit->text());
}


void MainWindow::on_lineEdit_returnPressed()
{
  SendToServer(ui->lineEdit->text());
}

