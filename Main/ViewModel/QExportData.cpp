#include "QExportData.h"
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
#include <sensors/data/capture/playback_data_object_interface.h>
#include <sensors/data/common/unknown_data_object_interface.h>
#include <iostream>


static inline QString toNativeFilePath(const QString& path)
{
    QString fixedPath;
    const QUrl url(path);
    if (url.isLocalFile())
    {
        return QDir::toNativeSeparators(url.toLocalFile());
    }
    else
    {
        return path;
    }
}



AtrViewer::QExportData::QExportData(QObject *parent): QObject(parent),
    m_totalTime(0),
    m_currentTime(0),
    m_isExporting(false),
    m_frame_counter(0)

{

    m_detection_type_counter[AtrViewer::detection_data_object::AlgoType::ATR] = 0;
    m_detection_type_counter[AtrViewer::detection_data_object::AlgoType::VMD] = 0;
    m_detection_type_counter[AtrViewer::detection_data_object::AlgoType::TRACK] = 0;

    m_playback_callback = utils::make_ref_count_ptr<utils::capture::smart_playback_sensor_callback>();

    m_playback_callback->state_changed += [&](sensors::capture::playback_state state)
    {
        if (state == sensors::capture::playback_state::STOPPED)
        {
            m_isExporting = false;
        }
    };

    m_callback = utils::make_ref_count_ptr<utils::data::smart_data_callback>([&](core::data::data_source_interface* sender, core::data::data_object_interface* data)
    {
            std::lock_guard<std::mutex> lock(m_mutex);
            set_currentTime(m_playback_sensor->current_time());
//            utils::ref_count_ptr<sensors::capture::playback_data_object_interface> playback_obj =
//                    utils::data::data_object_cast<sensors::capture::playback_data_object_interface>(data);

//            utils::ref_count_ptr<core::data::data_object_interface> wrapped_obj;
              uint64_t playback_timesptamp = 0;
//            if (playback_obj != nullptr)
//            {
//                playback_obj->query_object(&wrapped_obj);
//                playback_timesptamp = playback_obj->timestamp();
//            }
//            else
//            {
//                printf("ERR data\n");
//                return;
//            }

            if (data->id() == data::common::unknown_data_object_interface::ID())
            {
                utils::ref_count_ptr<data::common::unknown_data_object_interface> unknown_data =
                        utils::ref_count_ptr_cast<data::common::unknown_data_object_interface>(data);

                if (unknown_data == nullptr)
                    return;

                if((unknown_data->object_id() == sensors::video::video_frame_data_object_interface::ID()))
                {
                    if(!QDir(std::string(m_output_dir.toStdString() + "Video").c_str()).exists())
                        QDir().mkdir(std::string(m_output_dir.toStdString() + "Video").c_str());

                    utils::ref_count_ptr<sensors::video::video_frame_data_object_interface> video_data;

                    if (sensors::video::video_frame_data_object_interface::create((core::data::data_object_interface**)(&video_data)) == false)
                        return;

                    if (unknown_data->translate_to(video_data) == false)
                        return;

                    utils::ref_count_ptr<core::video::frame_interface> frame;
                    if (video_data->query_frame(&frame) == false)
                        return;

                    playback_timesptamp = video_data->timestamp();
                    video_data.release();
                    core::video::video_params frame_video_params = {};
                    frame->query_video_params(frame_video_params);
                    if (frame_video_params.data_type == core::video::H264)
                    {
                        printf("NOT SUPPORT YET.....\n");
                        return;
                    }
                    std::stringstream ss;
                    ss << m_output_dir.toStdString() <<
                          "Video/Frame_Number_" << m_frame_counter++ <<
                          "_ts_" << playback_timesptamp <<".png";

                    save_image_file(ss.str().c_str(), frame);
                }
                if((unknown_data->object_id() == AtrViewer::detection_data_object::ID()))
                {
                    if(!QDir(std::string(m_output_dir.toStdString() + "Detection").c_str()).exists())
                        QDir().mkdir(std::string(m_output_dir.toStdString() + "Detection").c_str());

                    utils::ref_count_ptr<AtrViewer::detection_data_object> detection =
                        utils::make_ref_count_ptr<AtrViewer::detection_data_object>();

                    if (!unknown_data->translate_to(detection))
                    {
                        return;
                    }

                    std::ofstream  detectionFile;
                    if (detection->m_type == AtrViewer::detection_data_object::AlgoType::ATR)
                    {
                        detectionFile.open(m_output_dir.toStdString() + "Detection/ATR.csv", std::ios::out | std::ios::app);
                    }
                    else if(detection->m_type == AtrViewer::detection_data_object::AlgoType::VMD)
                    {
                        detectionFile.open(m_output_dir.toStdString() + "Detection/VMD.csv", std::ios::out | std::ios::app);
                    }
                    else if(detection->m_type == AtrViewer::detection_data_object::AlgoType::TRACK)
                    {
                        detectionFile.open(m_output_dir.toStdString() + "Detection/TRACK.csv", std::ios::out | std::ios::app);
                    }
                    if(m_detection_type_counter[detection->m_type] == 0)
                    {
                        detectionFile << "Index,Timestamp,SourceId,VideoColumn,VideoRow,VideoHeight,VideoWidth,Classification,Range,IdDetection,classification_confidence,ForceTypes,forceType_confidence\n";
                    }
                    playback_timesptamp = detection->m_timestamp;
                    detectionFile << m_detection_type_counter[detection->m_type]++ << "," <<
                                    playback_timesptamp <<  "," <<
                                    detection->m_source_id <<   "," <<
                                    detection->m_videoColumn << "," <<
                                    detection->m_videoRow <<    "," <<
                                    detection->m_videoHeight << "," <<
                                    detection->m_videoWidth <<  "," <<
                                    detection->m_classification <<  "," <<
                                    detection->m_range <<   "," <<
                                    detection->m_id_detection << "," <<
                                    detection->m_classification_confidence <<  "," <<
                                    detection->m_ForceTypes <<   "," <<
                                    detection->m_forceType_confidence << "\n";
                    detectionFile.close();
                }
            }
    });

}


bool AtrViewer::QExportData::stop()
{
    std::lock_guard<std::mutex> lock(m_mutex);
	if (m_playback_sensor == nullptr)
		return false;

    m_playback_sensor->unregister_data_callback(m_callback);
    m_playback_sensor = nullptr;
    return true;
}

bool AtrViewer::QExportData::isExporting()
{
    return m_isExporting.load();
}

bool AtrViewer::QExportData::exportRecord(const QString& file_name, const QString& output_dir)
{
	stop();

    QString fixed_path =  toNativeFilePath(output_dir);

    utils::ref_count_ptr<core::stream_interface> file_stream_read;
    if (streams::file_stream_interface::create(
        file_name.toStdString().c_str(),
        streams::file_stream_interface::access_mode::read,
        &file_stream_read) == false)
        return false;

    utils::ref_count_ptr<core::compression_interface> compression;
    if (compression::lz4_compression_interface::create(9, &compression) == false)
        return false;

    utils::ref_count_ptr<core::stream_interface> stream_read;
    if (streams::compression_stream_interface::create(
        streams::compression_stream_interface::access_mode::read,
        compression,
        file_stream_read,
        &stream_read) == false)
        return false;

    if (sensors::capture::playback_sensor_interface::create(core::guid::undefined(), stream_read, nullptr, false,
                                                             sensors::capture::playback_speed::MAX_PLAYBACK_SPEED, false,
                                                            sensors::capture::playback_data_mode::DATA_ONLY, &m_playback_sensor) == false)
        return false;

    m_playback_sensor->register_data_callback(m_callback);

    m_output_dir = fixed_path;
    m_isExporting = true;
    m_frame_counter = 0;
    m_totalTime = m_playback_sensor->total_time();
    m_currentTime = 0;
    m_playback_sensor->start();

    return true;
}
