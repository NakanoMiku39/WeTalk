cmake_minimum_required(VERSION 3.16)
project(client_qml LANGUAGES CXX)
find_package(Qt6 REQUIRED COMPONENTS Core Gui Qml Quick)
qt_standard_project_setup()
qt_add_executable(client_qml
main.cpp
client.cpp
client.h
)
qt_add_qml_module(client_qml
URI client_qml
OUTPUT_DIRECTORY qml
VERSION 1.0
RESOURCE_PREFIX /qt/qml
QML_FILES
client.qml
# nokeyboard.qml
)
target_link_libraries(client_qml
PRIVATE
Qt::Core
Qt::Gui
Qt::Qml
Qt::Quick
)
