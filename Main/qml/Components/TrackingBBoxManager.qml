import QtQuick 2.4
import atrViewer 1.0
import QtQuick.Controls 2.12

Item
{
    id: trackManagerBbxId
    property var trackingBbListModel: null

    property int actualVideoWidth: 0
    property int actualVideoHeight: 0
    property int actualVideoX: 0
    property int actualVideoY: 0

    property bool personDisplay: true
    property bool vehicleDisplay: true

    Repeater
    {
        model: trackingBbListModel

        Item
        {
            TrackingBBox
            {
                bbox: modelData
                videoWidth: actualVideoWidth
                videoHeight: actualVideoHeight
                videoX: actualVideoX
                videoY: actualVideoY
                trId: modelData.idTrack
                visible:
                {
                    if(trackManagerBbxId.personDisplay === false)
                    {
                        if(modelData.classification === 1 || modelData.classification === 0 ||
                                modelData.classification === 6 || modelData.classification === 7)
                        {
                            return false;
                        }
                    }
                    if(trackManagerBbxId.vehicleDisplay === false)
                    {
                        if(modelData.classification === 2 || modelData.classification === 8)
                        {
                            return false;
                        }
                    }
                    return true;
                }

                showId:
                {
                    if (modelData.classification >= 6 && modelData.classification < 12)
                    {
                        return true;
                    }
                    return false;
                }
                bbox_name:
                {
                    if(modelData.classification === 1 || modelData.classification === 0)
                    {
                        return qsTr("Person")
                    }
                    else if(modelData.classification === 2 )
                    {
                        return qsTr("Vehicle")
                    }
                    else if(modelData.classification === 6 || modelData.classification === 7)
                    {
                        return qsTr("Track Person")
                    }
                    else if(modelData.classification === 8 )
                    {
                        return qsTr("Track Vehicle")
                    }
                    else if(modelData.classification === 12 )
                    {
                        return qsTr("VMD")
                    }
                    else
                    {
                        var s = toString( modelData.classification);
                        return qsTr("UNDEFINE" + s)
                    }
                }
                color:
                {
                    return modelData.color
                }

                border_width: 3
            }
        }
    }

}
