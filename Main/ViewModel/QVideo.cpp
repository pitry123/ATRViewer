#include "QVideo.h"

AtrViewer::QVideoChannel::QVideoChannel(QString name,
                                QObject *parent) :
    QObject(parent),
    m_name(name),
    m_videoArgs(""),
    m_lastUpdateTime(std::chrono::high_resolution_clock::now())
{
}

AtrViewer::QVideoChannel::QVideoChannel(QObject *parent) :
    QVideoChannel("", parent)
{
}

QString AtrViewer::QVideoChannel::get_url() const
{
    std::lock_guard<std::mutex> locker(m_videoArgsMutex);
    return m_videoArgs;
}

void AtrViewer::QVideoChannel::set_url(const QString& val)
{
    std::unique_lock<std::mutex> locker(m_videoArgsMutex);
    if (m_videoArgs == val)
        return;

    m_videoArgs = val;
    locker.unlock();

    emit urlChanged(val);
}

bool AtrViewer::QVideoChannel::update()
{
#ifdef TIMER_PROPERTIES
    return true;
#else
    auto now = std::chrono::high_resolution_clock::now();
    if (std::chrono::duration_cast<std::chrono::milliseconds>(now - m_lastUpdateTime).count() < GUI_MIN_UPDATE_INTERVAL)
        return false;

    m_lastUpdateTime = now;
    return true;
#endif
}


#ifndef USE_NGVA
static inline int32_t cvt_barak_id(int sysId, int modId)
{
    return (sysId << 16) | modId;
}
#endif





std::map<QString, QString> AtrViewer::QVideoChannels::splitVideoStream(const QString& videoPublisherStream)
{
    std::map<QString, QString> oppMap;
    QStringList listStream = videoPublisherStream.split(QLatin1Char(' '));
    for(int i = 0; i < listStream.size(); i++)
    {
        if(listStream.at(i).startsWith("-"))
        {
            QStringList listStreamOpp = listStream.at(i).split(QLatin1Char(':'));
            QString opp = listStreamOpp.at(1);
            for(int j = 2; j < listStreamOpp.size(); j++)
            {
                opp.append(":");
                opp.append(listStreamOpp.at(j));
            }
            oppMap[listStreamOpp.at(0)] = opp;
        }
    }
    return oppMap;
}

inline std::string getGstPipe(const QString& videoPublisherStream)
{
    QStringList listStream = videoPublisherStream.split(QLatin1Char('"'));
    return listStream.at(1).toStdString();
}


AtrViewer::QVideoChannels::QVideoChannels(
        const Database::DataSet &dataset,
        const Utils::Context &context,
        QObject *parent) :
    QDatabaseConnector(context, parent),
    m_dataset(dataset),
    m_video_stream_allocator(this)
{
    if (dataset.Empty() == true)
        throw std::invalid_argument("dataset");

#ifndef USE_NGVA
    for (auto& _row :  m_dataset[BarakProjectCommon::MonitorDBIndex::IdentifierXml])
    {
       IdentifiersStore::ResourceStruct resource = _row.Read<IdentifiersStore::ResourceStruct>();
       m_id_to_src_name[cvt_barak_id(resource.nSystemID, resource.nModuleID)] = resource.strTypeName;
    }

#endif
}

AtrViewer::QVideoChannels::QVideoChannels(QObject *parent) :
    QVideoChannels(nullptr, nullptr, parent)
{
}

void AtrViewer::QVideoChannels::Start()
{
   // Multi
    m_videoChannelList.clear();
    m_videoChannelList.push_back(QVariant::fromValue<QString>("Select Video"));

#ifdef USE_NGVA

    ParamStore::ChannelsInputStruct channels_params = m_dataset[ProjectCommon::MonitorDBIndex::XML_StoreDB]
            [ParamStore::XML_StoreDBEnum::CHANNELS_INPUT_STREAM].Read<ParamStore::ChannelsInputStruct>();
#else

    ParamStore::ChannelsInputStruct channels_params = m_dataset[BarakProjectCommon::MonitorDBIndex::XML_StoreDB]
            [ParamStore::XML_StoreDBEnum::CHANNELS_INPUT_STREAM].Read<ParamStore::ChannelsInputStruct>();
#endif

    set_channel_0_active(channels_params.channels[0].IsActive);
    set_channel_1_active(channels_params.channels[1].IsActive);
    set_channel_2_active(channels_params.channels[2].IsActive);
    set_channel_3_active(channels_params.channels[3].IsActive);


    std::function<int(const char*)> find_ch_name = [channels_params](const char* name)
    {
        for (int i = 0;i < 4; i++)
        {
            if(QString::compare(channels_params.channels[i].SourceName, name, Qt::CaseInsensitive) == 0)
                return i;
        }

        return -1;
    };

#ifdef USE_NGVA


    Subscribe(m_dataset[ProjectCommon::MonitorDBIndex::VideoExtension_Incoming][NGVA::P_Video_Extension_PSM_Rows::C_VideoStream_Description_EX_Row] , [&, find_ch_name,channels_params](const Database::RowData& data)
    {
        auto video_description = data.Read<NGVA::P_Video_Extension_PSM::C_VideoStream_Description_EX>();

        QString stream_name(video_description.A_streamDescription.str);
        QString full_stream(video_description.A_streamURI_Args_Location.str);

        int latency = 300;
        int ch_id = -1;
        if((ch_id = find_ch_name(stream_name.toStdString().c_str())) != -1)
        {
            latency = channels_params.channels[ch_id].Latency;
        }

        std::stringstream rtsp_args;
        auto stream_opptions_map = AtrViewer::QVideoChannels::splitVideoStream(full_stream);
        if(stream_opptions_map.find("-source") != stream_opptions_map.end())
        {
            if(stream_opptions_map["-source"].compare("rtsp") == 0)
            {
                std::string m_cast = "";
                std::string stream_rtsp = "";
                if(stream_opptions_map.find("-url") != stream_opptions_map.end())
                {
                    stream_rtsp = stream_opptions_map["-url"].toStdString();
                }
                if(stream_opptions_map.find("-multicast") != stream_opptions_map.end())
                {
                    if(stream_opptions_map["-multicast"].compare("true") == 0)
                        m_cast = " protocols=udp-mcast";
                }
                if(stream_opptions_map.find("-latency") != stream_opptions_map.end())
                {
                    latency = stream_opptions_map["-latency"].toInt();
                }
                rtsp_args << "-source:gstreamer \"-pipeline:rtspsrc location=" << stream_rtsp << " latency="<<  std::to_string(latency) << m_cast << " ! rtpatimeparse ! rtph264depay ! h264parse ! avdec_h264 ! appsink name=mysink\" -sink:mysink";
            }
            if(stream_opptions_map["-source"].compare("gstreamer") == 0)
            {
                std::string stream_rtsp = "";
                std::string urlgst = getGstPipe(full_stream);
               // if(stream_opptions_map.find("-url") != stream_opptions_map.end())
                //{
                    stream_rtsp = stream_opptions_map["-url"].toStdString();
                    rtsp_args << "-source:gstreamer \""<< urlgst << "\"  -sink:mysink";

                    printf("**gst** : %s\n",rtsp_args.str().c_str());

                //}
            }

        }


//        if(stream_rtsp.startsWith("rtsp") == true)
//        {
//           //protocols=\"udp-mcast\"
//            rtsp_args << "-source:gstreamer \"-pipeline:rtspsrc location=" << stream_rtsp.toStdString()  << " latency="<<  std::to_string(latency) <<  " ! rtpatimeparse ! rtph264depay ! h264parse ! avdec_h264 ! appsink name=mysink\" -sink:mysink";
//        }
//        else
//        {
//            rtsp_args << stream_rtsp.toStdString();
//        }

        if(existance_video_channel_by_name(stream_name))
            return;

        QString stream_pipe(rtsp_args.str().c_str());

        m_video_channels_map[video_description.A_videoStreamDescribed_sourceID.A_resourceId] =
                new QVideoChannel(stream_name, this);

        m_video_channels_map[video_description.A_videoStreamDescribed_sourceID.A_resourceId]->set_url(stream_pipe);

        emit m_video_stream_allocator.openStream(stream_name,
                                                 m_video_channels_map[video_description.A_videoStreamDescribed_sourceID.A_resourceId]);

        emit newVideoDetection(stream_name,video_description.A_videoStreamDescribed_sourceID.A_resourceId);

        QVariantList last_list = m_videoChannelList;
        last_list.push_back(QVariant::fromValue<QString>(stream_name));
        set_videoChannelList(last_list);
    });


    NGVA::DDSReaderState filter_content = {};
            filter_content.mode = NGVA::DDSReaderMode::ON;

    m_dataset[ProjectCommon::MonitorDBIndex::VideoExtension_States]
            [NGVA::P_Video_Extension_PSM_Topics::C_VideoStream_Description_EX_Topic].
                    Write<NGVA::DDSReaderState>(filter_content);

#else

    Subscribe(m_dataset[BarakProjectCommon::MonitorDBIndex::Video_Incoming][Barak::P_Video_PSM_Rows::C_Rtsp_Row] , [&, find_ch_name,channels_params](const Database::RowData& data)
    {

        auto video_description = data.Read<Barak::P_Video_PSM::C_Rtsp>();

        QString stream_name(m_id_to_src_name[cvt_barak_id(video_description.A_sourceID.A_systemId,
                                                          video_description.A_sourceID.A_moduleId)].c_str());
        QString full_stream(video_description.A_uri.str);

        int latency = 300;
        int ch_id = -1;
        if((ch_id = find_ch_name(stream_name.toStdString().c_str())) != -1)
        {
            latency = channels_params.channels[ch_id].Latency;
        }

        std::stringstream rtsp_args;
        auto stream_opptions_map = AtrViewer::QVideoChannels::splitVideoStream(full_stream);
        if(stream_opptions_map.find("-source") != stream_opptions_map.end())
        {
            if(stream_opptions_map["-source"].compare("rtsp") == 0)
            {
                std::string m_cast = "";
                std::string stream_rtsp = "";
                if(stream_opptions_map.find("-url") != stream_opptions_map.end())
                {
                    stream_rtsp = stream_opptions_map["-url"].toStdString();
                }
                if(stream_opptions_map.find("-multicast") != stream_opptions_map.end())
                {
                    if(stream_opptions_map["-multicast"].compare("true") == 0)
                        m_cast = " protocols=udp-mcast";
                }
                if(stream_opptions_map.find("-latency") != stream_opptions_map.end())
                {
                    latency = stream_opptions_map["-latency"].toInt();
                }
                rtsp_args << "-source:gstreamer \"-pipeline:rtspsrc location=" << stream_rtsp << " latency="<<  std::to_string(latency) << m_cast << " ! rtpatimeparse ! rtph264depay ! h264parse ! avdec_h264 ! appsink name=mysink\" -sink:mysink";
            }
            if(stream_opptions_map["-source"].compare("gstreamer") == 0)
            {
                std::string stream_rtsp = "";
                if(stream_opptions_map.find("-url") != stream_opptions_map.end())
                {
                    stream_rtsp = stream_opptions_map["-url"].toStdString();
                    rtsp_args << stream_rtsp;

                }
            }

        }


//        std::stringstream rtsp_args;
//        rtsp_args << "-source:gstreamer \"-pipeline:rtspsrc location=" << stream_rtsp.toStdString()  << " latency="<<  std::to_string(latency) <<  " ! rtpatimeparse !rtph264depay ! h264parse ! avdec_h264 ! appsink name=mysink\" -sink:mysink";

        if(existance_video_channel_by_name(stream_name))
            return;


        QString stream_pipe(rtsp_args.str().c_str());

        m_video_channels_map[cvt_barak_id(video_description.A_sourceID.A_systemId,
                                          video_description.A_sourceID.A_moduleId)] =
                new QVideoChannel(stream_name, this);

        m_video_channels_map[cvt_barak_id(video_description.A_sourceID.A_systemId,
                                          video_description.A_sourceID.A_moduleId)]->set_url(stream_pipe);

        emit m_video_stream_allocator.openStream(stream_name,
                                                 m_video_channels_map[cvt_barak_id(video_description.A_sourceID.A_systemId,
                                                                                   video_description.A_sourceID.A_moduleId)]);

        emit newVideoDetection(stream_name,
                               cvt_barak_id(video_description.A_sourceID.A_systemId,
                                                                         video_description.A_sourceID.A_moduleId));


        QVariantList last_list = m_videoChannelList;
        last_list.push_back(QVariant::fromValue<QString>(stream_name));
        set_videoChannelList(last_list);
    });


    Barak::DDSReaderState filter_content = {};
            filter_content.mode = Barak::DDSReaderMode::ON;

    m_dataset[BarakProjectCommon::MonitorDBIndex::Video_States]
            [Barak::P_Video_PSM_Topics::C_Rtsp_Topic].
                    Write<Barak::DDSReaderState>(filter_content);



#endif
}

void AtrViewer::QVideoChannels::Stop()
{
}


void AtrViewer::QVideoChannels::changeChannelInuput(int ch, QString appSrcName)
{
    if(existance_video_channel_by_name(appSrcName))
    {
        AtrViewer::QVideoChannel* vc_input = new QVideoChannel(appSrcName);
        switch (ch)
        {
        case 0:
            set_video_channel_intput0(vc_input);
            break;
        case 1:
            set_video_channel_intput1(vc_input);
            break;
        case 2:
            set_video_channel_intput2(vc_input);
            break;
        case 3:
            set_video_channel_intput3(vc_input);
            break;
        default:
            return;
        }

        emit videoChannelInputChanged(ch,get_video_resource_id_by_name(appSrcName));
    }
}

bool AtrViewer::QVideoChannels::existance_video_channel_by_name(QString& name)
{
    for(auto& vc: m_video_channels_map)
    {
        auto vc_name = vc.second->get_name();
        if(QString::compare(vc_name, name, Qt::CaseInsensitive) == 0)
        {
            return true;
        }
    }
    return false;
}

uint32_t AtrViewer::QVideoChannels::get_video_resource_id_by_name(QString& name)
{
    for(auto& vc: m_video_channels_map)
    {
        auto vc_name = vc.second->get_name();
        if(QString::compare(vc_name, name, Qt::CaseInsensitive) == 0)
        {
            return vc.first;
        }
    }
    return 0;
}

int AtrViewer::QVideoChannels::get_video_resource_id(QString name)
{
    return get_video_resource_id_by_name(name);
}


