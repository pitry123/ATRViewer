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
#include <VideoTracking_Extension_PSM_Gateway.hpp>
#else
#include <BarakProjectCommon.h>
#include <Tactical_Sensor_PSM_Gateway.hpp>
#endif

namespace AtrViewer
{
    class QBBoxParams : public QObject
    {
        Q_OBJECT
        QML_WRITABLE_AUTO_PROPERTY(BBoxParams, params)

    public:
        QBBoxParams(int x, int y, int height, int width, int classification, double range, double idTrack, QString color, QObject* parent = nullptr);
        QBBoxParams(BBoxParams params, QObject* parent = nullptr);
        explicit QBBoxParams(QObject* parent = nullptr);
        QBBoxParams(const QBBoxParams& other);
        virtual ~QBBoxParams() override;
    };

    QML_LIST_CLASS(BBoxParamsListBase, QBBoxParams)

    class QTracking : public QDatabaseConnector
    {
        Q_OBJECT
        QML_WRITABLE_AUTO_PROPERTY(BBoxParams, trackedObjch1)
        QML_WRITABLE_AUTO_PROPERTY(BBoxParams, trackedObjch2)
        // Multi
        //ATR
        QML_WRITABLE_AUTO_PROPERTY(QVariantList, atrBoundingBoxCh0List)
        QML_WRITABLE_AUTO_PROPERTY(QVariantList, atrBoundingBoxCh1List)
        QML_WRITABLE_AUTO_PROPERTY(QVariantList, atrBoundingBoxCh2List)
        QML_WRITABLE_AUTO_PROPERTY(QVariantList, atrBoundingBoxCh3List)
        // VMD
        QML_WRITABLE_AUTO_PROPERTY(QVariantList, vmdBoundingBoxCh0List)
        QML_WRITABLE_AUTO_PROPERTY(QVariantList, vmdBoundingBoxCh1List)
        QML_WRITABLE_AUTO_PROPERTY(QVariantList, vmdBoundingBoxCh2List)
        QML_WRITABLE_AUTO_PROPERTY(QVariantList, vmdBoundingBoxCh3List)
        //TRACK
        QML_WRITABLE_AUTO_PROPERTY(QVariantList, trackBoundingBoxCh0List)
        QML_WRITABLE_AUTO_PROPERTY(QVariantList, trackBoundingBoxCh1List)
        QML_WRITABLE_AUTO_PROPERTY(QVariantList, trackBoundingBoxCh2List)
        QML_WRITABLE_AUTO_PROPERTY(QVariantList, trackBoundingBoxCh3List)

        QVariantList m_BboxList;
        QMap<int, QVariantList> m_AtrChannelsBboxList;
        QMap<int, QVariantList> m_VmdChannelsBboxList;
        QMap<int, QVariantList> m_TrackChannelsBboxList;

        public:
            QTracking(const Database::Table& XmlConfigTable,
                        const Database::Table& TacticalSensorExIncomingTable,
                        const Database::Table& TacticalSensorExOutgoingTable,
                        const Database::Table& TacticalSensorExStatesTable,
                        const Database::Table& VideoTrakcingExIncomingTable,
                        const Database::Table& VideoTrakcingExOutgoingTable,
                        const Database::Table& VideoTrakcingExStatesTable,
                        const Utils::Context &context = nullptr,
                        QObject *parent = nullptr);
            explicit QTracking(QObject *parent = nullptr);

            virtual void Start() override;
            virtual void Stop() override;

            Q_INVOKABLE void track_request(int video_id, double x, double y, double w, double h);
            Q_INVOKABLE void stop_tracking(int video_id);
        private:

            Database::Table		m_XmlConfigurationTable;
            Database::Table		m_TacticalSensorExIncomingTable;
            Database::Table		m_TacticalSensorExOutgoingTable;
            Database::Table		m_TacticalSensorExStatesTable;

            Database::Table		m_VideoTrakcingExIncomingTable;
            Database::Table		m_VideoTrakcingExOutgoingTable;
            Database::Table		m_VideoTrakcingExStatesTable;

            std::map<int, QString> m_color_classification;
            std::map<int, bool> m_filter_classification;

            std::map<uint32_t, int32_t> m_video_channel_by_resource;

            std::mutex m_video_channel_mutex;

            std::vector<QString> m_color_vector;

            void insert_random_color();

#ifdef USE_NGVA

            void atr_detection_handler(const NGVA::P_Tactical_Sensor_Extension_PSM::C_Detection_Location_Ex& detectionLocation);
            void vmd_detection_handler(const NGVA::P_Tactical_Sensor_Extension_PSM::C_Detection_Location_Ex& detectionLocation);
            void track_detection_handler(const NGVA::P_Tactical_Sensor_Extension_PSM::C_Detection_Location_Ex& detectionLocation);
#else
            void atr_detection_handler(const Barak::P_Tactical_Sensor_PSM::C_Detection_Location& detectionLocation);
            void vmd_detection_handler(const Barak::P_Tactical_Sensor_PSM::C_Detection_Location& detectionLocation);
            void track_detection_handler(const Barak::P_Tactical_Sensor_PSM::C_Detection_Location& detectionLocation);

#endif
        public slots:
            void onVideoChannelInputChanged(int ch, int resource_video);
    };
}
