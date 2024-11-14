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

public:
  explicit Client(QObject *parent = nullptr);

  Q_INVOKABLE void sendMessage(const QString &message);

  QStringList messages() const;
  QString status() const;

signals:
  void messagesChanged();
  void statusChanged();

private slots:
  void onConnected();
  void onDisconnected();
  void onErrorOccurred(QAbstractSocket::SocketError socketError);
  void onMessageReceived();

private:
  QString m_status;
  QStringList m_messages;
  QTcpSocket *socket;
};

#endif // CLIENT_H
