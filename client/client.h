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

public:
  explicit Client(QObject *parent = nullptr);

  Q_INVOKABLE void sendMessage(const QString &message);

  QStringList messages() const;

signals:
  void messagesChanged();

private slots:
  void onConnected();
  void onMessageReceived();

private:
  QStringList m_messages;
  QTcpSocket *socket;
};

#endif // CLIENT_H
