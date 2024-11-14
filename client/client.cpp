#include "client.h"

Client::Client(QObject *parent) : QObject(parent) {
  socket = new QTcpSocket(this);

  connect(socket, &QTcpSocket::connected, this, &Client::onConnected);
  connect(socket, &QTcpSocket::readyRead, this, &Client::onMessageReceived);

  socket->connectToHost(QHostAddress("127.0.0.1"), 6666);
}

void Client::sendMessage(const QString &message) {
  if(socket->state() == QTcpSocket::ConnectedState) {
    QString formattedMessage = message + "\n";
    socket->write(formattedMessage.toUtf8());
  }

}

QStringList Client::messages() const { return m_messages; }

void Client::onConnected() {
  QString initialMessage = "ClientName";
  socket->write(initialMessage.toUtf8());
}

void Client::onMessageReceived() {
  while(socket->canReadLine()) {
    QString line = socket->readLine().trimmed();
    m_messages.append(line);
    emit messagesChanged();
  }
}
