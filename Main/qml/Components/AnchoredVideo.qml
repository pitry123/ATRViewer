
import QtQuick.Controls 2.0
import QtQuick 2.0
import atrViewer 1.0
import QtQuick.Layouts 1.12

import "../Controls" as Controls


Item
{
    id: anchoredVideo

    property var videoChannel: null

    property alias fillMode: videoItem.fillMode
    property alias hasVideo: videoItem.hasVideo

    property var atrTrackingBbList: null
    property var vmdTrackingBbList: null
    property var trackTrackingBbList: null
    property var trackObjBBox:null
    property var visibleCheckBox: true

    property alias listVideos: videoSwitchId.model

    property int channelNumber: -1

    property alias fullscreen: fullScreenVideoId.is_fullscreen



    Controls.VideoItem
    {
        id: videoItem
        anchors.fill: parent
        videoChannel: parent.videoChannel
        clip: true
    }




   Rectangle
    {
        id: radarImId
        width: 25
        height: 25
        color: "transparent"
        border.color: "#fdc9c9"
        border.width: 1
        radius: 4
        anchors.right: anchoredVideo.right
        property bool radar_en: false

        AnimatedImage
        {
            source: "qrc:/Images/radar_gif.gif"
            anchors.fill: parent
            z:-1
            MouseArea
            {
                anchors.fill: parent
                onClicked:
                {
                    if( radarImId.radar_en === false)
                    {
                        radarImId.radar_en = true;
                    }
                    else
                    {
                        radarImId.radar_en = false;
                    }
                }
            }
        }
    }

    Rectangle
    {
        id: recordVideoRectId
        anchors.right: radarImId.left
        anchors.rightMargin: 3
        width: 25
        height: 25
        color: "transparent"
        border.color: "#fdc9c9"
        border.width: 1
        radius: 4


        Behavior on radius
        {
            NumberAnimation
            {
                easing.type: Easing.InOutQuad
                duration: 500
            }
        }

        SequentialAnimation on color
        {
            id: colorLoop
            loops: Animation.Infinite
            running: false
            ColorAnimation { from: "transparent"; to: "#ff7e7e"; duration: 500 }
            ColorAnimation { from: "#ff7e7e"; to: "transparent"; duration: 500 }
        }


        Image
        {
            anchors.fill: parent
            source: "qrc:/Images/recordBtn.png"
        }

        MouseArea
        {
            anchors.fill: parent
            hoverEnabled: true
            onEntered:
            {
                parent.radius = 50

            }
            onClicked:
            {

                if(videoChannel !== null)
                {
                    console.log("name   ==" + videoChannel.name)
                    if (colorLoop.running === false)
                    {
                        var succ = guiConnector.process_record.startRecord(videoChannel.name)
                        if (succ === true)
                        {
                            colorLoop.running = true
                        }
                        else
                        {
                            colorLoop.running = false
                        }
                    }
                    else
                    {
                        var succ = guiConnector.process_record.endRecord()
                        colorLoop.running = false

                    }
                }
            }

            onExited:
            {
                if (colorLoop.running ===  true)
                {
                    parent.radius = 50

                }
                else
                {
                    parent.radius = 4
                }
            }
        }
    }

    ColumnLayout
    {
        Layout.fillWidth: false
        Layout.fillHeight: false

        x:
        {
            if(anchoredVideo.fullscreen === true)
            {
                return 0;
            }
            else
            {
                videoItem.contentRect.x
            }
        }
        y: 0

        Controls.CustomComboBox
        {
            id: videoSwitchId

            Layout.fillWidth: false
            Layout.fillHeight: false

//            x: videoItem.contentRect.x
//            y: videoItem.contentRect.y

            Layout.preferredHeight: 30
            Layout.preferredWidth:  250

            onActivated:
            {
              //  console.log("onActivated")
               // console.log(currentValue)
                guiConnector.videos.changeChannelInuput(channelNumber, currentValue)
               // console.log("onDone")
            }
        }

        RowLayout
        {

            ColumnLayout
            {
                Layout.fillWidth: false
                Layout.fillHeight: false

                Controls.CustomCheckBox
                {
                    id: showATR
                    text: qsTr("ATR")
                    checked: true
                    font.bold: true
                    visible: visibleCheckBox

                    onCheckedChanged:
                    {
                        if (checked === true)
                            atrTrackingManager.visible = true
                        else
                            atrTrackingManager.visible = false
                    }
                }

                Controls.CustomCheckBox
                {
                    id: showVMD
                    text: qsTr("VMD")
                    checked: true
                    font.bold: true
                    visible: visibleCheckBox

                    onCheckedChanged:
                    {
                        if (checked === true)
                            vmdTrackingManager.visible = true
                        else
                            vmdTrackingManager.visible = false

                    }
                }

                Controls.CustomCheckBox
                {
                    id: showTracker
                    text: qsTr("Tracker")
                    checked: true
                    font.bold: true
                    visible: visibleCheckBox

                    onCheckedChanged:
                    {
                        if (checked === true)
                            trackTrackingManager.visible = true
                        else
                            trackTrackingManager.visible = false

                    }
                }
                }
            ColumnLayout
            {
                Layout.fillWidth: false
                Layout.fillHeight: true

                Controls.CustomCheckBox
                {
                    id: showMan
                    text: qsTr("Person")
                    checked: true
                    font.bold: true
                    visible: visibleCheckBox

                    onCheckedChanged:
                    {
                        if (checked === true)
                        {
                            trackTrackingManager.personDisplay = true;
                            vmdTrackingManager.personDisplay = true;
                            atrTrackingManager.personDisplay = true;

                        }
                        else
                        {
                            trackTrackingManager.personDisplay = false;
                            vmdTrackingManager.personDisplay = false;
                            atrTrackingManager.personDisplay = false;
                        }
                    }
                }

                Controls.CustomCheckBox
                {
                    id: showVehicle
                    text: qsTr("Vehicle")
                    checked: true
                    font.bold: true
                    visible: visibleCheckBox

                    onCheckedChanged:
                    {
                        if (checked === true)
                        {
                            trackTrackingManager.vehicleDisplay = true;
                            vmdTrackingManager.vehicleDisplay = true;
                            atrTrackingManager.vehicleDisplay = true;

                        }
                        else
                        {
                            trackTrackingManager.vehicleDisplay = false;
                            vmdTrackingManager.vehicleDisplay = false;
                            atrTrackingManager.vehicleDisplay = false;
                        }
                    }
                }
                Item
                {
                    Layout.fillWidth: false
                    Layout.fillHeight: true
                }
            }

            }


    }

    Item {
        id: radarItemId
        width: 350
        height: 350
        x : videoItem.contentRect.x + (videoItem.width/5)
        y: root.height - radarItemId.height - 30
        visible: radarImId.radar_en
        Radar
        {
            showDetection:  showATR.checked
            trackDistListModel: atrTrackingBbList
            cam_name:
            {
                if(videoChannel !== null)
                {
                    return videoChannel.name;
                }
                return "no signal";
            }
        }
    }

    TrackingBBoxManager
    {
        id: atrTrackingManager

        anchors.fill: parent
        actualVideoWidth: videoItem.contentRect.width
        actualVideoHeight: videoItem.contentRect.height

        actualVideoX: videoItem.contentRect.x
        actualVideoY: videoItem.contentRect.y

        trackingBbListModel: atrTrackingBbList
    }

    TrackingBBoxManager
    {
        id: vmdTrackingManager

        anchors.fill: parent
        actualVideoWidth: videoItem.contentRect.width
        actualVideoHeight: videoItem.contentRect.height

        actualVideoX: videoItem.contentRect.x
        actualVideoY: videoItem.contentRect.y

        trackingBbListModel: vmdTrackingBbList
    }
    TrackingBBoxManager
    {
        id: trackTrackingManager

        anchors.fill: parent
        actualVideoWidth: videoItem.contentRect.width
        actualVideoHeight: videoItem.contentRect.height

        actualVideoX: videoItem.contentRect.x
        actualVideoY: videoItem.contentRect.y

        trackingBbListModel: trackTrackingBbList
    }

    Rectangle
    {
        id: trackObjPaintingId
        width: 0
        height: 0
        x: 0
        y: 0
        border.color: "#ff7e7e"
        border.width: 2
        color: "transparent"
    }

    onTrackObjBBoxChanged:
    {
        console.log(" sts track obj");
        trackObjPaintingId.x =  videoItem.contentRect.x + parseInt(trackObjBBox.x * videoItem.contentRect.width)
        trackObjPaintingId.y = videoItem.contentRect.y + parseInt(trackObjBBox.y * videoItem.contentRect.height)
        trackObjPaintingId.height = trackObjBBox.height * videoItem.contentRect.height
        trackObjPaintingId.width = trackObjBBox.width * videoItem.contentRect.width
        trackObjPaintingId.border.color = trackObjBBox.color
    }

    MouseArea
    {
        width:(videoItem.no_signal)?0:videoItem.contentRect.width
        height:(videoItem.no_signal)?0: videoItem.contentRect.height
        x: (videoItem.no_signal)?0:videoItem.contentRect.x
        y: (videoItem.no_signal)?0:videoItem.contentRect.y

        enabled: !videoItem.no_signal

        property variant clickPos: "1,1"
        acceptedButtons: Qt.LeftButton | Qt.RightButton


        onPressed: {
            clickPos  = Qt.point(mouse.x,mouse.y)
            trackObjPaintingId.x = clickPos.x + x
            trackObjPaintingId.y = clickPos.y + y
        }

        onPositionChanged: {
            var delta = Qt.point(mouse.x - clickPos.x, mouse.y - clickPos.y)
       //     console.log("delta x = " + delta.x)
       //     console.log("rec x = " + trackObjPaintingId.x)
            trackObjPaintingId.width = Math.abs(delta.x)
            trackObjPaintingId.height =  Math.abs(delta.y)
        }

        onReleased:
        {
            var x_track = (trackObjPaintingId.x - videoItem.contentRect.x) / videoItem.contentRect.width
            var y_track = (trackObjPaintingId.y - videoItem.contentRect.y) / videoItem.contentRect.height
            var w_track = trackObjPaintingId.width / videoItem.contentRect.width
            var h_track = trackObjPaintingId.height / videoItem.contentRect.height
            var v_id = guiConnector.videos.get_video_resource_id(videoChannel.name)
            guiConnector.tracking.track_request(v_id, x_track, y_track, w_track, h_track);
            trackObjPaintingId.width = 0
            trackObjPaintingId.height = 0
        }
        onClicked:
        {
            if (mouse.button == Qt.RightButton)
            {
                console.log("stop track")
                var v_id = guiConnector.videos.get_video_resource_id(videoChannel.name)
                guiConnector.tracking.stop_tracking(v_id);
                trackObjPaintingId.width = 0
                trackObjPaintingId.height = 0
                return;
            }
        }
    }

    Image
    {
        property bool is_fullscreen: false
        id: fullScreenVideoId
        source: "qrc:/Images/fullScreen.png"
        width: 25
        height: 25
        visible: !videoItem.no_signal
        x: videoItem.contentRect.x
        y: videoItem.contentRect.y
        anchors.right:
        {
            if(fullscreen === true)
            {
                return recordVideoRectId.left;
            }
            else
            {
                return anchoredVideo.right;
            }
        }
        anchors.rightMargin: 5
        anchors.topMargin: 5
        MouseArea
        {
            anchors.fill: parent
            onClicked:
            {
                if( fullScreenVideoId.is_fullscreen === false)
                {
                    fullScreenVideoId.is_fullscreen = true;
                    fullScreenVideoId.source =  "qrc:/Images/normalScreen.png"
                }
                else
                {
                    fullScreenVideoId.is_fullscreen = false;
                    fullScreenVideoId.source =  "qrc:/Images/fullScreen.png"
                }
            }
        }

    }

}
