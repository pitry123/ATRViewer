import QtQuick 2.0
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12
import QtQuick.Dialogs 1.2
import "../Controls" as Controls

Popup
{
    id: popupExportDataId
    property color bgColor : "black"
    property color frColor : "#deb887"


    Item {
        id: udpateExpItem
        property var playback_current_time: guiConnector.export_data.currentTime
        property var exportProcessPrecent: 0


        onPlayback_current_timeChanged:
        {
            exportProcessPrecent  = playback_current_time / guiConnector.export_data.totalTime;
        }
    }





    background: Rectangle
    {
          color: "#373737"
          border.color: "black"
          radius: 7

          MouseArea {
                id: fiterArea

                anchors.fill: parent
                property variant clickPos: "1,1"

                onPressed: {
                    clickPos  = Qt.point(mouse.x,mouse.y)
                }

                onPositionChanged: {
                    var delta = Qt.point(mouse.x-clickPos.x, mouse.y-clickPos.y)
                    popupExportDataId.x += delta.x;
                    popupExportDataId.y += delta.y;
                }
            }
    }



    Rectangle
    {
        anchors.right: parent.right
        width:closeWindowExpId.width
        height: closeWindowExpId.height
        color: "transparent"
        Image
        {

            id: closeWindowExpId
            source: "qrc:/Images/closeWindow.png"
        }
        MouseArea
        {
            anchors.fill: parent
            onClicked:
            {
                popupExportDataId.visible = false
            }
        }
    }


    Text {
        id: headLine
        text: qsTr("Export Playback")
        anchors.fill: parent
        horizontalAlignment: Text.AlignHCenter
        font.pixelSize: 25
        font.bold: true
        color: "#deb887"

    }

    ColumnLayout
    {


        anchors.fill: parent
        anchors.topMargin: 40
        spacing: 5

        Controls.CustomDialogSeletFile
        {
            id: selectRecordFileId
            Layout.fillHeight: false
            Layout.preferredHeight: 40
            Layout.fillWidth:  true
            txtInput: "Record File"
            txtTxtFieldInput: "Browse to select files"

        }

        Controls.CustomFolderDialogSelect
        {
            id: selectFolderExpId

            Layout.fillHeight: false
            Layout.fillWidth:  true
            txtInput: "Output Dir"
            txtTxtFieldInput: "Browse to select directory"
        }

        Item {
            Layout.fillHeight: false
            Layout.preferredHeight: 30
            Layout.fillWidth:  true
        }

        Rectangle
        {
            id: customProcessBarId
            Layout.fillWidth:  true
            height: 20
            radius: 10

            color: "#525555"
            opacity: 0.8
            border
            {
                width: 0.5
                color: "black"
            }

            Text {
                id: progressExportNum
                anchors.fill: parent
                verticalAlignment: Text.AlignVCenter
                horizontalAlignment: Text.AlignHCenter
                font.bold: true
                color: "#deb887"
                z:1


                text:
                {
                    var dis = parseInt((udpateExpItem.exportProcessPrecent) * 100.0);
                    if(isNaN(dis))
                    {
                        dis = 0
                    }
                    if(dis <= 100 && dis >= 98)
                    {
                        dis = 100;
                        actProcessBarID.width = parent.width
                    }

                    return (dis + "%")
                }
            }

            Rectangle
            {
                id: actProcessBarID
                width: (parent.width * (udpateExpItem.playback_current_time / guiConnector.export_data.totalTime))
                height: parent.height
                x: 0
                y: 0
                radius: 10

                Behavior on width
                {
                    NumberAnimation
                    {
                        easing.type: Easing.InOutQuad
                        duration: 200
                    }
                }

                border
                {
                    color: "transparent"
                }

                gradient: Gradient
                {
                    GradientStop {
                        position: 0.40;
                        color: "#625555";
                    }
                    GradientStop {
                        position: 0.98;
                        color: "#493e3e";
                    }
                }
            }
        }

        Item {
            Layout.fillHeight: false
            Layout.preferredHeight: 10
            Layout.fillWidth:  true
        }

        RowLayout
        {
            Layout.fillWidth: true
            Layout.fillHeight: false
            Layout.preferredHeight: 30

            spacing: 5
            Controls.CusotomButton1
            {
                Layout.fillWidth: true
                Layout.fillHeight: true
                txtBtn: "Export"
                onEvent:
                {
                     guiConnector.export_data.exportRecord(selectRecordFileId.file_path,selectFolderExpId.file_path + "\\")
                }
            }


            Controls.CusotomButton1
            {
                Layout.fillWidth: true
                Layout.fillHeight: true
                txtBtn: "Cancel"
                onEvent:
                {

                }
            }
        }
    }
}
