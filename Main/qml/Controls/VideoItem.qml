import QtQuick 2.0
import QtMultimedia 5.5
import elbit.qml.video 1.0

Item
{
    id: videoItem
    property var videoChannel: null
    property alias contentRect: videoOutput.contentRect
    property alias fillMode: videoOutput.fillMode
    property bool hasVideo: (no_signal_rect.visible === false)

    property alias no_signal: no_signal_rect.visible

    VideoSubscriberItem
    {
        id: video_subscriber
        property var videoChannel: videoItem.videoChannel
        videoName:
        {
            if (parent.videoChannel === null || parent.videoChannel === "undefined")
                return ""

            console.log("change the name -- " + parent.videoChannel.name)

            return parent.videoChannel.name
        }

    }

    VideoOutput
    {
        id: videoOutput
        anchors.fill: parent
        source: video_subscriber

        fillMode: VideoOutput.PreserveAspectFit
    }

    Rectangle
    {
        id: no_signal_rect
        anchors.fill: parent
        color: "#FF000000"
        visible: (video_subscriber === null || video_subscriber === "undefined") ?  true : video_subscriber.videoTimeout

        Text
        {
            text: qsTr("No Video")
            font.pixelSize: 70
            color: "#ffffff";
            font.family:"Arial"
            anchors.centerIn: parent
        }
    }
}
