#ifndef CLIENT_H
#define CLIENT_H

#include <QObject>
#include <QString>
#include <QList>

class Client : public QObject {
      Q_OBJECT
  Q_PROPERTY(QStringList messages READ messages NOTIFY messagesChanged)

public:
  explicit Client(QObject *parent = nullptr);

  Q_INVOKABLE void sendMessage(const QString &message);

  QStringList messages() const;

signals:
  void messagesChanged();

private:
  QStringList m_messages;

};

#endif // CLIENT_H
