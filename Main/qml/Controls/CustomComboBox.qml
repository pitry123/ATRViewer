import QtQuick 2.12
import QtQuick.Controls 2.12

ComboBox {
    id: control

    font.pixelSize: 12
    font.bold: true

    property var imgSrc: null

    delegate: ItemDelegate {
        width: control.width
        Rectangle
        {
            anchors.fill: parent
            color: "black"
            Rectangle
            {
                anchors.fill: parent
                color: parent.parent.highlighted ?"#949494" :"black"
                radius: 6

                Text
                {
                    anchors.fill: parent
                    text: control.textRole ? (Array.isArray(control.model) ? modelData[control.textRole] : model[control.textRole]) : modelData
                    color: "#deb887"
                    font: control.font
                    elide: Text.ElideRight
                    verticalAlignment: Text.AlignVCenter

                    leftPadding: 30
                   // topPadding: 10
                }
            }
        }
        highlighted: control.highlightedIndex === index
    }

    indicator: Image {
        id: name
        x: control.x + 7
        y: control.y
        width: 30
        height: 30
        source:
        {
            if(imgSrc === null)
            {
                return "qrc:/Images/video_camera.svg";
            }
            return imgSrc;
        }
    }



    contentItem: Text
    {
        leftPadding: control.indicator.width + control.indicator.x + 5
        width: control.width
        height: control.height
        text: control.displayText
        font: control.font
        color: control.pressed ? "#17a81a" : "#deb887"
        verticalAlignment: Text.AlignVCenter
        elide: Text.ElideRight
    }

    background: Rectangle
    {
        implicitWidth: 120
        implicitHeight: 40
        color: "transparent"
        radius: 6
    }

    popup: Popup
    {
        y: control.height - 1
        width: control.width
        implicitHeight: contentItem.implicitHeight
        padding: 1

        contentItem: ListView {
            clip: true
            implicitHeight: contentHeight
            model: control.popup.visible ? control.delegateModel : null
            currentIndex: control.highlightedIndex


            Rectangle {

                width: parent.width
                height: parent.height
                color: "transparent"
                border.color: "#deb887"
                radius: 6

            }

            ScrollIndicator.vertical: ScrollIndicator { }
        }


        background:Rectangle
        {
            width: parent.width
            height: parent.height
            color: "black"
            radius: 6


        }

    }
}
