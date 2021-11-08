import QtQuick 2.0


Rectangle {
    id: toggleswitch
    anchors.fill: parent
    color: "transparent"


//====== property ======

    property real knobWidth: parent.width / 3//rootMainScreen.width/68.57
    property real knobHeight: parent.height / 1.3157//rootMainScreen.width/101.05
    property real xStartKnob:  parent.width / 20// 3
    property real yStartKnob:  parent.height / 8.333 //3
    property real xEndKnob:  parent.width -  (parent.width / 20) - knobWidth//29

    property bool on: false
    state: "off"

    function toggle() {
        if (toggleswitch.state == "on")
            toggleswitch.state = "off";
        else
            toggleswitch.state = "on";
    }

    function releaseSwitch() {
        if (knob.x == xStartKnob) {
            if (toggleswitch.state == "off") return;
        }
        if (knob.x == xEndKnob) {
            if (toggleswitch.state == "on") return;
        }
        toggle();
    }




    Rectangle
    {
        id: bgToggleId

        color: (toggleswitch.state == "on")?"#deb887":"black"
        radius: 25
        anchors.fill: parent

        Text {
            text: qsTr("On")
            anchors.left: parent.left
            color: "#fbfafa"
            font.bold: true
            font.pixelSize: 10
            anchors.leftMargin: 4
            y: 5
        }

        Text {
            text: qsTr("Off")
            anchors.right: parent.right
            font.pixelSize: 10
            color: "#fbfafa"
            verticalAlignment: Text.AlignVCenter
            font.bold: true
            anchors.rightMargin: 4
            y: 5

        }


        MouseArea
        {
            anchors.fill: parent
            onClicked: toggle()
        }
    }


    Rectangle {
        id: knob
        width: knobWidth
        height: knobHeight
        color: "#eaeee7"
        radius: 20
        y:yStartKnob
        x:xStartKnob


        MouseArea {
            anchors.fill: parent
            drag.target: knob; drag.axis: Drag.XAxis; drag.minimumX: xStartKnob; drag.maximumX: xEndKnob
            onClicked: toggle()
            onReleased: releaseSwitch()
        }
    }


    states: [
        State {
            name: "on"
            PropertyChanges { target: knob; x: xEndKnob; color: "#eaeee7"}
            PropertyChanges { target: toggleswitch; on: true }
        },
        State {
            name: "off"
            PropertyChanges { target: knob; x: xStartKnob; color: "#eaeee7"}
            PropertyChanges { target: toggleswitch; on: false }
        }
    ]

    transitions: Transition {
        NumberAnimation { properties: "x"; easing.type: Easing.InOutQuad; duration: 200 }
    }
}
