#include "QGuiConnector.h"
#include <iostream>
void AtrViewer::QGuiConnector::RegisterTypes()
{
    qRegisterMetaType<Guid>();
    qRegisterMetaType<BBoxParams>();

    RegisterType<AtrViewer::QGuiTypes>("GuiTypes");

    RegisterType<AtrViewer::QVideoChannel>("VideoChannel");
    RegisterType<AtrViewer::QVideoChannels>("VideoChannels");
    RegisterType<AtrViewer::QTracking>("Tracker");
    RegisterType<AtrViewer::QBBoxParams>("BBoxParams");

    RegisterType<AtrViewer::QGuiConnector>("GuiConnector");
    RegisterType<AtrViewer::QScreenRecord>("ScreenRecord");
    RegisterType<AtrViewer::QAlgoSettings>("AlgoSettings");
    RegisterType<AtrViewer::QProcessRecord>("ProcessRecord");
    RegisterType<AtrViewer::QExportData>("ExportData");

}


AtrViewer::QGuiConnector::QGuiConnector(const Database::DataSet& memoryDataset,
                                 QObject *parent) :
    QDatabaseConnector(parent),
    m_videos(nullptr),
    m_memoryDataset(memoryDataset)
{
    RegisterTypes();
}

AtrViewer::QGuiConnector::QGuiConnector(QObject *parent) :
    QGuiConnector(nullptr, parent)
{
}

AtrViewer::QGuiConnector::~QGuiConnector()
{
}

void AtrViewer::QGuiConnector::Init()
{
    m_videos = new AtrViewer::QVideoChannels(m_memoryDataset, Context(), this);

#ifdef USE_NGVA
    m_tracking = new AtrViewer::QTracking(m_memoryDataset[ProjectCommon::MonitorDBIndex::XML_StoreDB],
            m_memoryDataset[ProjectCommon::MonitorDBIndex::P_Tactical_Sensor_Extension_PSM_Incoming],
            m_memoryDataset[ProjectCommon::MonitorDBIndex::P_Tactical_Sensor_Extension_PSM_Outgoing],
            m_memoryDataset[ProjectCommon::MonitorDBIndex::P_Tactical_Sensor_Extension_PSM_States],
            m_memoryDataset[ProjectCommon::MonitorDBIndex::VideoTracking_Incoming],
            m_memoryDataset[ProjectCommon::MonitorDBIndex::VideoTracking_Outgoing],
            m_memoryDataset[ProjectCommon::MonitorDBIndex::VideoTracking_States],
            Context(), this);

    m_process_record = new AtrViewer::QProcessRecord(m_memoryDataset[ProjectCommon::MonitorDBIndex::P_Tactical_Sensor_Extension_PSM_Incoming]
            [NGVA::P_Tactical_Sensor_Extension_PSM_Rows::C_Detection_Location_Ex_Row]);

#else
    m_tracking = new AtrViewer::QTracking(m_memoryDataset[BarakProjectCommon::MonitorDBIndex::XML_StoreDB],
            m_memoryDataset[BarakProjectCommon::MonitorDBIndex::Tactical_Sensor_Incoming],
            m_memoryDataset[BarakProjectCommon::MonitorDBIndex::Tactical_Sensor_Outgoing],
            m_memoryDataset[BarakProjectCommon::MonitorDBIndex::Tactical_Sensor_States],
            Context(), this);

    m_process_record = new AtrViewer::QProcessRecord(m_memoryDataset[BarakProjectCommon::MonitorDBIndex::Tactical_Sensor_Incoming]
            [Barak::P_Tactical_Sensor_PSM_Rows::C_Detection_Location_Row]);
#endif
    m_screen_rec = new AtrViewer::QScreenRecord();


#ifdef USE_NGVA
    m_algo_settings = new AtrViewer::QAlgoSettings(m_memoryDataset[ProjectCommon::MonitorDBIndex::XML_StoreDB],
            m_memoryDataset[ProjectCommon::MonitorDBIndex::Tactical_Sensor_ATR_Config_Extension_PSM_Incoming],
            m_memoryDataset[ProjectCommon::MonitorDBIndex::Tactical_Sensor_ATR_Config_Extension_PSM_Outgoing],
            m_memoryDataset[ProjectCommon::MonitorDBIndex::Tactical_Sensor_ATR_Config_Extension_PSM_States],
            Context(), this);

#else
    m_algo_settings = new AtrViewer::QAlgoSettings(m_memoryDataset[BarakProjectCommon::MonitorDBIndex::XML_StoreDB],
            m_memoryDataset[BarakProjectCommon::MonitorDBIndex::Tactical_Config_Incoming],
            m_memoryDataset[BarakProjectCommon::MonitorDBIndex::Tactical_Config_Outgoing],
            m_memoryDataset[BarakProjectCommon::MonitorDBIndex::Tactical_Config_States],
            Context(), this);
#endif

    m_export_data = new AtrViewer::QExportData(this);

    m_algo_settings->Init();
    m_videos->Init();
    m_tracking->Init();
}

void AtrViewer::QGuiConnector::Start()
{
    connect(m_videos, &QVideoChannels::videoChannelInputChanged, m_tracking, &QTracking::onVideoChannelInputChanged);
    connect(m_videos, &QVideoChannels::newVideoDetection, m_process_record, &QProcessRecord::onNewVideoDetection);

    m_videos->Start();
    m_tracking->Start();
    m_algo_settings->Start();
}

void AtrViewer::QGuiConnector::Stop()
{
    m_subscriptions.Clear();
}
