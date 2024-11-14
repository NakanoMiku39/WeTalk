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
        spacing: 0

        Rectangle {
            width: parent.width
            height: 40
            color: "#E0E0E0"
            border.color: "#B0B0B0"

            Text {
                 text: "Status " + chatClient.status
                 anchors.centerIn: parent
                 font.pixelSize: 14
            }
        }

        // 聊天记录显示区域
        ScrollView {
            id: chatArea
            anchors.horizontalCenter: parent.horizontalCenter
            width: parent.width
            height: parent.height - inputArea.height
            clip: true

            ListView {
                id: messageList
                width: parent.width
                height: parent.height
                model: chatClient.messages
                spacing: 20
                // padding: 10
                clip: true
                interactive: true

                onContentHeightChanged: {
                    messageList.positionViewAtEnd()
                }

                delegate: Rectangle {
                    width: parent.width - 20
                    height: implicitHeight
                    color: index % 2 === 0 ? "#DDEEFF" : "#FFDDEE"
                    radius: 10

                    Text {
                         text: modelData
                         wrapMode: Text.Wrap
                         padding: 20
                         anchors.fill: parent
                         anchors.margins: 5
                    }
                }
            }
        }

        // 输入框和发送按钮
        Rectangle{
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
                    anchors.verticalCenter: parent.verticalCenter
                    width: parent.width - sendButton.width - 30
                    height: parent.height - 20
                }

                Button {
                    id: sendButton
                    text: "Send"
                    anchors.verticalCenter: parent.verticalCenter
                    height: inputField.height
                    onClicked: {
                        if (inputField.text.trim() !== "") {
                           chatClient.sendMessage(inputField.text)
                           inputField.text = ""
                        }
                    }
                }
            }
        }
    }
    onClosing: {
        console.log("Application is closing")
    }

}
