#pragma once
#include <QObject>
#include <QMetaType>
#include <Helpers/QQmlAutoPropertyHelpers.h>
#include <ViewModel/QDatabaseConnector.h>
#include <ViewModel/QGuiTypes.h>

#include <ParamStore.h>

#ifdef USE_NGVA
#include <ProjectCommon.h>
#include <Tactical_Sensor_Extension_PSM_Gateway.hpp>
#include <Tactical_Sensor_ATR_Config_Extension_PSM_Gateway.hpp>
#else
#include <BarakProjectCommon.h>
#include <Tactical_Sensor_ATR_Config_Extension_PSM_Gateway.hpp>
#endif

namespace AtrViewer
{

    class QAlgoSettings: public QDatabaseConnector
    {
        Q_OBJECT

        QML_WRITABLE_AUTO_PROPERTY(bool, atr_algo_en)
        QML_WRITABLE_AUTO_PROPERTY(bool, vmd_algo_en)
        QML_WRITABLE_AUTO_PROPERTY(bool, track_algo_en)

        QML_WRITABLE_AUTO_PROPERTY(int, atr_algo_pd)
        QML_WRITABLE_AUTO_PROPERTY(int, vmd_algo_pd)
        QML_WRITABLE_AUTO_PROPERTY(int, track_algo_pd)

        public:
            QAlgoSettings(const Database::Table& XmlConfigTable,
                        const Database::Table& TacticalSensorExIncomingTable,
                        const Database::Table& TacticalSensorExOutgoingTable,
                        const Database::Table& TacticalSensorExStatesTable,
                        const Utils::Context &context = nullptr,
                        QObject *parent = nullptr);
            explicit QAlgoSettings(QObject *parent = nullptr);

            virtual void Start() override;
            virtual void Stop() override;

            enum algoType
            {
                ATR,
                VMD,
                TRACK
            };


        private:

            Database::Table		m_XmlConfigurationTable;
            Database::Table		m_TacticalSensorExIncomingTable;
            Database::Table		m_TacticalSensorExOutgoingTable;
            Database::Table		m_TacticalSensorExStatesTable;


            std::map<QString, std::map<algoType, bool>> m_video_algo_setting;
            std::map<QString, std::map<algoType, double>> m_video_algo_pd_setting;
            std::map<QString, int> m_map_video_id;
            QString m_current_video;
            std::mutex m_video_channel_mutex;
            std::unordered_map<QString, int> m_energy_policy_map;

            inline algoType convert_to_algoType(const QString& name);


    public:

            Q_INVOKABLE void changeVideo(QString appSrcName);
            Q_INVOKABLE void listNewVideo(QString appSrcName,  int resourceId);
            Q_INVOKABLE void changeAlgoState(QString AlgoName, bool state);
            Q_INVOKABLE void setDefault();
            Q_INVOKABLE void saveAsDefault();

            Q_INVOKABLE void changePdValue(QString AlgoName, double pd);
            Q_INVOKABLE void changeAINetwork(QString NetworkName);
            Q_INVOKABLE void changeEnergyPolicy(QString EnergyType);
            Q_INVOKABLE void restartSystem();
    };
}
