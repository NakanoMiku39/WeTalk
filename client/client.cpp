#include "client.h"

Client::Client(QObject *parent) : QObject(parent), m_status("Disconnected") {
  socket = new QTcpSocket(this);

  connect(socket, &QTcpSocket::connected, this, &Client::onConnected);
  connect(socket, &QTcpSocket::disconnected, this, &Client::onDisconnected);
  connect(socket, &QTcpSocket::readyRead, this, &Client::onMessageReceived);
  connect(socket, &QTcpSocket::errorOccurred, this, &Client::onErrorOccurred);

  // socket->connectToHost(QHostAddress("127.0.0.1"), 6666);
}

void Client::connectToServer(const QString &ip, quint16 port) {
  if(socket->state() != QTcpSocket::UnconnectedState) {
    socket->disconnectFromHost();
  }

  socket->connectToHost(QHostAddress(ip), port);
  m_status = "Connecting...";
  emit statusChanged();
}

QStringList Client::messages() const {
  return m_messages;
}

QString Client::status() const {
  return m_status;
}

void Client::sendMessage(const QString &message) {
  if(socket->state() == QTcpSocket::ConnectedState) {
    QString formattedMessage = message + "\n";
    socket->write(formattedMessage.toUtf8());
  }

}

void Client::onConnected() {
  m_status = "Connected";
  emit statusChanged();

  // Sending inital message such as username
  QString initialMessage = "ClientName";
  socket->write(initialMessage.toUtf8());
}

void Client::onDisconnected() {
  m_status = "Disconnected";
  emit statusChanged();
}

void Client::onErrorOccurred(QAbstractSocket::SocketError socketError) {
  m_status = "Error: " + socket->errorString();
  emit statusChanged();
}

void Client::onMessageReceived() {
  while(socket->canReadLine()) {
    QString line = socket->readLine().trimmed();
    m_messages.append(line);
    emit messagesChanged();
  }
}
