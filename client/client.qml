import QtQuick 2.15
import QtQuick.Controls 2.15
// import QtMultimedia 6.5
import com.example.chat 1.0

ApplicationWindow {
    visible: true
    width: 640
    height: 480
    title: "Chat Client"

    Client {
        id: chatClient
        onMessagesChanged: {
            if(client.messages[client.messages.length - 1] === "Received a video and saved it.") {
                playVideoButton.visible = true
            }
        }
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
                    text: "192.168.0.101"
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
                    width: parent.width
                    id: chatContainer
                    anchors.fill: parent
                    anchors.topMargin: 40
                    color: "#FFFFFF"

                    Column {
                        spacing: 10
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
                                height: parent.height - buttonRow.height - 20
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

                            // 视频播放器
                            // VideoPlayer {
                               //  id: videoPlayer
                                // width: parent.width
                                // height: 200
                                // visible: false
                            // }



                        // 输入区域
                        Rectangle {
                            id: inputArea
                            width: parent.width
                            height: 80
                            color: "#F0F0F0"
                            border.color: "#CCCCCC"

                            Row {
                                spacing: 10
                                anchors.fill: parent
                                anchors.margins: 10
                                id: buttonRow
                                TextField {
                                    id: inputField
                                    placeholderText: "Type a message..."
                                    width: parent.width * 0.5
                                }

                                Button {
                                    id: sendButton
                                    text: "Send"
                                    width: 50
                                    height: 50
                                    onClicked: {
                                        if (inputField.text.trim() !== "") {
                                            chatClient.sendMessage(inputField.text)
                                            inputField.text = ""
                                        }
                                    }
                                }

                                Button {
                                    id: videoButton
                                    text: "Record Video"
                                    width: 50
                                    height: 50
                                    onClicked: {
                                        chatClient.recordAndSendVideo();
                                    }
                                }
                                Button {
                                    id: playVideoButton
                                    text: "Play"
                                    visible: true
                                    width: 50
                                    height: 50
                                    onClicked: {
                                        client.playReceivedVideo()
                                    }
                                }

                                Button {
                                    id: exitButton
                                    text: "Exit"
                                    width: 50
                                    height:50
                                    onClicked: {
                                        chatClient.sendMessage("[CMD]")
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
    // 处理视频接收的信号
    //    Connections {
       //     target: chatClient

         //   function onVideoReceived(filePath) {
         //       videoPlayer.visible = true
           //     videoPlayer.playVideo(filePath)
            //}
        //}

}
