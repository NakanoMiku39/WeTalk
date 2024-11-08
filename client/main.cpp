#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include "client.h"
int main(int argc, char *argv[])
{
  QGuiApplication app(argc, argv);
  QQmlApplicationEngine engine;
  qmlRegisterType<Client>("CLIENT", 1, 0, "Client"); // 注册 Guess 类，在 guess.h 中定义
  Guess guess;
  QObject::connect(&guess, SIGNAL(onExit()), qApp, SLOT(quit()));
  engine.rootContext()->setContextProperty("client", &client); // 设置 QML 中的可访问属性
  engine.load(QUrl(QStringLiteral("client.qml"))); // 加载 qml 代码
  return app.exec();

}
