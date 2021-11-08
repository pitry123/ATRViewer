import QtQuick 2.12
import QtQuick.Controls 2.12

CheckBox
{
    id: control

    property color foregroundActive: "#FFFFFFFF"
    property color foregroundNormal: "#deb887"
   // property color textActive: "#000000"
    property color textActive: "#deb887"

    indicator: Rectangle
    {
        implicitWidth: 22
        implicitHeight: 20
        x: parent.leftPadding
        y: parent.height / 2 - height / 2
        radius: 5
        border.color: parent.down ? foregroundActive : foregroundNormal

        Rectangle {
            width: 14
            height: 12
            x: 4
            y: 4
            radius: 3
            color: parent.down ? foregroundActive : foregroundNormal
            visible: parent.parent.checked
        }
    }

    contentItem: Text
    {
        text: control.text
        font: control.font
        opacity: enabled ? 1.0 : 0.3
        color: textActive
        verticalAlignment: Text.AlignVCenter
        leftPadding: control.indicator.width + control.spacing
    }
}
