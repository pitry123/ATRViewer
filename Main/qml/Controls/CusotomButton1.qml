import QtQuick 2.12
import QtQuick.Controls 2.12


Item {
    property color bgColor: "black"
    property color frColor: "#deb887"
    property var txtBtn: ""
    signal event()

    Rectangle
    {
        color: "black"
        radius: 6
        anchors.fill: parent
        Text
        {
            id: stdtxtId
            anchors.fill: parent
            text: txtBtn
            color: "#deb887"
            elide: Text.ElideRight
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter
            font.bold: true
        }

        MouseArea
        {
            anchors.fill:parent

            onClicked:
            {
                event()
            }
            onPressed:
            {
                stdtxtId.color = bgColor
                parent.color =   frColor
            }

            onReleased:
            {
                stdtxtId.color = frColor
                parent.color = bgColor
            }
        }
    }
}
