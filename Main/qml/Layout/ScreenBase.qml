import QtQuick 2.0
import QtQuick.Controls 2.3
import atrViewer 1.0
import QtQuick.Layouts 1.12


import "../Components" as Components

Item
{
    id: screen_base

    function returnToNormalMode(id0,id1,id2,id3)
    {
        id0.visible = guiConnector.videos.channel_0_active
        id1.visible = guiConnector.videos.channel_1_active
        id2.visible = guiConnector.videos.channel_2_active
        id3.visible = guiConnector.videos.channel_3_active
    }


    GridLayout
    {
        Layout.fillWidth: true
        Layout.fillHeight: true

        anchors.fill: parent
        property var videoList: null

        columns: 2
        rows: 2



        // Channel 0
        Components.AnchoredVideo
        {
            id: anchoredVideo0
            Layout.fillWidth: true
            Layout.fillHeight: true
            visible: false
            atrTrackingBbList: guiConnector.tracking.atrBoundingBoxCh0List
            vmdTrackingBbList: guiConnector.tracking.vmdBoundingBoxCh0List
            trackTrackingBbList: guiConnector.tracking.trackBoundingBoxCh0List
            trackObjBBox: guiConnector.tracking.trackedObjch1
            channelNumber: 0

            listVideos: guiConnector.videos.videoChannelList

            Component.onCompleted:
            {
               visible = guiConnector.videos.channel_0_active
            }

            onFullscreenChanged:
            {
                if(fullscreen === true)
                {
                    anchoredVideo1.visible = false
                    anchoredVideo2.visible = false
                    anchoredVideo3.visible = false
                    anchoredVideo0.visible = true
                }
                else
                {
                    console.log("normalllll ")
                    screen_base.returnToNormalMode(anchoredVideo0, anchoredVideo1, anchoredVideo2, anchoredVideo3);
                }
            }

            Connections
            {
                target: guiConnector.videos
                onVideo_channel_intput0Changed:
                {
                    if(visible === true)
                    {
                        if(guiConnector.videos.video_channel_intput0 !== null)
                        {
                            console.log("changing the video" + guiConnector.videos.video_channel_intput0.name)
                            anchoredVideo0.videoChannel = guiConnector.videos.video_channel_intput0;
                        }
                    }
                }
            }



        }

        // Channel 1
        Components.AnchoredVideo
        {

            Layout.fillWidth: true
            Layout.fillHeight: true
            id: anchoredVideo1
            visible: false
            //atrTrackingBbList: guiConnector.tracking.atrBoundingBoxCh1List;
            //vmdTrackingBbList: guiConnector.tracking.vmdBoundingBoxCh1List
            trackTrackingBbList: guiConnector.tracking.trackBoundingBoxCh1List
            trackObjBBox: guiConnector.tracking.trackedObjch2
            channelNumber: 1
            listVideos: guiConnector.videos.videoChannelList

            Component.onCompleted:
            {
                visible = guiConnector.videos.channel_1_active
            }

            onFullscreenChanged:
            {
                if(fullscreen === true)
                {
                    anchoredVideo0.visible = false
                    anchoredVideo2.visible = false
                    anchoredVideo3.visible = false
                    anchoredVideo1.visible = true
                }
                else
                {
                    screen_base.returnToNormalMode(anchoredVideo0, anchoredVideo1, anchoredVideo2, anchoredVideo3);
                }
            }

            Connections
            {
                target: guiConnector.tracking
                onAtrBoundingBoxCh1ListChanged:
                {
                    if(visible === true)
                    {
                        if(guiConnector.videos.video_channel_intput1 !== null)
                        {
                            anchoredVideo1.atrTrackingBbList = guiConnector.tracking.atrBoundingBoxCh1List;
                        }
                    }
                }
                onVmdBoundingBoxCh1ListChanged:
                {
                    if(visible === true)
                    {
                        if(guiConnector.videos.video_channel_intput1 !== null)
                        {
                            anchoredVideo1.vmdTrackingBbList = guiConnector.tracking.vmdBoundingBoxCh1List;
                        }
                    }
                }

            }

            Connections
            {
                target: guiConnector.videos
                onVideo_channel_intput1Changed:
                {
                    if(visible === true)
                    {
                        if(guiConnector.videos.video_channel_intput1 !== null)
                        {
                            anchoredVideo1.videoChannel = guiConnector.videos.video_channel_intput1;
                        }
                    }
                }
            }

        }

        // Channel 2
        Components.AnchoredVideo
        {

            Layout.fillWidth: true
            Layout.fillHeight: true
            id: anchoredVideo2
            visible: false
            atrTrackingBbList: guiConnector.tracking.atrBoundingBoxCh2List;
            vmdTrackingBbList: guiConnector.tracking.vmdBoundingBoxCh2List
            trackTrackingBbList: guiConnector.tracking.trackBoundingBoxCh2List

            channelNumber: 2
            listVideos: guiConnector.videos.videoChannelList

            onFullscreenChanged:
            {
                if(fullscreen === true)
                {
                    anchoredVideo0.visible = false
                    anchoredVideo1.visible = false
                    anchoredVideo3.visible = false
                    anchoredVideo2.visible = true
                }
                else
                {
                    screen_base.returnToNormalMode(anchoredVideo0, anchoredVideo1, anchoredVideo2, anchoredVideo3);
                }
            }

            Component.onCompleted:
            {
                visible = guiConnector.videos.channel_2_active
                if(visible === true)
                {
                    videoChannel = guiConnector.videos.video_channel_intput2;
                }
            }
        }

        // Channel 3
        Components.AnchoredVideo
        {

            Layout.fillWidth: true
            Layout.fillHeight: true
            id: anchoredVideo3
            visible: false
            atrTrackingBbList: guiConnector.tracking.atrBoundingBoxCh3List
            vmdTrackingBbList: guiConnector.tracking.vmdBoundingBoxCh3List
            trackTrackingBbList: guiConnector.tracking.trackBoundingBoxCh3List

            onFullscreenChanged:
            {
                if(fullscreen === true)
                {
                    anchoredVideo0.visible = false
                    anchoredVideo1.visible = false
                    anchoredVideo2.visible = false
                    anchoredVideo3.visible = true
                }
                else
                {
                    screen_base.returnToNormalMode(anchoredVideo0, anchoredVideo1, anchoredVideo2, anchoredVideo3);
                }
            }

            channelNumber: 3
            listVideos: guiConnector.videos.videoChannelList

            Component.onCompleted:
            {
                visible = guiConnector.videos.channel_3_active
                if(visible === true)
                {
                    videoChannel = guiConnector.videos.video_channel_intput3;
                }
            }
        }
    }
}
