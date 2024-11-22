#ifndef CLIENT_H
#define CLIENT_H

#include <QHostAddress>
#include <QList>
#include <QObject>
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

  QStringList messages() const;
  QString status() const;
  bool connected() const;

signals:
  void messagesChanged();
  void statusChanged();
  void connectedChanged();

private slots:
  void onConnected();
  void onDisconnected();
  void onErrorOccurred(QAbstractSocket::SocketError socketError);
  void onMessageReceived();

private:
  QString m_status;
  QString m_username;
  QStringList m_messages;
  QTcpSocket *socket;
  bool m_connected;
};

#endif // CLIENT_H
