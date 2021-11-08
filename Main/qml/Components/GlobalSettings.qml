import QtQuick 2.0
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12
import QtQuick.Dialogs 1.2
import "../Controls" as Controls

Popup
{
    id: popupGlobalId
    property color bgColor : "black"
    property color frColor : "#deb887"


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
                    popupGlobalId.x += delta.x;
                    popupGlobalId.y += delta.y;
                }
            }
    }


    Text {
        id: headLine
        text: qsTr("System Settings")
        anchors.fill: parent
        horizontalAlignment: Text.AlignHCenter
        font.pixelSize: 25
        font.bold: true
        color: "#deb887"

    }

    ColumnLayout
    {

        height: 200
        anchors.fill: parent
        anchors.topMargin: 40



        spacing: 10


        RowLayout
        {
            Layout.fillWidth: true
            Layout.fillHeight: false
            spacing: 5

            Text
            {
                Layout.fillWidth: false
                Layout.fillHeight: true
                text: qsTr("AI Network: ")
                color: "#deb887"
                elide: Text.ElideRight
                verticalAlignment: Text.AlignVCenter
                font.bold: true
                // topPadding: 10
            }

            Controls.CusotomComboBox2
            {
                id: aiSwitchId
                Layout.fillHeight: true
                Layout.preferredHeight: 30
                Layout.fillWidth: true


                imgSrc: ""

                model: ["Select Netwoerk","Network_0", "Network_1", "Network_2"]

                onActivated:
                {
                    guiConnector.algo_settings.changeAINetwork(currentValue)
                }

            }
            Item
            {
                Layout.fillWidth: true
                Layout.fillHeight: true

            }

        }

        RowLayout
        {
            Layout.fillWidth: true
            Layout.fillHeight: false
            spacing: 5

            Text
            {
                Layout.fillWidth: false
                Layout.fillHeight: true
                text: qsTr("Energy Policy Mode: ")
                color: "#deb887"
                elide: Text.ElideRight
                verticalAlignment: Text.AlignVCenter
                font.bold: true
                // topPadding: 10
            }

            Controls.CusotomComboBox2
            {
                id: voltSwitchId
                Layout.fillHeight: true
                Layout.preferredHeight: 30
                Layout.fillWidth: true

                model: ["Select Mode","15W_2CORE", "15W_4CORE", "15W_6CORE", "10W_2CORE", "10W_4CORE"]

                onActivated:
                {
                    guiConnector.algo_settings.changeEnergyPolicy(currentValue)
                }

            }
            Item
            {
                Layout.fillWidth: true
                Layout.fillHeight: true

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
                        id: saveDeftxtId
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
                        }

                        onPressed:
                        {
                            saveDeftxtId.color = bgColor
                            parent.color = frColor
                        }

                        onReleased:
                        {
                            saveDeftxtId.color = frColor
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
                    id: setDeftxtId
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

                    }
                    onPressed:
                    {
                        setDeftxtId.color = bgColor
                        parent.color =   frColor
                    }

                    onReleased:
                    {
                        setDeftxtId.color = frColor
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
                    id: resetFactorytxtId
                    anchors.fill: parent
                    text: qsTr("Reset Factory")
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

                    }
                    onPressed:
                    {
                        resetFactorytxtId.color = bgColor
                        parent.color =   frColor
                    }

                    onReleased:
                    {
                        resetFactorytxtId.color = frColor
                        parent.color = bgColor
                    }
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
                id: systemRestarttxtId
                anchors.fill: parent
                text: qsTr("System Restart")
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
                    guiConnector.algo_settings.restartSystem()
                }
                onPressed:
                {
                    systemRestarttxtId.color = bgColor
                    parent.color =   frColor
                }

                onReleased:
                {
                    systemRestarttxtId.color = frColor
                    parent.color = bgColor
                }
            }
        }

        Item
        {
            Layout.fillWidth: true
            Layout.fillHeight: true

        }

    }






}
