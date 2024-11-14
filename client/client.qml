import QtQuick 2.15
import QtQuick.Controls 2.15
import com.example.chat 1.0

ApplicationWindow {
    visible: true
    width: 400
    height: 600
    title: "Chat Interface"

    Client {
        id: chatClient
    }

    Column {
        anchors.fill: parent

        // 聊天记录显示区域
        ScrollView {
            id: chatArea
            anchors.fill: parent
            height: parent.height - 60
            clip: true

            Column {
                width: parent.width
                spacing: 10
                Repeater {
                    model: chatClient.messages
                    delegate: Rectangle {
                        width: parent.width - 20
                        height: implicitHeight
                        color: "lightblue"
                        radius: 10
                        Text {
                            text: modelData
                            wrapMode: Text.Wrap
                            padding: 10
                        }
                    }
                }
            }
        }

        // 输入框和发送按钮
        Row {
            height: 60
            width: parent.width
            spacing: 10
            padding: 10

            TextField {
                id: inputField
                placeholderText: "Type a message..."
                anchors.verticalCenter: parent.verticalCenter
                width: parent.width - sendButton.width - 20
            }

            Button {
                id: sendButton
                text: "Send"
                anchors.verticalCenter: parent.verticalCenter
                onClicked: {
                    if (inputField.text.trim() !== "") {
                        chatClient.sendMessage(inputField.text)
                        inputField.text = ""
                    }
                }
            }
        }
    }

    onClosing: {
        console.log("Application is closing")
    }

}
