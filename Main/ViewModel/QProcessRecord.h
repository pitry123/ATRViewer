#pragma once
#include <QObject>
#include <QQuickWindow>
#include <QMetaType>
#include "QGuiTypes.h"
#include <unordered_map>
#include <Helpers/QQmlAutoPropertyHelpers.h>
#include <sensors/core/sensor_interface.h>
#include <sensors/utils/data.hpp>
#include <sensors/utils/sensors.hpp>
#include <ViewModel/QDatabaseConnector.h>
#ifdef USE_NGVA
#include <ProjectCommon.h>
#include <Tactical_Sensor_Extension_PSM_Gateway.hpp>
#else
#include <BarakProjectCommon.h>
#include <Tactical_Sensor_PSM_Gateway.hpp>
#endif



static const core::guid DETECTION_SENSOR_ID =
{ 0x11111111, 0x1111, 0x1111, { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } };

static const core::guid VIDEO_SENSOR_ID =
{ 0x22222222, 0x2222, 0x2222, { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } };


namespace AtrViewer
{

    class detectionDispatcher : public QDatabaseConnector
    {
        Q_OBJECT

        Database::Row m_tactical_row;

    public:

        detectionDispatcher(const Database::Row& tactical_row, const Utils::Context &context = nullptr,QObject *parent = nullptr):
            QDatabaseConnector(context,parent),
            m_tactical_row(tactical_row)
        {}

        void register_callback(std::function<void(const Database::RowData& data)> callback)
        {
            Subscribe(m_tactical_row, callback);
        }
        ~detectionDispatcher() {}

    };

    class detection_data_object : public utils::data::data_object_base<core::data::data_object_interface>
    {
    public:

        // {8E5D5537-22B2-4988-A3FD-9D94B845FDBA}
        static const core::guid ID()
        {
            return { 0x8e5d5537, 0x22b2, 0x4988, { 0xa3, 0xfd, 0x9d, 0x94, 0xb8, 0x45, 0xfd, 0xba } };
        }


        enum AlgoType
        {
            NONE = -1,
            ATR,
            VMD,
            TRACK
        };



        AlgoType m_type;
        uint32_t m_source_id;
        uint64_t m_timestamp;
        double   m_videoColumn;
        double   m_videoRow;
        double   m_videoHeight;
        double   m_videoWidth;
        uint32_t m_classification;
        double   m_range;
        double   m_id_detection;
        double   m_classification_confidence;
        uint32_t m_ForceTypes;
        double   m_forceType_confidence;


        //


        detection_data_object():
            m_type(AlgoType::NONE),
            m_source_id(0),
            m_timestamp(0),
            m_videoColumn(0),
            m_videoRow(0),
            m_videoHeight(0),
            m_videoWidth(0),
            m_classification(0),
            m_range(0),
            m_id_detection(0)
        {}

        detection_data_object(AlgoType type, uint32_t source_id, uint64_t timestamp, double videoCol, double videoRow,
                              double videoHeight, double videoWidth, uint32_t classification, double range, double id_detection,
                              double classification_confidence, uint32_t ForceTypes, double forceType_confidence):
            m_type(type),
            m_source_id(source_id),
            m_timestamp(timestamp),
            m_videoColumn(videoCol),
            m_videoRow(videoRow),
            m_videoHeight(videoHeight),
            m_videoWidth(videoWidth),
            m_classification(classification),
            m_range(range),
            m_id_detection(id_detection),
            m_classification_confidence(classification_confidence),
            m_ForceTypes(ForceTypes),
            m_forceType_confidence(forceType_confidence)
        {}

        detection_data_object(const detection_data_object& other):
            m_type(other.m_type),
            m_source_id(other.m_source_id),
            m_timestamp(other.m_timestamp),
            m_videoColumn(other.m_videoColumn),
            m_videoRow(other.m_videoRow),
            m_videoHeight(other.m_videoHeight),
            m_videoWidth(other.m_videoWidth),
            m_classification(other.m_classification),
            m_range(other.m_range),
            m_id_detection(other.m_id_detection),
            m_classification_confidence(other.m_classification_confidence),
            m_ForceTypes(other.m_ForceTypes),
            m_forceType_confidence(other.m_forceType_confidence)
        {}

        virtual core::guid id() const override
        {
            return ID();
        }

        virtual uint64_t timestamp() const override
        {
            return m_timestamp;
        }

        virtual uint64_t data_size() const override
        {
            return (sizeof(double) * 8) + (sizeof(uint32_t) * 3) + sizeof (AlgoType) + sizeof(uint64_t);
        }

        virtual bool serialize(core::stream_interface& stream) const override
        {

            if (stream.write_object<AlgoType>(m_type) != core::stream_status::status_no_error)
                return false;

            if (stream.write_object<uint32_t>(m_source_id) != core::stream_status::status_no_error)
                return false;

            if (stream.write_object<uint64_t>(m_timestamp) != core::stream_status::status_no_error)
                return false;

            if (stream.write_object<double>(m_videoColumn) != core::stream_status::status_no_error)
                return false;

            if (stream.write_object<double>(m_videoRow) != core::stream_status::status_no_error)
                return false;

            if (stream.write_object<double>(m_videoHeight) != core::stream_status::status_no_error)
                return false;

            if (stream.write_object<double>(m_videoWidth) != core::stream_status::status_no_error)
                return false;

            if (stream.write_object<uint32_t>(m_classification) != core::stream_status::status_no_error)
                return false;

            if (stream.write_object<double>(m_range) != core::stream_status::status_no_error)
                return false;

            if (stream.write_object<double>(m_id_detection) != core::stream_status::status_no_error)
                return false;

            if (stream.write_object<double>(m_classification_confidence) != core::stream_status::status_no_error)
                return false;

            if (stream.write_object<uint32_t>(m_ForceTypes) != core::stream_status::status_no_error)
                return false;

            if (stream.write_object<double>(m_forceType_confidence) != core::stream_status::status_no_error)
                return false;


            return true;
        }

        virtual bool deserialize(core::stream_interface& stream) override
        {

            AlgoType type;
            uint32_t source_id, classification;
            uint64_t timestamp;
            double videoColumn, videoRow, videoHeight, videoWidth;
            double range,id_detection;
            double   classification_confidence, forceType_confidence;
            uint32_t ForceTypes;

            if (stream.read_object<AlgoType>(type) != core::stream_status::status_no_error)
                return false;

            if (stream.read_object<uint32_t>(source_id) != core::stream_status::status_no_error)
                return false;

            if (stream.read_object<uint64_t>(timestamp) != core::stream_status::status_no_error)
                return false;

            if (stream.read_object<double>(videoColumn) != core::stream_status::status_no_error)
                return false;

            if (stream.read_object<double>(videoRow) != core::stream_status::status_no_error)
                return false;

            if (stream.read_object<double>(videoHeight) != core::stream_status::status_no_error)
                return false;

            if (stream.read_object<double>(videoWidth) != core::stream_status::status_no_error)
                return false;

            if (stream.read_object<uint32_t>(classification) != core::stream_status::status_no_error)
                return false;

            if (stream.read_object<double>(range) != core::stream_status::status_no_error)
                return false;

            if (stream.read_object<double>(id_detection) != core::stream_status::status_no_error)
                return false;

            if (stream.read_object<double>(classification_confidence) != core::stream_status::status_no_error)
                return false;

            if (stream.read_object<uint32_t>(ForceTypes) != core::stream_status::status_no_error)
                return false;

            if (stream.read_object<double>(forceType_confidence) != core::stream_status::status_no_error)
                return false;


            m_type = type;
            m_source_id = source_id;
            m_timestamp = timestamp;
            m_classification = classification;
            m_videoColumn = videoColumn;
            m_videoRow = videoRow;
            m_videoHeight = videoHeight;
            m_videoWidth = videoWidth;
            m_range = range;
            m_id_detection = id_detection;
            m_classification_confidence = classification_confidence;
            m_forceType_confidence = forceType_confidence;
            m_ForceTypes = ForceTypes;
            return true;
        }


        virtual bool clone(core::data::data_object_interface** obj) override
        {
            if (obj == nullptr)
                return false;

            utils::ref_count_ptr<core::data::data_object_interface> instance;
            try
            {
                instance = utils::make_ref_count_ptr<detection_data_object>(*this);
            }
            catch (...)
            {
                return false;
            }

            if (instance == nullptr)
                return false;

            *obj = instance;
            (*obj)->add_ref();
            return true;
        }

    };

    class detection_sensor : public utils::sensors::sensor_base<core::sensors::sensor_interface>
    {
        int32_t m_resource_id;
        detectionDispatcher m_detection_dispatcher;

    public:

        detection_sensor(const core::guid& id,
                         const Database::Row& TacticalSensorExIncomingTable,
                         int32_t resource_id):
            utils::sensors::sensor_base<core::sensors::sensor_interface>(id),
            m_resource_id(resource_id),
            m_detection_dispatcher(TacticalSensorExIncomingTable)
        {
            m_detection_dispatcher.register_callback([this](const Database::RowData& data)
            {

#ifdef USE_NGVA
                NGVA::P_Tactical_Sensor_Extension_PSM::C_Detection_Location_Ex detectionLocation =
                    data.Read<NGVA::P_Tactical_Sensor_Extension_PSM::C_Detection_Location_Ex>();

                if(detectionLocation.A_detection_data_sourceID.A_resourceId == m_resource_id)
                {
                    QString recognizingDetectorStr(detectionLocation.A_recognizingDetectorTypes[0].str);
                    int classification = static_cast<int>(detectionLocation.A_object_Classisfication);

                    AtrViewer::detection_data_object::AlgoType t = AtrViewer::detection_data_object::AlgoType::NONE;
                    if( recognizingDetectorStr.compare("ATR") == 0)
                        t = AtrViewer::detection_data_object::AlgoType::ATR;
                    else if (recognizingDetectorStr.compare("VMD") == 0)
                        t = AtrViewer::detection_data_object::AlgoType::VMD;
                    else if (recognizingDetectorStr.compare("TRACK") == 0)
                        t = AtrViewer::detection_data_object::AlgoType::TRACK;

                    uint64_t timestamp =
                            static_cast<uint64_t>((detectionLocation.A_timeOfDataGeneration.A_second * 1000000000 +
                                                   detectionLocation.A_timeOfDataGeneration.A_nanoseconds));
                    utils::ref_count_ptr<AtrViewer::detection_data_object> data_obj =
                            utils::make_ref_count_ptr<AtrViewer::detection_data_object>(t,
                                                                                        (uint32_t)m_resource_id,
                                                                                        timestamp,
                                                                                        detectionLocation.A_videoLocation.A_videoColumn,
                                                                                        detectionLocation.A_videoLocation.A_videoRow,
                                                                                        detectionLocation.A_videoBoundingBox.A_videoHeight,
                                                                                        detectionLocation.A_videoBoundingBox.A_videoWidth,
                                                                                        classification,
                                                                                        detectionLocation.A_absolutePolarLocation.A_range,
                                                                                        detectionLocation.A_detection_data_Guid.A_LSB,
                                                                                        detectionLocation.A_classification_confidence,
                                                                                        static_cast<uint32_t>(detectionLocation.A_force_Type),
                                                                                        detectionLocation.A_forceType_confidence);


                    this->raise(data_obj);
                }
#endif
            });
        }

        virtual bool query_sub_sensor(size_t index, core::sensors::sensor_interface** sensor) override
        {
            return false;
        }

        ~detection_sensor(){}

    };

    class QProcessRecord: public QDatabaseConnector
    {
        Q_OBJECT

    public:

        QProcessRecord() = default;

        QProcessRecord(const Database::Row& TacticalSensorExIncomingTable, QObject *parent = 0);

        Q_INVOKABLE bool startRecord(const QString& stream_name);
        Q_INVOKABLE bool endRecord();
        Q_INVOKABLE bool isRecord();


    public:
        bool m_isRecording;
        utils::dispatcher m_worker_dispo;
        Database::Row m_tactical_sensor_incoming;

        using coreSensors = std::vector<utils::ref_count_ptr<core::sensors::sensor_interface>>;
        utils::thread_safe_object<coreSensors> m_sensors;
        uint32_t m_current_video_resource_id;
        std::unordered_map<QString, utils::ref_count_ptr<core::sensors::sensor_interface>> m_shared_video_sensors_map;
        std::unordered_map<QString, utils::ref_count_ptr<core::sensors::sensor_interface>> m_detection_sensor_map;

        utils::ref_count_ptr<core::sensors::sensor_interface> m_record_sensor;

        std::unordered_map<QString, utils::ref_count_ptr<core::data::data_callback_interface>> m_callback;



    public slots:

        void onNewVideoDetection(QString video_name, int resource_id);
    };
}
