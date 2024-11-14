#include "client.h"

Client::Client(QObject *parent) : QObject(parent) {}

void Client::sendMessage(const QString &message) {
  if (!message.trimmed().isEmpty()) {
    m_messages.append(message);
    emit messagesChanged();
  
  }

}

QStringList Client::messages() const { return m_messages; }
