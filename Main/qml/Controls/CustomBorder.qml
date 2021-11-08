import QtQuick 2.12

Rectangle
{

    width: parent.width
    height: parent.height
    property bool commonBorder : true

    property int lBorderwidth : 1
    property int rBorderwidth : 1
    property int tBorderwidth : 1
    property int bBorderwidth : 1

    property int commonBorderWidth : 5

    z : -8

    property string borderColor : "white"

    color: borderColor

    anchors
    {
        left: parent.left
        right: parent.right
        top: parent.top
        bottom: parent.bottom

        topMargin    : commonBorder ? -commonBorderWidth : -tBorderwidth
        bottomMargin : commonBorder ? -commonBorderWidth : -bBorderwidth
        leftMargin   : commonBorder ? -commonBorderWidth : -lBorderwidth
        rightMargin  : commonBorder ? -commonBorderWidth : -rBorderwidth
    }
}
