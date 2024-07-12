import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Dialog {
    id: errorDialog
    modal: true
    width: 300
    height: 200
    opacity: 0.9

    standardButtons: Dialog.NoButton

    Component.onCompleted: {
            errorDialog.x = (mainAppWindow.width - errorDialog.width) / 2
            errorDialog.y = (mainAppWindow.height - errorDialog.height) / 2
        }

    contentItem: ColumnLayout {
        width: parent.width
        height: parent.height

        // Header
        Rectangle {
            anchors.horizontalCenter: parent.horizontalCenter
            width: parent.width
            height: 30
            color: "#FA4141"
            Label {
                anchors.centerIn: parent
                text: "Error"
                font.family: "Helvetica"
                font.pixelSize: 24
                font.weight: Font.DemiBold
                color: "white"
            }
        }

        // Body text
        Label {
            Layout.alignment: Qt.AlignHCenter
            width: parent.width
            text: "Unsupported file format!"
            font.family: "Helvetica"
            font.pixelSize: 24
            font.weight: Font.DemiBold
            color: "#333333"
        }

        Rectangle {
            anchors.horizontalCenter: parent.horizontalCenter
            width: parent.width
            height: 30
            color: "#00000000"

            Button {
                id: okButton

                anchors.horizontalCenter: parent.horizontalCenter
                anchors.verticalCenter: parent.verticalCenter
                width: 70
                height: parent.height-2
                padding: 10

                text: "OK"
                font.family: "Helvetica"
                font.pixelSize: 14

                property int colorIndex: 0
                property var colors: ["#4CAF50", "#2196F3", "#FFC107", "#E91E63"]

                background: Rectangle {
                    color: "#4CAF50"  // (green)
                    radius: 5
                }

                contentItem: Text {
                    color: "white"
                    text: parent.text
                    verticalAlignment: Text.AlignVCenter
                    horizontalAlignment: Text.AlignHCenter
                }

                onClicked: {
                    colorIndex = (colorIndex + 1) % colors.length;  // cycle through colors
                    okButton.background.color = colors[colorIndex];  // update button color dynamically

                    errorDialog.close()
                }
                Layout.alignment: Qt.AlignCenter
            } // Button
        } // Rectangle
    } // ColumnLayout
}
