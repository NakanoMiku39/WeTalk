#include "client.h"

Client::Client(QObject *parent) : QObject(parent), m_status("Disconnected") {
  socket = new QTcpSocket(this);
  videoProcess = new QProcess(this);

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

void Client::startVideoPreview() {
  QString device = "/dev/video0";
  QString previewCommand = "ffplay";
  QStringList previewArgs = {"-f", "v412", "-i", device};

  videoProcess->start(previewCommand, previewArgs);
}

void Client::recordAndSendVideo() {
  QString outputFile = "video_output.mp4";
  QString command = "ffmpeg";
  QStringList args = {"-y", "-f", "v4l2", "-input_format", "mjpeg", "-i", "/dev/video0", "-t", "10", outputFile };

  videoProcess->start(command, args);
  connect(videoProcess, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this, [=](int exitCode, QProcess::ExitStatus status) {
    if(exitCode == 0) {
      sendVideo(outputFile);
    }
  });
}

void Client::sendVideo(const QString &filePath) {
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << "Failed to open video file.";
        return;
    }


qint64 fileSize = file.size();  // Get the size of the file
    QByteArray header = "[VIDEO]";  // VIDEO prefix
    QByteArray sizeHeader = QByteArray::number(fileSize) + ";";  // Append delimiter after the size

    socket->write(header);  // Send [VIDEO] prefix
    socket->flush();
    socket->write(sizeHeader);  // Send the size of the video followed by a delimiter
    socket->flush();
    qDebug() << "Header: " << header;
    qDebug() << "sizeHeader: " << sizeHeader;
    // Send the video data in chunks
    while (!file.atEnd()) {
        QByteArray chunk = file.read(4096);  // Read 4 KB at a time
        socket->write(chunk);
        socket->flush();  // Ensure data is sent immediately
    }

        file.close();
    // Send the [/VIDEO] suffix
    qDebug() << "Video data sent.";
}

void Client::receiveVideoData(const QString &prefix, const QByteArray &data) {
  printf("Video Received\n");
    QFile videoFile("received_video.mp4");
    if(videoFile.open(QIODevice::WriteOnly)) {
      videoFile.write(data);
      videoFile.close();
      qDebug() << "Video saved to received_video.mp4";
      m_messages.append("Received a video and saved it.");
      emit messagesChanged();
    }

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
    QByteArray line = socket->readLine();
    if(line.startsWith("[VIDEO]")) {
      QByteArray videoData = line.mid(7);
      receiveVideoData("[VIDEO]", videoData);
    } else {
      QString text = QString::fromUtf8(line).trimmed();
      m_messages.append(text);
      emit messagesChanged();
    }


  }
}
