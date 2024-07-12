import QtQuick
import QtQuick.Controls

Window {
    id: mainAppWindow
    width: 640
    height: 480
    visible: true
    title: qsTr("PocketBook BMP-BARCH")

    Text {
        id: workingDirectoryText
        font.bold: true
        text: workingDirectory
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.leftMargin: 5
        anchors.topMargin: 5
    }
    ComboBox {
        id: extensionComboBox

        anchors.top: workingDirectoryText.bottom
        anchors.left: workingDirectoryText.left
        anchors.topMargin: 5

        model: ["All", "bmp", "png", "barch"]
        onCurrentTextChanged: {
            var currentText = extensionComboBox.currentText
            proxyModel.filterPattern = currentText === "All" ? "" : currentText
        }
    }

    ListView {
        model: proxyModel
        width: parent.width
        height: parent.height-workingDirectoryText.height-extensionComboBox.height

        anchors.top: extensionComboBox.bottom
        anchors.left: extensionComboBox.left
        anchors.leftMargin: 5
        anchors.topMargin: 15

        delegate: Item {
            width: ListView.view.width
            height: 20

            Rectangle {
                id: background
                width: parent.width
                height: parent.height
                color: "white"

                Row {
                    spacing: 5

                    Text {
                        text: model.file_name
                        font.bold: true
                    }

                    Text {
                        text: "| " + model.file_size
                    }

                    Text {
                        text: "| " + model.created_time
                    }

                    Text {
                        text: model.is_processing
                        font.weight: Font.Bold
                    }
                }

                MouseArea {
                    anchors.fill: parent
                    hoverEnabled: true

                    onEntered: {
                        background.state = 'hovered'
                    }

                    onExited: {
                        background.state = ''
                    }

                    onClicked: {
                        background.state = 'clicked'
                        var canHandle = proxyModel.canHandleItem(index)
                        if(canHandle === true){
                            proxyModel.handleItemClick(index)
                        } else {
                            errorDialogLoader.active = true
                            if (errorDialogLoader.item) {
                                errorDialogLoader.item.open()
                            }
                        }
                    }
                } // MouseArea

                states: [
                    State {
                        name: "hovered"
                        PropertyChanges {
                            target: background
                            color: "lightgrey"
                        }
                    },
                    State {
                        name: "clicked"
                        PropertyChanges {
                            target: background
                            color: "lightblue"
                        }
                    }
                ]

                transitions: [
                    Transition {
                        from: ""
                        to: "hovered"
                        ColorAnimation {
                            target: background
                            property: "color"
                            duration: 200
                        }
                    },
                    Transition {
                        from: "hovered"
                        to: ""
                        ColorAnimation {
                            target: background
                            property: "color"
                            duration: 200
                        }
                    },
                    Transition {
                        from: "hovered"
                        to: "clicked"
                        ColorAnimation {
                            target: background
                            property: "color"
                            duration: 50
                        }
                    }
                ] // transitions
            } // Rectangle
        } // delegate
    } // ListView

    Loader {
        id: errorDialogLoader
        source: "error_dialog.qml"
        active: false
        onLoaded: {
            errorDialogLoader.item.open()
        }
    }
}
