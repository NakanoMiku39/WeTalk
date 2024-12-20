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
    if(message != "[CMD]"){
    QString formattedMessage = "[TEXT]" + message + "\n";
    socket->write(formattedMessage.toUtf8());
    }
    else{
	QString formattedMessage = message + "\n";
	socket->write(formattedMessage.toUtf8());
    }
  }

}

void Client::startVideoPreview() {
  QString device = "/dev/video0";
  QString previewCommand = "ffplay";
  QStringList previewArgs = {"-f", "v412", "-i", device};

  videoProcess->start(previewCommand, previewArgs);
}

void Client::recordAndSendVideo() {
disconnect(videoProcess, nullptr, this, nullptr);
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
    QByteArray videoData = file.readAll();
    file.close();
    // QByteArray header = "[VIDEO]";  // VIDEO prefix
    QByteArray sizeHeader = QByteArray::number(fileSize) + ";";  // Append delimiter after the size

    QByteArray sendData = "[VIDEO]" + sizeHeader + videoData;
    // socket->write(header);  // Send [VIDEO] prefix
    // socket->flush();
    // socket->write(sizeHeader);  // Send the size of the video followed by a delimiter
    // socket->flush();
    // qDebug() << "Header: " << header;
    // qDebug() << "sizeHeader: " << sizeHeader;
    // Send the video data in chunks
    // while (!file.atEnd()) {
    //     QByteArray chunk = file.read(4096);  // Read 4 KB at a time
    //     socket->write(chunk);
    //     socket->flush();  // Ensure data is sent immediately
    // }
    qint64 bytesSent = socket -> write(sendData);
    socket -> flush();
   //     file.close();
    // Send the [/VIDEO] suffix
    qDebug() << "Video data sent.";
}

void Client::receiveVideoData(const QByteArray &data) {
    static bool receivingVideo = false;      // 标识是否正在接收视频数据
    static int expectedVideoSize = 0;        // 期望接收的视频数据大小
    static int receivedSize = 0;             // 已接收到的视频数据大小
    static QByteArray videoBuffer;           // 用于存储接收到的视频数据

    // 如果尚未开始接收视频数据
    if (!receivingVideo) {
        qDebug() << "Entering receiving condition!";

        // 查找分隔符 ';' 的位置
        int delimiterIndex = data.indexOf(';');
        if (delimiterIndex == -1) {
            qDebug() << "Error: Delimiter not found in video header!";
            return;
        }

        // 提取视频大小信息
        QByteArray sizeData = data.mid(0, delimiterIndex); // 跳过 "[VIDEO]"
        expectedVideoSize = sizeData.toInt();
        qDebug() << "Video size to receive: " << expectedVideoSize;

        // 提取第一个数据块（分隔符之后的视频数据）
        QByteArray initialVideoData = data.mid(delimiterIndex + 1);
        videoBuffer.clear();
        videoBuffer.append(initialVideoData);
        receivedSize = initialVideoData.size();

        receivingVideo = true;
        qDebug() << "Initial video data received: " << receivedSize << " bytes";

        // 检查是否已经接收到完整的视频数据
        if (receivedSize >= expectedVideoSize) {
            saveVideo(videoBuffer);
            receivingVideo = false;
            expectedVideoSize = 0;
            receivedSize = 0;
            videoBuffer.clear();
            qDebug() << "Video reception completed.";
            return;
        }
    }

    // 如果正在接收视频数据
    while (receivingVideo) {
        // 读取可用数据
        if (socket->bytesAvailable() <= 0) {
            socket->waitForReadyRead(100);  // 等待数据到达
            continue;
        }

        QByteArray newData = socket->readAll();
        videoBuffer.append(newData);
        receivedSize += newData.size();
        qDebug() << "Received additional video data: " << newData.size() << " bytes, Total: " << receivedSize << " bytes";

        // 检查是否接收到完整的视频数据
        if (receivedSize >= expectedVideoSize) {
            saveVideo(videoBuffer);
            receivingVideo = false;
            expectedVideoSize = 0;
            receivedSize = 0;
            videoBuffer.clear();
            qDebug() << "Video reception completed.";
        }
    }
}

void Client::saveVideo(const QByteArray &videoData) {
    QFile videoFile("received_video.mp4");
    if (videoFile.open(QIODevice::WriteOnly)) {
        videoFile.write(videoData);  // 写入完整的视频数据
        videoFile.close();
        qDebug() << "Video saved to received_video.mp4";

        m_messages.append("Received a video and saved it.");

    } else {
        qDebug() << "Failed to save video to file.";
    }
}

void Client::playReceivedVideo() {
    // 调用外部播放器播放视频
    QString videoPath = "received_video.mp4";
    QString playerCommand = "ffplay";

    QStringList arguments;
    arguments << videoPath;

    qDebug() << "Playing video using " << playerCommand;

    videoProcess->start(playerCommand, arguments);

    if (!videoProcess->waitForStarted()) {
        qDebug() << "Failed to start video player.";
    }
}


// 保存视频文件并重置状态
// void Client::saveVideo() {
//     QFile videoFile("received_video.mp4");
//     if (videoFile.open(QIODevice::WriteOnly)) {
//         videoFile.write(videoBuffer);  // 写入完整的视频数据
//         videoFile.close();
//         qDebug() << "Video saved to received_video.mp4";

//         m_messages.append("Received a video and saved it.");
//         emit messagesChanged();
//     } else {
//         qDebug() << "Failed to save video to file.";
//     }

//     // 重置状态
//     receivingVideo = false;
//     expectedVideoSize = 0;
//     videoBuffer.clear();
// }


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
    // qDebug() << "Received: " << line;
    if(strncmp(line, "[VIDEO]", 7) == 0) {
      qDebug() << "Video Received!" ;
      QByteArray videoData = line.mid(7);
      receiveVideoData(videoData);
      emit messagesChanged();
    } else if(strncmp(line, "[TEXT]", 6) == 0) {
      QString text = QString::fromUtf8(line).trimmed();
      qDebug() << "Text Received!";
      m_messages.append(line.mid(6));
      emit messagesChanged();
    } else {
      // qDebug() << "Failed to receive!";
    }


  }
}
