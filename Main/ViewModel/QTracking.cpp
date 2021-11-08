#include "QTracking.h"
#include <QDebug>
#include <math.h>
#include <qqmlengine.h>

AtrViewer::QBBoxParams::QBBoxParams(int x, int y, int height, int width, int classification, double range, double idTrack,
                                    QString color, QObject* parent):
    QObject(parent)
{
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);
    this->set_params(BBoxParams(x, y, height, width, classification, range, idTrack, color));
}

AtrViewer::QBBoxParams::QBBoxParams(BBoxParams params, QObject* parent):
    QObject(parent)
{
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);
    this->set_params(params);
}

AtrViewer::QBBoxParams::QBBoxParams(QObject *parent):
    QBBoxParams(0,0,0,0,0,0,0, "",parent)
{
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);
}

AtrViewer::QBBoxParams::QBBoxParams(const AtrViewer::QBBoxParams &other)
{
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);
    m_params = other.get_params();
}

AtrViewer::QBBoxParams::~QBBoxParams()
{
}

#ifndef USE_NGVA


static inline int32_t get_barak_id(int sysId, int modId)
{
    return (sysId << 16) | modId;
}

static inline int32_t get_classification_barak_convert(const char* classificatio_str)
{
    std::string str_class(classificatio_str);
    if(str_class.compare("NONE") == 0)
    {
        return 0;
    }
    else if(str_class.compare("MAN") == 0)
    {
        return 1;
    }
    else if(str_class.compare("VEHICLE") == 0)
    {
        return 2;
    }
//    else if(str_class.compare("NONE") == 0)
//    {
//        return 0;
//    }
//    else if(str_class.compare("NONE") == 0)
//    {
//        return 0;
//    }


    return 0;
}

#endif

static constexpr int VECTOR_COLOR_SIZE = 20;






AtrViewer::QTracking::QTracking(const Database::Table& XmlConfigTable,
        const Database::Table& TacticalSensorExIncomingTable,
        const Database::Table& TacticalSensorExOutgoingTable,
        const Database::Table& TacticalSensorExStatesTable,
        const Database::Table& VideoTrakcingExIncomingTable,
        const Database::Table& VideoTrakcingExOutgoingTable,
        const Database::Table& VideoTrakcingExStatesTable,
        const Utils::Context &context,
        QObject *parent):
    QDatabaseConnector(context, parent),
    m_XmlConfigurationTable(XmlConfigTable),
    m_TacticalSensorExIncomingTable(TacticalSensorExIncomingTable),
    m_TacticalSensorExOutgoingTable(TacticalSensorExOutgoingTable),
    m_TacticalSensorExStatesTable(TacticalSensorExStatesTable),
    m_VideoTrakcingExIncomingTable(VideoTrakcingExIncomingTable),
    m_VideoTrakcingExOutgoingTable(VideoTrakcingExOutgoingTable),
    m_VideoTrakcingExStatesTable(VideoTrakcingExStatesTable)
{
    auto color_table = m_XmlConfigurationTable[ParamStore::XML_StoreDBEnum::COLORS_DETECTION].
            Read<ParamStore::ColorsDetectionStruct>();

    m_color_classification[0] =   color_table.unknownColor;
    m_color_classification[1] =   color_table.manColor;
    m_color_classification[2] =   color_table.vehicleColor;
    m_color_classification[3] =   color_table.armoredVehicleColor;
    m_color_classification[4] =   color_table.antiTankColor;
    m_color_classification[5] =   color_table.sniperColor;


    auto filter_class_table = m_XmlConfigurationTable[ParamStore::XML_StoreDBEnum::FILTER_CLASSIFICATION].
                    Read<ParamStore::FilterClassificationStruct>();

    m_filter_classification[0] = filter_class_table.unknownValid;
    m_filter_classification[1] = filter_class_table.manValid;
    m_filter_classification[2] = filter_class_table.vehicleValid;
    m_filter_classification[3] = filter_class_table.armoredVehicleValid;
    m_filter_classification[4] = filter_class_table.antiTankValid;
    m_filter_classification[5] = filter_class_table.sniperValid;


    m_video_channel_by_resource.clear();
    insert_random_color();

}

AtrViewer::QTracking::QTracking(QObject *parent) :
    QTracking(nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,  parent)
{}
#include <QColor>

void AtrViewer::QTracking::insert_random_color()
{
   int  r = rand() % 100 + 15;
   qsrand(time(0));
   float c_hue = 0.0;
   c_hue += 0.618033988749895f;
   c_hue = std::fmod(c_hue, 1.0f);
   for(int i = 0; i < VECTOR_COLOR_SIZE ; i++)
   {
       QColor c(QColor::fromHslF(c_hue, 1.0, 0.5)); //(qrand()%255, qrand()%255, qrand()%255);
       c_hue += 0.618033988749895f;
       c_hue = std::fmod(c_hue, 1.0f);
       m_color_vector.emplace_back(c.name());
   }
}

void AtrViewer::QTracking::Start()
{

    std::function<bool(const std::vector<uint32_t>& listResouce, uint32_t ch)> find_channel =
            [](const std::vector<uint32_t>& listResouce, uint32_t ch)
    {
        auto it = std::find(listResouce.begin(), listResouce.end(), ch);
        if(it != listResouce.end())
            return true;
        return  false;
    };

#ifdef USE_NGVA
    Subscribe(m_TacticalSensorExIncomingTable[NGVA::P_Tactical_Sensor_Extension_PSM_Rows::C_Detection_Location_Ex_Row], [this, find_channel](const Database::RowData& data)
    {
        NGVA::P_Tactical_Sensor_Extension_PSM::C_Detection_Location_Ex DetectionLocation =
            data.Read<NGVA::P_Tactical_Sensor_Extension_PSM::C_Detection_Location_Ex>();

        QString recognizingDetectorStr(DetectionLocation.A_recognizingDetectorTypes[0].str);
        if( recognizingDetectorStr.compare("ATR") == 0)
        {
            atr_detection_handler(DetectionLocation);
        }
        else if (recognizingDetectorStr.compare("VMD") == 0)
        {
            vmd_detection_handler(DetectionLocation);
        }
        else if (recognizingDetectorStr.compare("TRACK") == 0)
        {
            track_detection_handler(DetectionLocation);
        }
        else
        {
             Core::Console::ColorPrint(true,false, Core::Console::Colors::MAGENTA,"aaaaaaaaaa   atr wrapper aaaaaaaaaa\n");
        }
    });


    Subscribe(m_VideoTrakcingExIncomingTable[NGVA::P_VideoTracking_Extension_PSM_Rows::C_Video_TrackedObject_Status_Ex_Row], [this, find_channel](const Database::RowData& data)
    {
        NGVA::P_VideoTracking_Extension_PSM::C_Video_TrackedObject_Status_Ex TrackedObjectSts =
                data.Read<NGVA::P_VideoTracking_Extension_PSM::C_Video_TrackedObject_Status_Ex>();

        BBoxParams detectionMulti(TrackedObjectSts.A_positionColumn,
                                TrackedObjectSts.A_positionRow,
                                TrackedObjectSts.A_trackedObjectHeight,
                                TrackedObjectSts.A_trackedObjectWidth,
                                  0,
                                  0.0,0.0,//Range
                                  "black");

        for(int i = 0; i < MAX_CHANNEL_VIDEO_STREAM_INPUT; i++)
        {
            if(m_video_channel_by_resource[i] == TrackedObjectSts.A_trackedVideoStreamID.A_resourceId)
            {
                if (i == 0)
                {
                    set_trackedObjch1(detectionMulti);
                }
                if (i == 1)
                {
                    set_trackedObjch1(detectionMulti);
                }
            }
        }
    });





    NGVA::DDSReaderState filter_content = {};
            filter_content.mode = NGVA::DDSReaderMode::ON;

    m_VideoTrakcingExStatesTable[NGVA::P_VideoTracking_Extension_PSM_Topics::C_Video_TrackedObject_Status_Ex_Topic].
            Write<NGVA::DDSReaderState>(filter_content);

    m_TacticalSensorExStatesTable[NGVA::P_Tactical_Sensor_Extension_PSM_Topics::C_Detection_Location_Ex_Topic].
                    Write<NGVA::DDSReaderState>(filter_content);

    m_VideoTrakcingExStatesTable[NGVA::P_VideoTracking_Extension_PSM_Topics::C_Video_Tracker_leadOnAtCenter_Writer].
            Write(NGVA::DDSWriterState{ NGVA::DDSWriterMode::PERSISTENT, {0, 0} });

#else

    Subscribe(m_TacticalSensorExIncomingTable[Barak::P_Tactical_Sensor_PSM_Rows::C_Detection_Location_Row], [this, find_channel](const Database::RowData& data)
    {
        Barak::P_Tactical_Sensor_PSM::C_Detection_Location DetectionLocation =
            data.Read<Barak::P_Tactical_Sensor_PSM::C_Detection_Location>();

        QString recognizingDetectorStr(DetectionLocation.A_recognizingDetectorTypes[0].str);
        if( recognizingDetectorStr.compare("ATR") == 0)
        {
            atr_detection_handler(DetectionLocation);
        }
        else if (recognizingDetectorStr.compare("VMD") == 0)
        {
            vmd_detection_handler(DetectionLocation);
        }
        else if (recognizingDetectorStr.compare("TRACK") == 0)
        {
            track_detection_handler(DetectionLocation);
        }
        else
        {
             Core::Console::ColorPrint(true,false, Core::Console::Colors::MAGENTA,"aaaaaaaaaa   atr wrapper aaaaaaaaaa\n");
        }


    });


    Barak::DDSReaderState filter_content = {};
            filter_content.mode = Barak::DDSReaderMode::ON;

    m_TacticalSensorExStatesTable[Barak::P_Tactical_Sensor_PSM_Topics::C_Detection_Location_Topic].
                    Write<Barak::DDSReaderState>(filter_content);


#endif
}

#ifdef USE_NGVA

void AtrViewer::QTracking::atr_detection_handler(const NGVA::P_Tactical_Sensor_Extension_PSM::C_Detection_Location_Ex& detectionLocation)
{
    int classification = static_cast<int>(detectionLocation.A_object_Classisfication);
//    if ((m_filter_classification[classification] == false) &&
//            (detectionLocation.A_detection_data_Guid.A_LSB != -1 &&
//                        detectionLocation.A_detection_data_Guid.A_MSB != -1))
//        return;

   // auto time_atr_wrapper_micro = static_cast<uint64_t>((((detectionLocation.A_timeOfDataGeneration.A_second * 1000000000) + detectionLocation.A_timeOfDataGeneration.A_nanoseconds) / 1000) + 0.5);
   // uint64_t timestamp_now_micro = static_cast<uint64_t>(std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch()).count())
   // Core::Console::ColorPrint(true,false, Core::Console::Colors::MAGENTA,"atr wrapper[%lld]   atr viewer[%lld]   wrapper-view[%lld]\n",time_atr_wrapper_micro, timestamp_now_micro, time_atr_wrapper_micro- timestamp_now_micro);

    std::lock_guard<std::mutex> lock(m_video_channel_mutex);

    if(detectionLocation.A_detection_data_Guid.A_LSB == -1 &&
            detectionLocation.A_detection_data_Guid.A_MSB == -1)
    {
        for(int i = 0; i < MAX_CHANNEL_VIDEO_STREAM_INPUT; i++)
        {
            if(m_video_channel_by_resource[i] == detectionLocation.A_detection_data_sourceID.A_resourceId)
            {
                switch (i)
                {
                case 0:
                    set_atrBoundingBoxCh0List(m_AtrChannelsBboxList[0]);
                    break;
                case 1:
                    set_atrBoundingBoxCh1List(m_AtrChannelsBboxList[1]);
                    break;
                case 2:
                    set_atrBoundingBoxCh2List(m_AtrChannelsBboxList[2]);
                    break;
                case 3:
                    set_atrBoundingBoxCh3List(m_AtrChannelsBboxList[3]);
                    break;
                default:
                    return;
                }
                m_AtrChannelsBboxList[i].clear();
            }
        }
        return;
    }

    BBoxParams detectionMulti(detectionLocation.A_videoLocation.A_videoColumn,
                        detectionLocation.A_videoLocation.A_videoRow,
                        detectionLocation.A_videoBoundingBox.A_videoHeight,
                        detectionLocation.A_videoBoundingBox.A_videoWidth,
                        classification,
                        detectionLocation.A_absolutePolarLocation.A_range,
                        0.0,m_color_classification[classification]);


    for(int i = 0; i < MAX_CHANNEL_VIDEO_STREAM_INPUT; i++)
    {
        if(m_video_channel_by_resource[i] == detectionLocation.A_detection_data_sourceID.A_resourceId)
        {
            m_AtrChannelsBboxList[i].push_back((QVariant::fromValue<BBoxParams>(detectionMulti)));
        }
    }
}

void AtrViewer::QTracking::vmd_detection_handler(const NGVA::P_Tactical_Sensor_Extension_PSM::C_Detection_Location_Ex& detectionLocation)
{
    std::lock_guard<std::mutex> lock(m_video_channel_mutex);

    if(detectionLocation.A_detection_data_Guid.A_LSB == -1 &&
            detectionLocation.A_detection_data_Guid.A_MSB == -1)
    {
        for(int i = 0; i < MAX_CHANNEL_VIDEO_STREAM_INPUT; i++)
        {
            if(m_video_channel_by_resource[i] == detectionLocation.A_detection_data_sourceID.A_resourceId)
            {
                switch (i)
                {
                case 0:
                    set_vmdBoundingBoxCh0List(m_VmdChannelsBboxList[0]);
                    break;
                case 1:
                    set_vmdBoundingBoxCh1List(m_VmdChannelsBboxList[1]);
                    break;
                case 2:
                    set_vmdBoundingBoxCh2List(m_VmdChannelsBboxList[2]);
                    break;
                case 3:
                    set_vmdBoundingBoxCh3List(m_VmdChannelsBboxList[3]);
                    break;
                default:
                    return;
                }
                m_VmdChannelsBboxList[i].clear();
            }
        }
        return;
    }

    BBoxParams detectionMulti(detectionLocation.A_videoLocation.A_videoColumn,
                        detectionLocation.A_videoLocation.A_videoRow,
                        detectionLocation.A_videoBoundingBox.A_videoHeight,
                        detectionLocation.A_videoBoundingBox.A_videoWidth,
                              12,
                              0.0,0.0,//Range
                              "White");


    for(int i = 0; i < MAX_CHANNEL_VIDEO_STREAM_INPUT; i++)
    {
        if(m_video_channel_by_resource[i] == detectionLocation.A_detection_data_sourceID.A_resourceId)
        {
            m_VmdChannelsBboxList[i].push_back((QVariant::fromValue<BBoxParams>(detectionMulti)));
        }
    }
}

void AtrViewer::QTracking::track_detection_handler(const NGVA::P_Tactical_Sensor_Extension_PSM::C_Detection_Location_Ex& detectionLocation)
{
    int classification = static_cast<int>(detectionLocation.A_object_Classisfication);
//  if ((m_filter_classification[classification] == false) &&
//            (detectionLocation.A_detection_data_Guid.A_LSB != -1 &&
//                        detectionLocation.A_detection_data_Guid.A_MSB != -1))
//        return;

    std::lock_guard<std::mutex> lock(m_video_channel_mutex);

    if(detectionLocation.A_detection_data_Guid.A_LSB == -1 &&
            detectionLocation.A_detection_data_Guid.A_MSB == -1)
    {
        for(int i = 0; i < MAX_CHANNEL_VIDEO_STREAM_INPUT; i++)
        {
            if(m_video_channel_by_resource[i] == detectionLocation.A_detection_data_sourceID.A_resourceId)
            {
                switch (i)
                {
                case 0:
                    set_trackBoundingBoxCh0List(m_TrackChannelsBboxList[0]);
                    break;
                case 1:
                    set_trackBoundingBoxCh1List(m_TrackChannelsBboxList[1]);
                    break;
                case 2:
                    set_trackBoundingBoxCh2List(m_TrackChannelsBboxList[2]);
                    break;
                case 3:
                    set_trackBoundingBoxCh3List(m_TrackChannelsBboxList[3]);
                    break;
                default:
                    return;
                }
                m_TrackChannelsBboxList[i].clear();
            }
        }
        return;
    }


    double trId = static_cast<double>(detectionLocation.A_detection_data_Guid.A_LSB);
    QString C_name = m_color_vector[static_cast<int>(trId) % VECTOR_COLOR_SIZE];
    BBoxParams detectionMulti(detectionLocation.A_videoLocation.A_videoColumn,
                        detectionLocation.A_videoLocation.A_videoRow,
                        detectionLocation.A_videoBoundingBox.A_videoHeight,
                        detectionLocation.A_videoBoundingBox.A_videoWidth,
                              classification + 6,
                              0.0,
                              trId, C_name);


    for(int i = 0; i < MAX_CHANNEL_VIDEO_STREAM_INPUT; i++)
    {
        if(m_video_channel_by_resource[i] == detectionLocation.A_detection_data_sourceID.A_resourceId)
        {
            m_TrackChannelsBboxList[i].push_back((QVariant::fromValue<BBoxParams>(detectionMulti)));
        }
    }
}

#else

void AtrViewer::QTracking::atr_detection_handler(const Barak::P_Tactical_Sensor_PSM::C_Detection_Location& detectionLocation)
{
    int classification = get_classification_barak_convert(detectionLocation.A_objectClassification.str);

    std::lock_guard<std::mutex> lock(m_video_channel_mutex);

    if(detectionLocation.A_detectionUniqueID.A_lsb == -1 &&
            detectionLocation.A_detectionUniqueID.A_msb == -1)
    {
        for(int i = 0; i < MAX_CHANNEL_VIDEO_STREAM_INPUT; i++)
        {
            if(m_video_channel_by_resource[i] == get_barak_id( detectionLocation.A_videoStreamSourceID.A_systemId,
                                                               detectionLocation.A_videoStreamSourceID.A_moduleId))
            {
                switch (i)
                {
                case 0:
                    printf("Atr: count detect: %d     \n", m_AtrChannelsBboxList[0].size());
                    set_atrBoundingBoxCh0List(m_AtrChannelsBboxList[0]);
                    break;
                case 1:
                    set_atrBoundingBoxCh1List(m_AtrChannelsBboxList[1]);
                    break;
                case 2:
                    set_atrBoundingBoxCh2List(m_AtrChannelsBboxList[2]);
                    break;
                case 3:
                    set_atrBoundingBoxCh3List(m_AtrChannelsBboxList[3]);
                    break;
                default:
                    return;
                }
                m_AtrChannelsBboxList[i].clear();
            }
        }
        return;
    }

   // int  r = rand() % 100 + 15;

    BBoxParams detectionMulti(detectionLocation.A_detectionCenterVideoLocation.A_horizontalPosition,
                        detectionLocation.A_detectionCenterVideoLocation.A_verticalPosition,
                        detectionLocation.A_detectionSizeOnVideo.A_verticalPosition,
                              detectionLocation.A_detectionSizeOnVideo.A_horizontalPosition,
                              classification,
                             detectionLocation.A_detectionSize.A_range,0 , m_color_classification[classification]);

//    printf("ATR - x: %f y: %f  w: %f h: %f\n",detectionLocation.A_detectionCenterVideoLocation.A_horizontalPosition,
//           detectionLocation.A_detectionCenterVideoLocation.A_verticalPosition,
//           detectionLocation.A_detectionSizeOnVideo.A_horizontalPosition,
//           detectionLocation.A_detectionSizeOnVideo.A_verticalPosition);


    for(int i = 0; i < MAX_CHANNEL_VIDEO_STREAM_INPUT; i++)
    {
        if(m_video_channel_by_resource[i] ==get_barak_id( detectionLocation.A_videoStreamSourceID.A_systemId,
                                                          detectionLocation.A_videoStreamSourceID.A_moduleId))
        {
            m_AtrChannelsBboxList[i].push_back((QVariant::fromValue<BBoxParams>(detectionMulti)));
        }
    }
}

void AtrViewer::QTracking::vmd_detection_handler(const Barak::P_Tactical_Sensor_PSM::C_Detection_Location& detectionLocation)
{
    int classification = get_classification_barak_convert(detectionLocation.A_objectClassification.str);

    std::lock_guard<std::mutex> lock(m_video_channel_mutex);

    if(detectionLocation.A_detectionUniqueID.A_lsb == -1 &&
            detectionLocation.A_detectionUniqueID.A_msb == -1)
    {
        for(int i = 0; i < MAX_CHANNEL_VIDEO_STREAM_INPUT; i++)
        {
            if(m_video_channel_by_resource[i] == get_barak_id( detectionLocation.A_videoStreamSourceID.A_systemId,
                                                               detectionLocation.A_videoStreamSourceID.A_moduleId))
            {
                switch (i)
                {
                case 0:
                    set_vmdBoundingBoxCh0List(m_VmdChannelsBboxList[0]);
                    break;
                case 1:
                    set_vmdBoundingBoxCh1List(m_VmdChannelsBboxList[1]);
                    break;
                case 2:
                    set_vmdBoundingBoxCh2List(m_VmdChannelsBboxList[2]);
                    break;
                case 3:
                    set_vmdBoundingBoxCh3List(m_VmdChannelsBboxList[3]);
                    break;
                default:
                    return;
                }
                m_VmdChannelsBboxList[i].clear();
            }
        }
        return;
    }

    BBoxParams detectionMulti(detectionLocation.A_detectionCenterVideoLocation.A_horizontalPosition,
                              detectionLocation.A_detectionCenterVideoLocation.A_verticalPosition,
                              detectionLocation.A_detectionSizeOnVideo.A_verticalPosition,
                                    detectionLocation.A_detectionSizeOnVideo.A_horizontalPosition,
                              12,
                             0,0 , "White");


    for(int i = 0; i < MAX_CHANNEL_VIDEO_STREAM_INPUT; i++)
    {
        if(m_video_channel_by_resource[i] == get_barak_id( detectionLocation.A_videoStreamSourceID.A_systemId,
                                                          detectionLocation.A_videoStreamSourceID.A_moduleId))
        {
            m_VmdChannelsBboxList[i].push_back((QVariant::fromValue<BBoxParams>(detectionMulti)));
        }
    }
}

void AtrViewer::QTracking::track_detection_handler(const Barak::P_Tactical_Sensor_PSM::C_Detection_Location& detectionLocation)
{
    int classification = get_classification_barak_convert(detectionLocation.A_objectClassification.str);

    std::lock_guard<std::mutex> lock(m_video_channel_mutex);

    if(detectionLocation.A_detectionUniqueID.A_lsb == -1 &&
            detectionLocation.A_detectionUniqueID.A_msb == -1)
    {
        for(int i = 0; i < MAX_CHANNEL_VIDEO_STREAM_INPUT; i++)
        {
            if(m_video_channel_by_resource[i] == get_barak_id( detectionLocation.A_videoStreamSourceID.A_systemId,
                                                               detectionLocation.A_videoStreamSourceID.A_moduleId))
            {
                switch (i)
                {
                case 0:
                    set_trackBoundingBoxCh0List(m_TrackChannelsBboxList[0]);
                    break;
                case 1:
                    set_trackBoundingBoxCh1List(m_TrackChannelsBboxList[1]);
                    break;
                case 2:
                    set_trackBoundingBoxCh2List(m_TrackChannelsBboxList[2]);
                    break;
                case 3:
                    set_trackBoundingBoxCh3List(m_TrackChannelsBboxList[3]);
                    break;
                default:
                    return;
                }
                m_TrackChannelsBboxList[i].clear();
            }
        }
        return;
    }

    double trId = static_cast<double>(detectionLocation.A_detectionUniqueID.A_lsb);
    QString C_name = m_color_vector[static_cast<int>(trId) % VECTOR_COLOR_SIZE];
    BBoxParams detectionMulti(detectionLocation.A_detectionCenterVideoLocation.A_horizontalPosition,
                              detectionLocation.A_detectionCenterVideoLocation.A_verticalPosition,
                              detectionLocation.A_detectionSizeOnVideo.A_verticalPosition,
                                    detectionLocation.A_detectionSizeOnVideo.A_horizontalPosition,
                              classification + 6,
                             detectionLocation.A_detectionSize.A_range,trId , C_name);


    for(int i = 0; i < MAX_CHANNEL_VIDEO_STREAM_INPUT; i++)
    {
        if(m_video_channel_by_resource[i] == get_barak_id( detectionLocation.A_videoStreamSourceID.A_systemId,
                                                          detectionLocation.A_videoStreamSourceID.A_moduleId))
        {
            m_TrackChannelsBboxList[i].push_back((QVariant::fromValue<BBoxParams>(detectionMulti)));
        }
    }
}

#endif

void AtrViewer::QTracking::Stop()
{
}


void AtrViewer::QTracking::onVideoChannelInputChanged(int ch, int resource_video)
{

    std::lock_guard<std::mutex> lock(m_video_channel_mutex);
    m_video_channel_by_resource[ch] = resource_video;
}



void AtrViewer::QTracking::track_request(int video_id, double x, double y, double w, double h)
{
#ifdef USE_NGVA
    //stop before send because the algo support only in one track
//    NGVA::P_VideoTracking_Extension_PSM::C_Video_Tracker_StopTracking_Cmd_Ex stopLeadOn = {};
//    stopLeadOn.A_sourceID.A_resourceId = video_id;
//    m_VideoTrakcingExOutgoingTable[NGVA::P_VideoTracking_Extension_PSM_Rows::C_Video_Tracker_StopTracking_Cmd_Ex_Row].
//        Write(stopLeadOn);

    NGVA::P_VideoTracking_Extension_PSM::C_Video_Tracker_leadOnAtCenter leadOn = {};

    static int aaa = 0;
    leadOn.A_sourceID.A_resourceId = video_id;
    leadOn.A_sourceID.A_instanceId = aaa++;

    leadOn.A_positionColumn = x;
    leadOn.A_positionRow = y;
    leadOn.A_BBoxWidth = w;
    leadOn.A_BBoxHeight = h;

    m_VideoTrakcingExOutgoingTable[NGVA::P_VideoTracking_Extension_PSM_Rows::C_Video_Tracker_leadOnAtCenter_Row].
        Write(leadOn);
#endif
}


void AtrViewer::QTracking::stop_tracking(int video_id)
{
#ifdef USE_NGVA
    //stop before send because the algo support only in one track
    NGVA::P_VideoTracking_Extension_PSM::C_Video_Tracker_StopTracking_Cmd_Ex stopLeadOn = {};
    stopLeadOn.A_sourceID.A_resourceId = video_id;
    m_VideoTrakcingExOutgoingTable[NGVA::P_VideoTracking_Extension_PSM_Rows::C_Video_Tracker_StopTracking_Cmd_Ex_Row].
        Write(stopLeadOn);

#endif
}
