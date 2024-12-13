#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include "client.h"
int main(int argc, char *argv[])
{
  QGuiApplication app(argc, argv);
  QQmlApplicationEngine engine;
  qmlRegisterType<Client>("com.example.chat", 1, 0, "Client");
  // qmlRegisterType<VideoPlayer>("com.example.chat", 1, 0, "VideoPlayer");

  Client client;
  QObject::connect(&client, SIGNAL(onExit()), qApp, SLOT(quit()));
  engine.rootContext()->setContextProperty("client", &client); // 设置 QML 中的可访问属性
  // engine.rootContext()->setContextProperty("videoplayer", &videoplayer);
  engine.load(QUrl(QStringLiteral("../client.qml"))); // 加载 qml 代码
  return app.exec();

}
