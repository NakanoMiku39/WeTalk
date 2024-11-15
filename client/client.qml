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
        anchors.fill: parent // 确保填满窗口

        // 初始界面
        initialItem: Rectangle {
            width: mainStack.width // 绑定到 mainStack 的宽度
            height: mainStack.height // 绑定到 mainStack 的高度

            Column {
                spacing: 10
                anchors.centerIn: parent

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
            width: mainStack.width // 绑定到 mainStack 的宽度
            height: mainStack.height // 绑定到 mainStack 的高度

            Column {
                anchors.fill: parent
                spacing: 0

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

                // 聊天记录
                ScrollView {
                    id: chatArea
                    anchors.fill: parent
                    height: parent.height - inputArea.height - 40

                    ListView {
                        id: messageList
                        width: parent.width
                        height: parent.height
                        model: chatClient.messages
                        delegate: Text {
                            text: modelData
                        }
                    }
                }

                // 输入区域
                Rectangle {
                    id: inputArea
                    width: parent.width
                    height: 60
                    color: "#F0F0F0"

                    Row {
                        spacing: 10
                        anchors.fill: parent
                        anchors.margins: 10

                        TextField {
                            id: inputField
                            placeholderText: "Type a message..."
                            width: parent.width - sendButton.width - 20
                        }

                        Button {
                            id: sendButton
                            text: "Send"
                            onClicked: {
                                chatClient.sendMessage(inputField.text)
                                inputField.text = ""
                            }
                        }
                    }
                }
            }
        }
    }
}
