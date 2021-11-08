import QtQuick 2.7
import QtQuick.Layouts 1.1
import QtQuick.Window 2.2

Rectangle {
    id: slider
    anchors.fill: parent

//===== property ======
    property int value: 0
    //onValueChanged: updatePos();
    property real maximum: 1
    property real minimum: 1
    property double lineIdX: 3
    property int xMax: bgSlider.width - handle.width - lineIdX + 4
    property real valueNormal: (xMax - xMin)
    property int xMin: lineIdX

    property color green_color: "#deb887"



function clickSlider(posMouse)
{
    if(handle.x > posMouse)
    {
       if(value != 0)
          value -= 1;
    }
    else
    {
        if(value != 100)
            value += 1;
    }
    handle.x = ((value * valueNormal) / 100) + xMin;
}

    Rectangle {
        id: bgSlider
        anchors.fill: parent
        color: "transparent"

        Rectangle
        {
            id: lineId
            color: green_color
            anchors.verticalCenter: parent.verticalCenter
            width: parent.width
            x: lineIdX
            height: 1
        }

        Rectangle
        {
            id: ballStartId
            width: 7
            height: 7
            radius: 20
            color: green_color
            x: lineId.x - 3
            y: lineId.y - 3

        }

        Rectangle
        {
            id: ballEndId
            width: 7
            height: 7
            radius: 20
            color: green_color
            x: lineId.x  + lineId.width - 4
            y: lineId.y - 3

        }

        Text {
            id: valueSlider
            font.pixelSize: 10
            font.bold: true
            color: "#deb887"

            text:
            {
                return value.toLocaleString()
            }
            anchors.fill: parent
           // verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter
        }

      MouseArea { anchors.fill: parent; onClicked: clickSlider(mouseX) }
    }


    Rectangle {
        id: handle
        y: 6
        x: lineIdX
        width: 22
        height: 13
        radius: 2
        color: green_color


        Rectangle
        {
            anchors.right: parent.right
            anchors.rightMargin: 3
            anchors.verticalCenter:  parent.verticalCenter
            height: 7
            width: 1
            color: "#638185"
        }
        Rectangle
        {
            anchors.right: parent.right
            anchors.rightMargin: 5
            anchors.verticalCenter:  parent.verticalCenter
            height: 7
            width: 1
            color: "#638185"
        }

        MouseArea {
            id: mouse
            anchors.fill: parent; drag.target: parent
            drag.axis: Drag.XAxis; drag.minimumX: lineIdX; drag.maximumX: slider.xMax
            onPositionChanged:
            {
                value = ((handle.x - xMin)/(valueNormal))*100;
                if(value >= 100)
                    value = 100;
                if(value <= 0)
                    value = 0;
            }
        }
    }
}
