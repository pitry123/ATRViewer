import QtQuick 2.12
import QtQuick.Window 2.3
import QtQuick.Controls 2.0
import qmlReload 1.0
import atrViewer 1.0
import QtQuick.Layouts 1.12


import "./Layout" as Layouts
import "./Controls" as Controls
import "./Components" as Components

Window
{

    readonly property int screen_id: 0

    id: root
    title: "ATR Viewer"
    flags: Qt.Window | Qt.FramelessWindowHint
    visible: true

    width: 1000
    height:  1000

    // visibility: "FullScreen"
    visibility: "Windowed"

    x: 0
    y: 0

    color: "#ff000000"

    property string stTime: "00:00"


    property color bgBlackColor : "black"
    property color frBrownColor : "#deb887"



    MouseArea
    {
          id: movingMainWindow
          width: root.width
          height: topLineId.height
          x: 0
          y: 0
          property variant clickPos: "1,1"
          enabled: !maximizeId.isFullScreen
          onPressed:
          {
              clickPos  = Qt.point(mouse.x,mouse.y)
          }

          onPositionChanged:
          {
              var delta = Qt.point(mouse.x-clickPos.x, mouse.y-clickPos.y)
              root.x += delta.x;
              root.y += delta.y;
              if(maximizeId.isFullScreen === true)
              {
                  root.x = 0
                  root.y = 0
              }
          }

          onDoubleClicked:
          {
              if(maximizeId.isFullScreen === false)
              {
                  maximizeId.isFullScreen = true
                  root.visibility = "FullScreen"

              }
              else
              {
                  maximizeId.isFullScreen = false
                  root.visibility = "Windowed"
              }
          }
    }

    Item
    {
        id: rootItem
        anchors.fill: parent
        Timer
        {
            id:recordTimer
            interval: 50
            running: false
            repeat: true
            onTriggered: {
                if (false === guiConnector.screen_rec.record(rootItem, rootItem.width, rootItem.height))
                {
                    recordTimer.stop();
                    guiConnector.screen_rec.endRecord();
                }
            }
        }


        Components.Settings
        {
            id: settingsPopupId
            listVideos: guiConnector.videos.videoChannelList
            width: 300
            height: 400
            x:200
            y:200
            visible: false

            onListVideosChanged:
            {
                for(var i=1; i < listVideos.length; i++)
                {
                    console.log("video list settings:" + listVideos[i]);
                    var reId = guiConnector.videos.get_video_resource_id(listVideos[i]);
                    guiConnector.algo_settings.listNewVideo(listVideos[i], reId);
                }
            }
        }

        Components.GlobalSettings
        {
            id: globalSettingsPopupId
            width: 300
            height: 250
            x:200
            y:200
            visible: false
        }


        Components.ExportDataForm
        {
            id: exportDataId
            width: 600
            height: 300
            x:200
            y:200
            visible: false
        }

        Popup
        {
            id:menuBurgerId
            visible: false
            x:0
            y: menuImage.height + 2

            width: 200
            height: 170

            background: Rectangle
            {
                color: "#373737"
                border.color: "black"
                radius: 7
            }

            ColumnLayout
            {
                anchors.fill: parent
                Rectangle
                {
                    color: /*parent.parent.highlighted ?"#949494" :*/"black"
                    radius: 6
                    Layout.preferredHeight: 30
                    Layout.fillWidth: true


                    Text
                    {
                        id: txtAlgoSettings
                        anchors.fill: parent
                        text: qsTr("Algo Settings")
                        color: "#deb887"
                        elide: Text.ElideRight
                        verticalAlignment: Text.AlignVCenter
                        font.bold: true
                        leftPadding: 30
                        // topPadding: 10
                    }

                    MouseArea
                    {
                        anchors.fill: parent
                        onClicked:
                        {
                            settingsPopupId.visible = true
                        }

                        onPressed:
                        {
                            txtAlgoSettings.color = bgBlackColor
                            parent.color =   frBrownColor
                        }

                        onReleased:
                        {
                            txtAlgoSettings.color = frBrownColor
                            parent.color = bgBlackColor
                        }
                    }
                }

                Rectangle
                {
                    color: /*parent.parent.highlighted ?"#949494" :*/"black"
                    radius: 6
                    Layout.preferredHeight: 30
                    Layout.fillWidth: true


                    Text
                    {
                        id: systemSettingsId
                        anchors.fill: parent
                        text: qsTr("System Settings")
                        color: "#deb887"
                        elide: Text.ElideRight
                        verticalAlignment: Text.AlignVCenter
                        font.bold: true
                        leftPadding: 30
                        // topPadding: 10
                    }

                    MouseArea
                    {
                        anchors.fill: parent
                        onClicked:
                        {
                            globalSettingsPopupId.visible = true
                        }

                        onPressed:
                        {
                            systemSettingsId.color = bgBlackColor
                            parent.color =   frBrownColor
                        }

                        onReleased:
                        {
                            systemSettingsId.color = frBrownColor
                            parent.color = bgBlackColor
                        }
                    }
                }


                Rectangle
                {
                    color: "black"
                    radius: 6
                    Layout.preferredHeight: 30
                    Layout.fillWidth: true
                    Text
                    {
                        id: exportTxtid
                        anchors.fill: parent
                        text: qsTr("Export Playback")
                        color: "#deb887"
                        elide: Text.ElideRight
                        verticalAlignment: Text.AlignVCenter
                        font.bold: true
                        leftPadding: 30
                    }

                    MouseArea
                    {
                        anchors.fill: parent
                        onClicked:
                        {
                            exportDataId.visible = true
                        }

                        onPressed:
                        {
                            exportTxtid.color = bgBlackColor
                            parent.color =   frBrownColor
                        }

                        onReleased:
                        {
                            exportTxtid.color = frBrownColor
                            parent.color = bgBlackColor
                        }
                    }
                }


                Rectangle
                {
                    color: /*parent.parent.highlighted ?"#949494" :*/"black"
                    radius: 6
                    Layout.preferredHeight: 30
                    Layout.fillWidth: true


                    Text
                    {
                        id: exitTxtId
                        anchors.fill: parent
                        text: qsTr("Exit")
                        color: "#deb887"
                        elide: Text.ElideRight
                        verticalAlignment: Text.AlignVCenter
                        font.bold: true
                        leftPadding: 30
                    }

                    MouseArea
                    {
                        anchors.fill: parent
                        onClicked:
                        {
                            Qt.quit();
                        }

                        onPressed:
                        {
                            exitTxtId.color = bgBlackColor
                            parent.color =   frBrownColor
                        }

                        onReleased:
                        {
                            exitTxtId.color = frBrownColor
                            parent.color = bgBlackColor
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


        RowLayout
        {
            id: topLineId
            visible: true
            width: parent.width - 10
            anchors.top:  parent.top// | Qt.AlignRight
            x: 5
            anchors.rightMargin: 35
            anchors.topMargin:   5
            height: 25


            Rectangle
            {
                id: menuRec

                width: 25
                height: 25
                color: "#15000000"
                Image {
                    id: menuImage
                    anchors.fill: parent
                    source: "qrc:/Images/menu.png"
                    MouseArea
                    {
                        anchors.fill: parent
                        onClicked:
                        {
                            menuBurgerId.visible = true
                        }
                    }
                }
            }

            Text
            {
                id: appNameTxt

                Layout.fillWidth: false
                Layout.fillHeight: false

                //           Layout.alignment: Qt.AlignLeft

                horizontalAlignment: Text.AlignLeft
                verticalAlignment: Text.AlignTop

                color: "#b2ebf2"

                font.bold: true

                text:"ATR Viewer(ver 1.1.1)"
            }

            Item
            {
                Layout.fillWidth: true
                Layout.fillHeight: false
            }

            Text
            {
                id: timeNowTxt

                Layout.fillWidth: false
                Layout.fillHeight: false

                Layout.alignment: Qt.AlignLeft

                horizontalAlignment: Text.AlignLeft
                verticalAlignment: Text.AlignTop

                color: "#b2ebf2"

                font.bold: true

                text: ""
            }

            Item
            {
                Layout.fillWidth: true
                Layout.fillHeight: false
            }

            Image
            {
                Layout.alignment: Qt.AlignRight
                id: minmizeId
                source: "qrc:/Images/minimizeWindow.png"
                MouseArea
                {
                    anchors.fill: parent
                    onClicked:
                    {
                        root.showMinimized()
                    }
                }
            }
            Image
            {
                Layout.alignment: Qt.AlignRight
                id: maximizeId
                source: "qrc:/Images/lineIcon.png"
                property bool isFullScreen:
                {
                    if(root.visibility === "FullScreen")
                    {
                        return true;
                    }
                    return false
                }
                MouseArea
                {
                    anchors.fill: parent
                    onClicked:
                    {
                        maximizeId.isFullScreen = !maximizeId.isFullScreen
                        if(maximizeId.isFullScreen === true)
                        {
                            root.visibility = "FullScreen"
                        }
                        else
                        {
                            root.visibility = "Windowed"
                        }
                    }
                }
            }

            Image
            {
                id: closeWindowId
                source: "qrc:/Images/closeWindow.png"

                MouseArea
                {
                    anchors.fill: parent
                    onClicked:
                    {
                        Qt.quit();
                    }
                }
            }
        }

        Item
        {
            id: center_screen
            anchors.top: topLineId.bottom
            width: root.width
            height: root.height - topLineId.height - 30
            Layouts.ScreenBase
            {
                anchors.fill: parent
            }
        }

        Item
        {
            id: bottom_screen
            anchors.top: center_screen.bottom
            width: root.width
            height: 30

            Image {
                source:  "qrc:/Images/backTimerRec.svg"
                height: parent.height
                width: 180
                anchors.horizontalCenter: parent.horizontalCenter

                RowLayout
                {
                    anchors.fill: parent
                    Item {
                        Layout.fillHeight: true
                        Layout.fillWidth: true
                    }

                    Image
                    {
                        source: "qrc:/Images/recordBtn.png"
                        Layout.preferredWidth: 30
                        Layout.fillHeight: false
                        Layout.fillWidth: false

                        MouseArea
                        {
                            anchors.fill: parent
                            onClicked:
                            {
                                recTimerCnt.start()
                                recordTimer.start()
                            }
                        }
                    }
                    Rectangle
                    {
                        id : test2
                        Layout.fillHeight: true
                        Layout.fillWidth: true
                        color: "transparent"

                        Text {
                            id: textRecTime
                            anchors.centerIn: parent
                            text: "00:00"
                            Layout.preferredWidth: 80
                            color: "#b2ebf2"
                            font.bold: true
                            font.pixelSize: 12
                        }
                        Timer
                        {
                            id: recTimerCnt
                            interval: 1000
                            repeat: true

                            property int cnt_sec: 0
                            property int cnt_min: 0
                            onTriggered:
                            {
                                cnt_sec = cnt_sec + 1
                                if (cnt_sec === 60)
                                {
                                    cnt_min = cnt_min + 1
                                    cnt_sec = 0
                                }
                                var timeString =  ((cnt_min >= 10)? cnt_min.toString():"0"+ cnt_min.toString()) + ":" + ((cnt_sec >= 10)?cnt_sec.toString():"0"+cnt_sec.toString())
                                console.log(timeString)
                                textRecTime.text = timeString
                            }
                        }
                    }

                    Image
                    {
                        source: "qrc:/Images/stop.png"
                        Layout.preferredWidth: 20
                        Layout.preferredHeight: 20
                        Layout.fillHeight: false
                        Layout.fillWidth: false

                        MouseArea
                        {
                            anchors.fill: parent
                            onClicked:
                            {
                                recTimerCnt.stop()
                                recTimerCnt.cnt_min = 0
                                recTimerCnt.cnt_sec = 0
                                textRecTime.text = "00:00"
                                recordTimer.stop()
                                guiConnector.screen_rec.endRecord();
                            }
                        }
                    }


                    Item {
                        Layout.fillHeight: true
                        Layout.fillWidth: true
                    }




                }
            }
        }



        Timer {
            interval: 500; running: true; repeat: true
            onTriggered: timeNowTxt.text = (new Date().toLocaleTimeString(Qt.locale(),Locale.ShortFormat)) +
                         " | "  + (new Date().toLocaleDateString(Qt.locale(),Locale.ShortFormat))
        }

    }






    // Debugging helpers

    //    QmlReload
    //    {
    //        id: reloader
    //    }

    //    Button
    //    {
    //        x: center_screen.x + center_screen.width - width
    //        y: parent.height - 25
    //        width: 100
    //        height: 25
    //        text: "Reload!"

    //        onClicked:
    //        {
    //            reloader.reload()
    //        }
    //    }
}
