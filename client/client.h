#ifndef CLIENT_H
#define CLIENT_H

#include <QFile>
#include <QHostAddress>
#include <QList>
#include <QObject>
#include <QProcess>
#include <QString>
#include <QStringList>
#include <QTcpSocket>

class Client : public QObject {
  Q_OBJECT
  Q_PROPERTY(QStringList messages READ messages NOTIFY messagesChanged)
  Q_PROPERTY(QString status READ status NOTIFY statusChanged)
  Q_PROPERTY(bool connected READ connected NOTIFY connectedChanged)

public:
  explicit Client(QObject *parent = nullptr);

  Q_INVOKABLE void setUsername(const QString &username);
  Q_INVOKABLE void connectToServer(const QString &ip, quint16 port);
  Q_INVOKABLE void sendMessage(const QString &message);
  Q_INVOKABLE void recordAndSendVideo();
  Q_INVOKABLE void receiveVideoData(const QByteArray &data);
// Q_INVOKABLE void receiveVideo(QTcpSocket *socket, const QString &outputFilename);
  Q_INVOKABLE void startVideoPreview();
  Q_INVOKABLE void playReceivedVideo();

  QStringList messages() const;
  QString status() const;
  bool connected() const;

  void playVideo(const QString& filePath);
signals:
  void messagesChanged();
  void statusChanged();
  void connectedChanged();
  void videoReceived(const QString& filePath);

private slots:
  void onConnected();
  void onDisconnected();
  void onErrorOccurred(QAbstractSocket::SocketError socketError);
  void onMessageReceived();

private:
  void sendVideo(const QString &filePath);
  void saveVideo(const QByteArray &videoData);        
  QProcess* videoProcess;
  QString m_status;
  QString m_username;
  QStringList m_messages;
  QTcpSocket *socket;
  bool m_connected;
};

#endif // CLIENT_H
