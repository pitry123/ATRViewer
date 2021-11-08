import QtQuick 2.4
import QtQuick.Controls 2.4

Item
{
    id: tracking_bbox
    property var bbox: null
    property var bbox_name: ""
    property var color: "#FFFFFF"
    property var border_width: 2
    visible:  true

    property var videoWidth: 0
    property var videoHeight: 0
    property var videoX: 0
    property var videoY: 0

    property bool showId: false
    property var trId: 0

    Rectangle
    {
        id: bbDrawId
        x: videoX + parseInt(tracking_bbox.bbox.x * videoWidth)
        y: videoY + parseInt(tracking_bbox.bbox.y * videoHeight)
        height: tracking_bbox.bbox.height * videoHeight
        width: tracking_bbox.bbox.width * videoWidth
        border.color: tracking_bbox.color
        border.width: tracking_bbox.border_width
        color: "transparent"

        Text
        {
            text: bbox_name
            font.pixelSize: 18
            anchors.top: parent.bottom
            anchors.left: parent.left
            color: tracking_bbox.color
        }
    }

//    Rectangle
//    {
//        visible: showId
//        //x: videoX + parseInt(tracking_bbox.bbox.x * videoWidth)
//        //y: videoY + parseInt(tracking_bbox.bbox.y * videoHeight)
//        height:bbDrawId.height * (0.2)
//        width: bbDrawId.width  * (0.5)
//        color: tracking_bbox.color
//        border.color: tracking_bbox.color
//        anchors.bottom: bbDrawId.top
//        anchors.left: bbDrawId.left
//        Text {
//            visible: showId
//            anchors.centerIn: parent.Center
//            //fontSizeMode: Text.Fit
//            font.pixelSize: parent.width * 0.8
//            font.bold: true
//            horizontalAlignment: Text.AlignHCenter
//            verticalAlignment: Text.AlignVCenter
//            text: "121"//toString(tracking_bbox.idTrack)
//        }
//    }

    Label
    {
        visible: showId
        padding: 0
        text: trId
        font.pixelSize: 10
        font.bold: true
        anchors.bottom: bbDrawId.top
        anchors.left: bbDrawId.left
        background: Rectangle
        {
            color: tracking_bbox.color
        }
    }


}
