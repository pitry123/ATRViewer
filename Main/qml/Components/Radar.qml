import QtQuick 2.0
import QtQuick.Controls 2.4

Item
{
    id: itemRadar
    anchors.fill: parent
    property var trackDistListModel: null
    property color pointColor : "red"
    property color radarColor: "#7ec29c"
    property var cam_name: ""

    Rectangle
    {
        anchors.fill: parent
        color: "transparent"
        border.color: "#7ec29c"
        border.width: 3
        radius: 4

        Label
        {
            padding: 0
            text: itemRadar.cam_name
            font.pixelSize: 12
            font.bold: true
            anchors.top:  parent.top
            anchors.left: parent.left
            anchors.leftMargin: 4
            anchors.topMargin: 3
        }


          MouseArea {

                anchors.fill: parent
                property variant clickPos: "1,1"

                onPressed: {
                    clickPos  = Qt.point(mouse.x,mouse.y)
                }

                onPositionChanged: {
                    var delta = Qt.point(mouse.x-clickPos.x, mouse.y-clickPos.y)
                    radarItemId.x += delta.x;
                    radarItemId.y += delta.y;
                }
            }
    }


    property bool showDetection: true

    onTrackDistListModelChanged:
    {
        canvas_detection.requestPaint()
    }

    Canvas
    {
      id:canvasC
      anchors.fill: parent

      onPaint:{
          var ctx = getContext('2d');
          ctx.reset();
          var centreX = itemRadar.width / 2;
          var centreY = itemRadar.height / 2;



          var start_radius = 10
          var delta_radius = 8
          ctx.lineWidth = 3
          ctx.strokeStyle = itemRadar.radarColor;

          for(var i=1; i <= 5; i++)
          {
              ctx.beginPath();
              ctx.arc(centreX, centreY, (start_radius * (i * 3)) , 0, Math.PI * 2, true);
              ctx.stroke();
          }

          ctx.beginPath();
          ctx.lineWidth = 2;
          ctx.setLineDash([5, 5])
          ctx.moveTo(0, centreY)
          ctx.lineTo(itemRadar.width,centreY);
          ctx.stroke();

          ctx.beginPath();
          ctx.lineWidth = 2;
          ctx.setLineDash([5, 5])
          ctx.moveTo(centreX, 0)
          ctx.lineTo(centreX,itemRadar.height  );
          ctx.stroke();
      }
    }



    Canvas
    {
      id:canvas_detection
      anchors.fill: parent
      onPaint:
      {

          var ctx = getContext('2d');
          ctx.reset()
          ctx.stroke();
          if(showDetection === false)
          {
              return;
          }

          var centreX = itemRadar.width / 2;
          var centreY = itemRadar.height / 2;

          ctx.fillStyle = pointColor;
          if(trackDistListModel === null)
          {
              return;
          }

          //console.log("(trackDistListModel).length" + (trackDistListModel).length)
          for(var i=0; i < (trackDistListModel).length; i++)
          {
              var teta = (Math.PI / 4) - (trackDistListModel[i].x * (Math.PI / 2))
              var newX = centreX - (Math.sin(teta) * trackDistListModel[i].range * 1.5)
              //var newX =( trackDistListModel[i].x * parent.width/2)
              var newY = centreY - ((Math.cos(teta) * (trackDistListModel[i].range *1.5)))
              ctx.beginPath();
              ctx.arc(newX,newY , 3 , 0, Math.PI * 2, false);
             // console.log("i :" + i + "newX:"+ newX + "newY:"+newY + "  t:" +teta)

              ctx.fill();
          }


      }

    }


//    Image
//    {
//        id: radarIdBg
//        anchors.fill: parent
//        source: "qrc:/Images/radar_bg.png"

//    }
}
