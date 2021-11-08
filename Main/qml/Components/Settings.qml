import QtQuick 2.0
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12
import QtQuick.Dialogs 1.2
import "../Controls" as Controls

Popup
{
    id: popupId
    property alias listVideos: videoSwitchSettingId.model
    property color bgColor : "black"
    property color frColor : "#deb887"

    Item
    {
        property bool atrAlgoEn: guiConnector.algo_settings.atr_algo_en
        property bool vmdAlgoEn: guiConnector.algo_settings.vmd_algo_en
        property var trackAlgoEn: guiConnector.algo_settings.track_algo_en

        property int atrAlgoPd: guiConnector.algo_settings.atr_algo_pd
        property int vmdAlgoPd: guiConnector.algo_settings.vmd_algo_pd
        property int trackAlgoPd: guiConnector.algo_settings.track_algo_pd

        onAtrAlgoEnChanged:
        {
            toggleAtr.state = (guiConnector.algo_settings.atr_algo_en === true)? "on":"off";
        }

        onVmdAlgoEnChanged:
        {
            toggleVMD.state = (guiConnector.algo_settings.vmd_algo_en === true)? "on":"off";
        }

        onTrackAlgoEnChanged:
        {
            toggleTrack.state = (guiConnector.algo_settings.track_algo_en === true)? "on":"off";
        }

        onAtrAlgoPdChanged:
        {
            atrPdSlider.value = atrAlgoPd
        }

        onVmdAlgoPdChanged:
        {
            vmdPdSlider.value = vmdAlgoPd;
        }

        onTrackAlgoPdChanged:
        {
            trackPdSlider.value = trackAlgoPd
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
                    popupId.x += delta.x;
                    popupId.y += delta.y;
                }
            }
    }

    ColumnLayout
    {
        anchors.fill: parent

        spacing: 5
        Controls.CustomComboBox
        {
            id: videoSwitchSettingId
            Layout.fillWidth: false
            Layout.fillHeight: false
            Layout.preferredHeight: 30
            Layout.preferredWidth:  250

            onActivated:
            {
                guiConnector.algo_settings.changeVideo(currentValue);
                toggleAtr.state = (guiConnector.algo_settings.atr_algo_en === true)? "on":"off";
                toggleVMD.state = (guiConnector.algo_settings.vmd_algo_en === true)? "on":"off";
                toggleTrack.state = (guiConnector.algo_settings.track_algo_en === true)? "on":"off";
            }
        }

        Item
        {
            Layout.fillWidth: true
            Layout.fillHeight: false
            Layout.preferredHeight: 15
        }

        RowLayout
        {

            Layout.leftMargin: 8
            Text {
                font.pixelSize: 15
                color: "#deb887"
                text: qsTr("ATR:      ")
            }

            Rectangle
            {
                Layout.fillWidth: false
                Layout.fillHeight: false
                radius: 8
                Layout.preferredHeight: 25
                Layout.preferredWidth:70
                color: "transparent"

                Controls.CustomToggleBotton
                {
                    id: toggleAtr
                    visible: true

                    onOnChanged:
                    {
                        guiConnector.algo_settings.changeAlgoState("ATR",toggleAtr.on)
                    }

                 }
            }
        }

        RowLayout
        {

            Layout.leftMargin: 8
            Text {
                font.pixelSize: 15
                color: "#deb887"
                text: qsTr("ATR PD: ")
            }

            Item
            {
                Layout.fillWidth: true
                Layout.fillHeight: false
                Layout.preferredHeight: 25
                Controls.CustomSlider
                {
                    id: atrPdSlider
                    color: "transparent"
                    anchors.fill: parent
                    onValueChanged:
                    {
                        guiConnector.algo_settings.changePdValue("ATR",value)
                    }
                }
            }
        }


        RowLayout
        {

            Layout.leftMargin: 8
            Text {
                font.pixelSize: 15
                color: "#deb887"
                text: qsTr("VMD:     ")
            }

            Rectangle
            {
                Layout.fillWidth: false
                Layout.fillHeight: false
                radius: 8
                Layout.preferredHeight: 25
                Layout.preferredWidth:70
                color: "transparent"

                Controls.CustomToggleBotton
                {
                    id: toggleVMD
                    visible: true

                    onOnChanged:
                    {
                        guiConnector.algo_settings.changeAlgoState("VMD",toggleVMD.on)
                    }

                 }
            }
        }

        RowLayout
        {

            Layout.leftMargin: 8
            Text {
                font.pixelSize: 15
                color: "#deb887"
                text: qsTr("VMD PD: ")
            }

            Item
            {
                Layout.fillWidth: true
                Layout.fillHeight: false
                Layout.preferredHeight: 25
                Controls.CustomSlider
                {
                    id: vmdPdSlider
                    color: "transparent"
                    anchors.fill: parent
                    onValueChanged:
                    {
                        guiConnector.algo_settings.changePdValue("VMD",value)
                    }
                }
            }
        }

        RowLayout
        {

            Layout.leftMargin: 8
            Text {
                font.pixelSize: 15
                color: "#deb887"
                text: qsTr("Track:   ")
            }

            Rectangle
            {
                Layout.fillWidth: false
                Layout.fillHeight: false
                radius: 8
                Layout.preferredHeight: 25
                Layout.preferredWidth:70
                color: "transparent"

                Controls.CustomToggleBotton
                {
                    id: toggleTrack
                    visible: true

                    onOnChanged:
                    {
                        guiConnector.algo_settings.changeAlgoState("Track",toggleTrack.on)
                    }

                 }
            }
        }

        RowLayout
        {

            Layout.leftMargin: 8
            Text {
                font.pixelSize: 15
                color: "#deb887"
                text: qsTr("Track PD: ")
            }

            Item
            {
                Layout.fillWidth: true
                Layout.fillHeight: false
                Layout.preferredHeight: 25
                Controls.CustomSlider
                {
                    id: trackPdSlider
                    color: "transparent"
                    anchors.fill: parent
                    onValueChanged:
                    {
                        guiConnector.algo_settings.changePdValue("Track",value)
                    }
                }
            }
        }

        RowLayout
        {
            Layout.fillWidth: true
            Layout.fillHeight: false
            spacing: 5

            Rectangle
            {
                color: /*parent.parent.highlighted ?"#949494" :*/"black"
                radius: 6
                Layout.preferredHeight: 30
                Layout.fillWidth: true

                Text
                {
                    id: sdtxtId
                    anchors.fill: parent
                    text: qsTr("Save As Default")
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
                        guiConnector.algo_settings.saveAsDefault()
                    }

                    onPressed:
                    {
                        sdtxtId.color = bgColor
                        parent.color = frColor
                    }

                    onReleased:
                    {
                        sdtxtId.color = frColor
                        parent.color = bgColor
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
                    id: stdtxtId
                    anchors.fill: parent
                    text: qsTr("Load Default")
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
                        guiConnector.algo_settings.setDefault()

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




//            Item
//            {
//                Layout.fillWidth: true
//                Layout.fillHeight: false

//            }



        }

        Item
        {
            Layout.fillWidth: true
            Layout.fillHeight: true

        }

    }







}
