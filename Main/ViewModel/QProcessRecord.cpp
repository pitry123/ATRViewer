#include "QProcessRecord.h"
#include <QDateTime>
#include <QDir>
#include <QImage>
#include <QQuickItem>
#include <QQuickItemGrabResult>
#include <sensors/data/video/video_data_objets_interfaces.h>
#include <video/sources/shared_memory_video_source.h>
#include <sensors/video/video_sensor_interface.h>
#include <sensors/streams/compression_stream_interface.h>
#include <streams/memory_stream_interface.h>
#include <streams/file_stream_interface.h>
#include <compression/lz4_compression_interface.h>
#include <sensors/capture/recording_sensor_intreface.h>




AtrViewer::QProcessRecord::QProcessRecord(const Database::Row& TacticalSensorExIncomingTable, QObject *parent):
    QDatabaseConnector(nullptr, parent),
  m_tactical_sensor_incoming(TacticalSensorExIncomingTable)

{
    m_isRecording = false;
}

bool AtrViewer::QProcessRecord::startRecord(const QString& stream_name)
{
    if(isRecord())
        return false;

    if(m_shared_video_sensors_map.find(stream_name) == m_shared_video_sensors_map.end())
        return false;

    QString filePathName = "./" + stream_name;

    filePathName += QDateTime::currentDateTime().toString("yyyy-MM-dd_hh-mm-ss-zzz");

    utils::ref_count_ptr<core::stream_interface> file_stream_write;
    if (streams::file_stream_interface::create(
        filePathName.toStdString().c_str(),
        streams::file_stream_interface::access_mode::write,
        &file_stream_write) == false)
        throw std::runtime_error("failed to create file stream");

    utils::ref_count_ptr<core::compression_interface> compression;
    if (compression::lz4_compression_interface::create(9, &compression) == false)
        throw std::runtime_error("failed to create compression algorithm");

    utils::ref_count_ptr<core::stream_interface> stream_write;
    if (streams::compression_stream_interface::create(
        streams::compression_stream_interface::access_mode::write,
        compression,
        file_stream_write,
        &stream_write) == false)
        throw std::runtime_error("failed to create compression stream");

    utils::ref_count_ptr<core::sensors::sensor_interface> multi = utils::make_ref_count_ptr<utils::sensors::multi_sensor>(
        core::guid::undefined(),
        std::vector<utils::ref_count_ptr<core::sensors::sensor_interface>>
    {
        static_cast<core::sensors::sensor_interface*>(m_shared_video_sensors_map[stream_name]),
        static_cast<core::sensors::sensor_interface*>(m_detection_sensor_map[stream_name])
    });


    if (sensors::capture::recording_sensor_interface::create(core::guid::undefined(),multi,
                                                             stream_write, &m_record_sensor) == false)
        throw std::runtime_error("failed to create record sensor");

    m_isRecording = true;
    return true;
}


bool AtrViewer::QProcessRecord::endRecord()
{

    if(m_record_sensor != nullptr &&
            isRecord() == true)
    {
        m_record_sensor.release();
        m_isRecording = false;
    }


    return true;
}

bool AtrViewer::QProcessRecord::isRecord()
{
    return m_isRecording;
}

void AtrViewer::QProcessRecord::onNewVideoDetection(QString video_name, int resource_id)
{

    printf("DETECTED NEW VIDEO\n");
    utils::ref_count_ptr<core::video::video_source_interface> video_source;
    video::sources::shared_memory_video_source::create(video_name.toStdString().c_str(), &video_source);
    if (video_source == nullptr)
        return;

    utils::ref_count_ptr<core::sensors::sensor_interface> video_sensor;
    if (sensors::video::video_sensor_interface::create(VIDEO_SENSOR_ID, 0, video_source, &video_sensor) == false)
        return;

    utils::ref_count_ptr<AtrViewer::detection_sensor> detection_sensor =
            utils::make_ref_count_ptr<AtrViewer::detection_sensor>(DETECTION_SENSOR_ID, m_tactical_sensor_incoming, resource_id);


//    m_callback[video_name] = utils::make_ref_count_ptr<utils::data::smart_data_callback>([=](core::data::data_source_interface* sender,
//                                                                                           core::data::data_object_interface* data)
//    {
//            utils::ref_count_ptr<sensors::video::video_frame_data_object_interface> video_data =
//                utils::data::data_object_cast<sensors::video::video_frame_data_object_interface>(data);

//            if (video_data == nullptr)
//                return;

//            printf("aaaaa  %s\n", video_name.toStdString().c_str());
//    });

    video_source->start();

   // video_sensor->register_data_callback(m_callback[video_name]);
    m_shared_video_sensors_map[video_name] = video_sensor;
    m_detection_sensor_map[video_name] = detection_sensor;
}





