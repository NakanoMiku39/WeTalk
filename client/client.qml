import QtQuick 2.15
import QtQuick.Controls 2.15
import com.example.chat 1.0

ApplicationWindow {
    visible: true
    width: 400
    height: 600
    title: "Chat Client"

    Client {
        id: chatClient
    }

    StackView {
        id: mainStack
        anchors.fill: parent // 填满窗口

        // 初始界面
        initialItem: Rectangle {
            width: mainStack.width
            height: mainStack.height

            Column {
                spacing: 10
                anchors.centerIn: parent

                TextField {
                    id: usernameField
                    placeholderText: "Enter Username"
                    text: "User"
                }

                TextField {
                    id: ipField
                    placeholderText: "Enter Server IP"
                    text: "127.0.0.1"
                }

                TextField {
                    id: portField
                    placeholderText: "Enter Port"
                    text: "6666"
                    inputMethodHints: Qt.ImhDigitsOnly
                }

                Button {
                    text: "Connect"
                    onClicked: {
                        chatClient.setUsername(usernameField.text)
                        chatClient.connectToServer(ipField.text, parseInt(portField.text))
                    }
                }

                Text {
                    text: chatClient.status
                    color: chatClient.connected ? "green" : "red"
                }
            }
        }
    }

    Connections {
        target: chatClient

        function onConnectedChanged() {
            if (chatClient.connected) {
                mainStack.push(chatScreen)
            }
        }
    }

    // 聊天界面
    Component {
        id: chatScreen

        Rectangle {
            width: mainStack.width
            height: mainStack.height

            Column {
                spacing: 0
                anchors.fill: parent

                // 状态栏
                Rectangle {
                    width: parent.width
                    height: 40
                    color: "#E0E0E0"

                    Text {
                        text: "Status: " + chatClient.status
                        anchors.centerIn: parent
                    }
                }

                // 聊天记录和输入框
                Rectangle {
                    id: chatContainer
                    anchors.fill: parent
                    anchors.topMargin: 40
                    color: "#FFFFFF"

                    Column {
                        spacing: 0
                        anchors.fill: parent

                        // 聊天记录
                        ScrollView {
                            id: chatArea
                            width: parent.width
                            height: parent.height - inputArea.height
                            clip: true

                            ListView {
                                id: messageList
                                width: parent.width
                                height: parent.height
                                model: chatClient.messages
                                spacing: 10

                                // 自动滚动到底部
                                Component.onCompleted: positionViewAtEnd()
                                onContentHeightChanged: positionViewAtEnd()

                                delegate: Rectangle {
                                    width: parent.width - 20
                                    height: implicitHeight
                                    color: index % 2 === 0 ? "#DDEEFF" : "#FFDDEE"
                                    radius: 10

                                    Text {
                                        text: modelData
                                        wrapMode: Text.Wrap
                                        padding: 10
                                        anchors.fill: parent
                                        anchors.margins: 5
                                    }
                                }
                            }
                        }

                        // 输入区域
                        Rectangle {
                            id: inputArea
                            width: parent.width
                            height: 60
                            color: "#F0F0F0"
                            border.color: "#CCCCCC"

                            Row {
                                spacing: 10
                                anchors.fill: parent
                                anchors.margins: 10

                                TextField {
                                    id: inputField
                                    placeholderText: "Type a message..."
                                    width: parent.width - sendButton.width - exitButton.width - 40
                                }

                                Button {
                                    id: sendButton
                                    text: "Send"
                                    onClicked: {
                                        if (inputField.text.trim() !== "") {
                                            chatClient.sendMessage(inputField.text)
                                            inputField.text = ""
                                        }
                                    }
                                }

                                Button {
                                    id: exitButton
                                    text: "Exit"
                                    onClicked: {
                                        chatClient.sendMessage("quit")
                                        Qt.quit() // 退出应用
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}
