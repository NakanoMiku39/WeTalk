#include "client.h"

Client::Client(QObject *parent) : QObject(parent), m_status("Disconnected") {
  socket = new QTcpSocket(this);
  videoRecorder = new QProcess(this);

  connect(socket, &QTcpSocket::connected, this, &Client::onConnected);
  connect(socket, &QTcpSocket::disconnected, this, &Client::onDisconnected);
  connect(socket, &QTcpSocket::readyRead, this, &Client::onMessageReceived);
  connect(socket, &QTcpSocket::errorOccurred, this, &Client::onErrorOccurred);

  // socket->connectToHost(QHostAddress("127.0.0.1"), 6666);
}

void Client::setUsername(const QString &username) {
  m_username = username;
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

bool Client::connected() const {
  return m_connected;
}

void Client::sendMessage(const QString &message) {
  if(socket->state() == QTcpSocket::ConnectedState) {
    QString formattedMessage = "[TEXT] " + message + "\n";
    socket->write(formattedMessage.toUtf8());
  }

}

void Client::recordAndSendVideo(const QString &device) {
  QString outputFile = "video_output.mp4";
  QString command = "ffmpeg";
  QStringList args = {"-y", "-f", "v412", "-i", device, "-t", "5", outputFile };

  videoRecorder->start(command, args);
  connect(videoRecorder, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this, [=](int exitCode, QProcess::ExitStatus status) {
    if(exitCode == 0) {
      sendVideo(outputFile);
    }
  });
}

void Client::sendVideo(const QString &filePath) {
  QFile file(filePath);
  if(!file.open(QIODevice::ReadOnly)) {
    qDebug() << "Failed to open video file.";
    return;
  }

  QByteArray videoData = file.readAll();
  file.close();

  QByteArray header = QString("[VIDEO] ").toUtf8();
  socket->write(header + videoData);
}

void Client::onConnected() {
  m_status = "Connected";
  m_connected = true;
  emit statusChanged();
  emit connectedChanged();
  // Sending inital message such as username
  QString initialMessage = m_username;
  socket->write(initialMessage.toUtf8());
}

void Client::onDisconnected() {
  m_status = "Disconnected";
  m_connected = false;
  emit statusChanged();
  emit connectedChanged();
}

void Client::onErrorOccurred(QAbstractSocket::SocketError socketError) {
  m_status = "Error: " + socket->errorString();
  m_connected = false;
  emit statusChanged();
  emit connectedChanged();
}

void Client::onMessageReceived() {
  while(socket->canReadLine()) {
    QString line = QString::fromUtf8(socket->readLine()).trimmed();
    m_messages.append(line);
    emit messagesChanged();
  }
}
