#include "QAlgoSettings.h"
#include <QDebug>
#include <math.h>
#include <qqmlengine.h>
#include <iostream>

#ifndef USE_NGVA
static inline int32_t cvt_barak_id(int16_t sysId, int16_t modId)
{
    return (sysId << 16) | modId;
}

static inline void cvt_revers_barak_id(const int32_t input,int16_t& sysId, int16_t& modId)
{
    modId = input & 0x00FFFF;
    sysId = (input >> 16) & 0x00FFFF;
}
#endif



#ifdef USE_NGVA
    namespace DDS = NGVA;
#else
    namespace DDS = Barak;
#endif


AtrViewer::QAlgoSettings::QAlgoSettings(const Database::Table& XmlConfigTable,
        const Database::Table& TacticalSensorExIncomingTable,
        const Database::Table& TacticalSensorExOutgoingTable,
        const Database::Table& TacticalSensorExStatesTable,
        const Utils::Context &context,
        QObject *parent):
    QDatabaseConnector(context, parent),
    m_atr_algo_en(false),
    m_vmd_algo_en(false),
    m_track_algo_en(false),
    m_atr_algo_pd(0.0),
    m_vmd_algo_pd(0.0),
    m_track_algo_pd(0.0),
    m_XmlConfigurationTable(XmlConfigTable),
    m_TacticalSensorExIncomingTable(TacticalSensorExIncomingTable),
    m_TacticalSensorExOutgoingTable(TacticalSensorExOutgoingTable),
    m_TacticalSensorExStatesTable(TacticalSensorExStatesTable),
    m_current_video("")
{
    m_energy_policy_map["15W_2CORE"]  = 1;
    m_energy_policy_map["15W_4_CORE"] = 2;
    m_energy_policy_map["15W_6_CORE"] = 3;
    m_energy_policy_map["10W_2_CORE"] = 4;
    m_energy_policy_map["10W_4_CORE"] = 5;
}

AtrViewer::QAlgoSettings::QAlgoSettings(QObject *parent) :
    QAlgoSettings(nullptr, nullptr, nullptr, nullptr, nullptr,  parent)
{}


AtrViewer::QAlgoSettings::algoType AtrViewer::QAlgoSettings::convert_to_algoType(const QString& name)
{
    algoType c_algo_enum;
    if(name.compare("ATR") == 0)
        c_algo_enum = algoType::ATR;
    if(name.compare("VMD") == 0)
        c_algo_enum = algoType::VMD;
    if(name.compare("Track") == 0)
        c_algo_enum = algoType::TRACK;

    return c_algo_enum;
}


void AtrViewer::QAlgoSettings::Start()
{
#ifdef USE_NGVA

    m_TacticalSensorExStatesTable[NGVA::P_Tactical_Sensor_ATR_Config_Extension_PSM_Topics::C_Tactical_Sensor_Config_Ex_Writer].
            Write(NGVA::DDSWriterState{ NGVA::DDSWriterMode::PERSISTENT, {0, 0} });
    Subscribe(m_TacticalSensorExIncomingTable[NGVA::P_Tactical_Sensor_ATR_Config_Extension_PSM_Rows::C_Tactical_Sensor_Config_Ex_Row], [&](const Database::RowData& data)
    {

        auto _Sensor_Config = data.Read< NGVA::P_Tactical_Sensor_ATR_Config_Extension_PSM::C_Tactical_Sensor_Config_Ex>();
        for(auto& source_id: m_map_video_id)
        {
            if(_Sensor_Config.A_sourceID.A_resourceId == source_id.second)
            {
                for (int i=0; i<static_cast<int>(_Sensor_Config.A_algostatus_ItemsCount); i++)
                {
                    QString AlgoName(_Sensor_Config.A_algostatus[i].A_sensorType.str);
                    algoType c_algo_enum = convert_to_algoType(AlgoName);

                    if(m_current_video.compare(source_id.first) == 0)
                    {
                        switch (c_algo_enum)
                        {
                        case AtrViewer::QAlgoSettings::ATR:
                            set_atr_algo_en(_Sensor_Config.A_algostatus[i].A_algoState);
                            set_atr_algo_pd(static_cast<int>(_Sensor_Config.A_algostatus[i].A_detectionProbability));
                            break;
                        case AtrViewer::QAlgoSettings::VMD:
                            set_vmd_algo_en(_Sensor_Config.A_algostatus[i].A_algoState);
                            set_vmd_algo_pd(static_cast<int>(_Sensor_Config.A_algostatus[i].A_detectionProbability));
                            break;
                        case AtrViewer::QAlgoSettings::TRACK:
                            set_track_algo_en(_Sensor_Config.A_algostatus[i].A_algoState);
                            set_track_algo_pd(static_cast<int>(_Sensor_Config.A_algostatus[i].A_detectionProbability));
                            break;
                        }
                    }
                    m_video_algo_setting[source_id.first][c_algo_enum] = _Sensor_Config.A_algostatus[0].A_algoState;
                }
                return;
            }
        }
    });

    NGVA::DDSReaderState filter_content = {};
            filter_content.mode = NGVA::DDSReaderMode::ON;

    m_TacticalSensorExStatesTable[NGVA::P_Tactical_Sensor_ATR_Config_Extension_PSM_Topics::C_Tactical_Sensor_Config_Ex_Topic].
            Write(filter_content);


#else
    m_TacticalSensorExStatesTable[Barak::P_Tactical_Sensor_ATR_Config_Extension_PSM_Topics::C_Tactical_Sensor_Config_Ex_Writer].
            Write(Barak::DDSWriterState{ Barak::DDSWriterMode::PERSISTENT, {0, 0} });
    Subscribe(m_TacticalSensorExIncomingTable[Barak::P_Tactical_Sensor_ATR_Config_Extension_PSM_Rows::C_Tactical_Sensor_Config_Ex_Row], [&](const Database::RowData& data)
    {

        auto _Sensor_Config = data.Read< Barak::P_Tactical_Sensor_ATR_Config_Extension_PSM::C_Tactical_Sensor_Config_Ex>();
        for(auto& source_id: m_map_video_id)
        {
            if(cvt_barak_id(_Sensor_Config.A_sourceID.A_systemId, _Sensor_Config.A_sourceID.A_moduleId) ==
                    source_id.second)
            {
                for (int i=0; i<static_cast<int>(_Sensor_Config.A_algostatus_ItemsCount); i++)
                {
                    QString AlgoName(_Sensor_Config.A_algostatus[i].A_sensorType.str);
                    algoType c_algo_enum = convert_to_algoType(AlgoName);

                    if(m_current_video.compare(source_id.first) == 0)
                    {
                        switch (c_algo_enum)
                        {
                        case AtrViewer::QAlgoSettings::ATR:
                            set_atr_algo_en(_Sensor_Config.A_algostatus[i].A_algoState);
                            set_atr_algo_pd(static_cast<int>(_Sensor_Config.A_algostatus[i].A_detectionProbability));
                            break;
                        case AtrViewer::QAlgoSettings::VMD:
                            set_vmd_algo_en(_Sensor_Config.A_algostatus[i].A_algoState);
                            set_vmd_algo_pd(static_cast<int>(_Sensor_Config.A_algostatus[i].A_detectionProbability));
                            break;
                        case AtrViewer::QAlgoSettings::TRACK:
                            set_track_algo_en(_Sensor_Config.A_algostatus[i].A_algoState);
                            set_track_algo_pd(static_cast<int>(_Sensor_Config.A_algostatus[i].A_detectionProbability));
                            break;
                        }
                    }
                    m_video_algo_setting[source_id.first][c_algo_enum] = _Sensor_Config.A_algostatus[i].A_algoState;
                    m_video_algo_pd_setting[source_id.first][c_algo_enum] = _Sensor_Config.A_algostatus[i].A_detectionProbability;
                }
                return;
            }
        }
    });


    Barak::DDSReaderState filter_content = {};
            filter_content.mode = Barak::DDSReaderMode::ON;

    m_TacticalSensorExStatesTable[Barak::P_Tactical_Sensor_ATR_Config_Extension_PSM_Topics::C_Tactical_Sensor_Config_Ex_Topic].
            Write(filter_content);

#endif
}

void AtrViewer::QAlgoSettings::Stop()
{
}


void AtrViewer::QAlgoSettings::changeVideo(QString appSrcName)
{
    if(m_map_video_id.find(appSrcName) != m_map_video_id.end())
    {

        m_current_video = appSrcName;
        this->set_atr_algo_en(m_video_algo_setting[m_current_video][AtrViewer::QAlgoSettings::ATR]);
        this->set_vmd_algo_en(m_video_algo_setting[m_current_video][AtrViewer::QAlgoSettings::VMD]);
        this->set_track_algo_en(m_video_algo_setting[m_current_video][AtrViewer::QAlgoSettings::TRACK]);

        this->set_atr_algo_pd(m_video_algo_pd_setting[m_current_video][AtrViewer::QAlgoSettings::ATR]);
        this->set_vmd_algo_pd(m_video_algo_pd_setting[m_current_video][AtrViewer::QAlgoSettings::VMD]);
        this->set_track_algo_pd(m_video_algo_pd_setting[m_current_video][AtrViewer::QAlgoSettings::TRACK]);

        std::cout << "M atr: "<< m_atr_algo_en <<"\n";
        std::cout << "map atr: "<< m_video_algo_setting[m_current_video][AtrViewer::QAlgoSettings::ATR] <<"\n";


    }
}

void AtrViewer::QAlgoSettings::listNewVideo(QString appSrcName, int resourceId)
{
    std::lock_guard<std::mutex> lock(m_video_channel_mutex);
    if(m_map_video_id.find(appSrcName) == m_map_video_id.end())
    {
        m_map_video_id[appSrcName] = resourceId;
        m_video_algo_setting[appSrcName][algoType::ATR] = false;
        m_video_algo_setting[appSrcName][algoType::VMD] = false;
        m_video_algo_setting[appSrcName][algoType::TRACK] = false;
    }
}


void AtrViewer::QAlgoSettings::changeAlgoState(QString AlgoName, bool state)
{
    algoType c_algo_enum;
    if(AlgoName.compare("ATR") == 0)
        c_algo_enum = algoType::ATR;
    if(AlgoName.compare("VMD") == 0)
        c_algo_enum = algoType::VMD;
    if(AlgoName.compare("Track") == 0)
        c_algo_enum = algoType::TRACK;

     m_video_algo_setting[m_current_video][c_algo_enum] = state;
     double pd = m_video_algo_pd_setting[m_current_video][c_algo_enum];
//     std::cout << "change map atr: "<< m_video_algo_setting[m_current_video][AtrViewer::QAlgoSettings::ATR] <<"\n";

     DDS::P_Tactical_Sensor_ATR_Config_Extension_PSM::C_Tactical_Sensor_Config_Ex config = {};
#ifdef USE_NGVA
     config.A_sourceID.A_resourceId = m_map_video_id[m_current_video];
#else
     int16_t sys_id, mod_id;
     cvt_revers_barak_id(m_map_video_id[m_current_video], sys_id, mod_id);
     config.A_sourceID.A_systemId = sys_id;
     config.A_sourceID.A_moduleId = mod_id;
#endif
     config.A_algostatus_ItemsCount = 1;
     config.A_algostatus[0].A_algoState = state;
     config.A_algostatus[0].A_detectionProbability = pd;
     config.A_config_request = DDS::P_Tactical_Sensor_ATR_Config_Extension_PSM::T_Config_RequestType::L_Config_RequestType_SaveAsAlgoStatus;
     strcpy(config.A_algostatus[0].A_sensorType.str, AlgoName.toStdString().c_str());

     m_TacticalSensorExOutgoingTable[DDS::P_Tactical_Sensor_ATR_Config_Extension_PSM_Rows::C_Tactical_Sensor_Config_Ex_Row].
             Write(config);


}

void AtrViewer::QAlgoSettings::changePdValue(QString AlgoName, double pd)
{
    algoType c_algo_enum;
    if(AlgoName.compare("ATR") == 0)
        c_algo_enum = algoType::ATR;
    if(AlgoName.compare("VMD") == 0)
        c_algo_enum = algoType::VMD;
    if(AlgoName.compare("Track") == 0)
        c_algo_enum = algoType::TRACK;

     bool state = m_video_algo_setting[m_current_video][c_algo_enum];
     m_video_algo_pd_setting[m_current_video][c_algo_enum] = pd;
     std::cout << "change pd atr: "<< pd<<"\n";

     DDS::P_Tactical_Sensor_ATR_Config_Extension_PSM::C_Tactical_Sensor_Config_Ex config = {};
#ifdef USE_NGVA

     config.A_sourceID.A_resourceId = m_map_video_id[m_current_video];
#else
     int16_t sys_id, mod_id;
     cvt_revers_barak_id(m_map_video_id[m_current_video], sys_id, mod_id);
     config.A_sourceID.A_systemId = sys_id;
     config.A_sourceID.A_moduleId = mod_id;
#endif

     config.A_algostatus_ItemsCount = 1;
     config.A_algostatus[0].A_algoState = state;
     config.A_algostatus[0].A_detectionProbability = pd;
     config.A_config_request = DDS::P_Tactical_Sensor_ATR_Config_Extension_PSM::T_Config_RequestType::L_Config_RequestType_SaveAsAlgoStatus;
     strcpy(config.A_algostatus[0].A_sensorType.str, AlgoName.toStdString().c_str());

     m_TacticalSensorExOutgoingTable[DDS::P_Tactical_Sensor_ATR_Config_Extension_PSM_Rows::C_Tactical_Sensor_Config_Ex_Row].
             Write(config);
}


void AtrViewer::QAlgoSettings::setDefault()
{

     DDS::P_Tactical_Sensor_ATR_Config_Extension_PSM::C_Tactical_Sensor_Config_Ex config = {};
#ifdef USE_NGVA

     config.A_sourceID.A_resourceId = m_map_video_id[m_current_video];
#else
     int16_t sys_id, mod_id;
     cvt_revers_barak_id(m_map_video_id[m_current_video], sys_id, mod_id);
     config.A_sourceID.A_systemId = sys_id;
     config.A_sourceID.A_moduleId = mod_id;
#endif
     config.A_config_request = DDS::P_Tactical_Sensor_ATR_Config_Extension_PSM::T_Config_RequestType::L_Config_RequestType_SetDefault;

     m_TacticalSensorExOutgoingTable[DDS::P_Tactical_Sensor_ATR_Config_Extension_PSM_Rows::C_Tactical_Sensor_Config_Ex_Row].
             Write(config);


}


void AtrViewer::QAlgoSettings::saveAsDefault()
{

     DDS::P_Tactical_Sensor_ATR_Config_Extension_PSM::C_Tactical_Sensor_Config_Ex config = {};
#ifdef USE_NGVA
     config.A_sourceID.A_resourceId = m_map_video_id[m_current_video];
#else
     int16_t sys_id, mod_id;
     cvt_revers_barak_id(m_map_video_id[m_current_video], sys_id, mod_id);
     config.A_sourceID.A_systemId = sys_id;
     config.A_sourceID.A_moduleId = mod_id;
#endif
     config.A_config_request = DDS::P_Tactical_Sensor_ATR_Config_Extension_PSM::T_Config_RequestType::L_Config_RequestType_SaveAsDefault;

     m_TacticalSensorExOutgoingTable[DDS::P_Tactical_Sensor_ATR_Config_Extension_PSM_Rows::C_Tactical_Sensor_Config_Ex_Row].
             Write(config);


}



void AtrViewer::QAlgoSettings::changeAINetwork(QString NetworkName)
{
     DDS::P_Tactical_Sensor_ATR_Config_Extension_PSM::C_Tactical_Sensor_Config_Ex config = {};
#ifdef USE_NGVA
     config.A_sourceID.A_resourceId = m_map_video_id[m_current_video];
#else
     int16_t sys_id, mod_id;
     cvt_revers_barak_id(m_map_video_id[m_current_video], sys_id, mod_id);
     config.A_sourceID.A_systemId = sys_id;
     config.A_sourceID.A_moduleId = mod_id;
#endif

    DDS::P_Tactical_Sensor_ATR_Config_Extension_PSM::T_Config_AINetworkType network =
            DDS::P_Tactical_Sensor_ATR_Config_Extension_PSM::T_Config_AINetworkType::L_Config_AINetworkType_None;

     if(NetworkName.compare("Network_0") == 0)
         network = DDS::P_Tactical_Sensor_ATR_Config_Extension_PSM::T_Config_AINetworkType::L_Config_AINetworkType_Network_0;
     else if(NetworkName.compare("Network_1") == 0)
         network = DDS::P_Tactical_Sensor_ATR_Config_Extension_PSM::T_Config_AINetworkType::L_Config_AINetworkType_Network_1;
     else if(NetworkName.compare("Network_2") == 0)
         network = DDS::P_Tactical_Sensor_ATR_Config_Extension_PSM::T_Config_AINetworkType::L_Config_AINetworkType_Network_2;
    else
         return;


     config.A_network = network;
     m_TacticalSensorExOutgoingTable[DDS::P_Tactical_Sensor_ATR_Config_Extension_PSM_Rows::C_Tactical_Sensor_Config_Ex_Row].
             Write(config);


}

void AtrViewer::QAlgoSettings::changeEnergyPolicy(QString EnergyType)
{
     DDS::P_Tactical_Sensor_ATR_Config_Extension_PSM::C_Tactical_Sensor_Config_Ex config = {};
#ifdef USE_NGVA
     config.A_sourceID.A_resourceId = m_map_video_id[m_current_video];
#else
     int16_t sys_id, mod_id;
     cvt_revers_barak_id(m_map_video_id[m_current_video], sys_id, mod_id);
     config.A_sourceID.A_systemId = sys_id;
     config.A_sourceID.A_moduleId = mod_id;
#endif
     DDS::P_Tactical_Sensor_ATR_Config_Extension_PSM::T_Config_EnergyPolicyType policy =
             DDS::P_Tactical_Sensor_ATR_Config_Extension_PSM::T_Config_EnergyPolicyType::L_Config_EnergyPolicyType_None;
     if(EnergyType.compare("15W_2CORE") == 0)
         policy = DDS::P_Tactical_Sensor_ATR_Config_Extension_PSM::T_Config_EnergyPolicyType::L_Config_EnergyPolicyType_MODE_15W_2CORE;
     else if(EnergyType.compare("15W_4CORE") == 0)
         policy = DDS::P_Tactical_Sensor_ATR_Config_Extension_PSM::T_Config_EnergyPolicyType::L_Config_EnergyPolicyType_MODE_15W_4_CORE;
     else if(EnergyType.compare("15W_6CORE") == 0)
         policy = DDS::P_Tactical_Sensor_ATR_Config_Extension_PSM::T_Config_EnergyPolicyType::L_Config_EnergyPolicyType_MODE_15W_6_CORE;
     else if(EnergyType.compare("10W_2CORE") == 0)
         policy = DDS::P_Tactical_Sensor_ATR_Config_Extension_PSM::T_Config_EnergyPolicyType::L_Config_EnergyPolicyType_MODE_10W_2_CORE;
     else if(EnergyType.compare("10W_4CORE") == 0)
         policy = DDS::P_Tactical_Sensor_ATR_Config_Extension_PSM::T_Config_EnergyPolicyType::L_Config_EnergyPolicyType_MODE_10W_4_CORE;
    else
         return;

    config.A_energyPolicyMode = policy;
    m_TacticalSensorExOutgoingTable[DDS::P_Tactical_Sensor_ATR_Config_Extension_PSM_Rows::C_Tactical_Sensor_Config_Ex_Row].
             Write(config);
}



void AtrViewer::QAlgoSettings::restartSystem()
{


    DDS::P_Tactical_Sensor_ATR_Config_Extension_PSM::C_Tactical_Sensor_Config_Ex config = {};
#ifdef USE_NGVA
    config.A_sourceID.A_resourceId = m_map_video_id[m_current_video];
#else
    int16_t sys_id, mod_id;
    cvt_revers_barak_id(m_map_video_id[m_current_video], sys_id, mod_id);
    config.A_sourceID.A_systemId = sys_id;
    config.A_sourceID.A_moduleId = mod_id;
#endif
    config.A_config_request = DDS::P_Tactical_Sensor_ATR_Config_Extension_PSM::T_Config_RequestType::L_Config_RequestType_Restart;

    m_TacticalSensorExOutgoingTable[DDS::P_Tactical_Sensor_ATR_Config_Extension_PSM_Rows::C_Tactical_Sensor_Config_Ex_Row].
            Write(config);

}
