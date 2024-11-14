cmake_minimum_required(VERSION 3.16)
project(Client LANGUAGES CXX)
find_package(Qt6 REQUIRED COMPONENTS Core Gui Qml Quick)
qt_standard_project_setup()
qt_add_executable(client
main.cpp
client.cpp
client.h
)
qt_add_qml_module(Client
URI com.example.chat
# OUTPUT_DIRECTORY qml
VERSION 1.0
# RESOURCE_PREFIX /qt/qml
QML_FILES
client.qml
# nokeyboard.qml
RESOURCES
qml.qrc
)
target_link_libraries(client
PRIVATE
Qt::Core
Qt::Gui
Qt::Qml
Qt::Quick
)
