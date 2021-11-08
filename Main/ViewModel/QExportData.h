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
#include <ViewModel/QProcessRecord.h>
#include <sensors/utils/capture.hpp>
#include <imaging/image_converter.h>
#include <sensors/capture/playback_sensor_interface.h>

#include <Helpers/Utils.h>

static constexpr core::imaging::pixel_format COLOR_IMAGES_FORMAT = core::imaging::pixel_format::BGR;
static constexpr core::imaging::pixel_format GRAY8_IMAGES_FORMAT = core::imaging::pixel_format::GRAY8;


namespace AtrViewer
{
    class QExportData: public QObject
    {
        Q_OBJECT

        QML_WRITABLE_AUTO_PROPERTY(quint64, totalTime)
        QML_WRITABLE_AUTO_PROPERTY(quint64, currentTime)

    public:

        ~QExportData(){}
        QExportData(QObject *parent = 0);
        Q_INVOKABLE bool exportRecord(const QString& file_name, const QString& output_dir);
        Q_INVOKABLE bool stop();
        Q_INVOKABLE bool isExporting();


    private:
        std::atomic<bool> m_isExporting;
        std::mutex m_mutex;
        utils::ref_count_ptr<sensors::capture::playback_sensor_interface> m_playback_sensor;
        utils::ref_count_ptr<core::data::data_callback_interface> m_callback;
        QString m_output_dir;
        uint32_t m_frame_counter;
        std::unordered_map<AtrViewer::detection_data_object::AlgoType, uint32_t> m_detection_type_counter;
        utils::ref_count_ptr<utils::capture::smart_playback_sensor_callback> m_playback_callback;
        utils::ref_count_ptr<core::imaging::image_algorithm_interface> m_color_image_converter;
        utils::ref_count_ptr<core::imaging::image_algorithm_interface> m_gray_image_converter;



        bool getConvertedImage(core::imaging::image_interface* input, core::imaging::image_interface** output)
        {

            if (input == nullptr || output == nullptr)
                return false;

            core::imaging::image_params image_params = {};
            if (input->query_image_params(image_params) == false)
                throw std::runtime_error("Not found image params");

            switch (image_params.format)
            {
            case core::imaging::pixel_format::BGRA:
            case core::imaging::pixel_format::RGBA:
            case core::imaging::pixel_format::BGR:
            case core::imaging::pixel_format::RGB:
            case core::imaging::pixel_format::I420:
            case core::imaging::pixel_format::NV12:
            case core::imaging::pixel_format::UYVY:
            case core::imaging::pixel_format::YUY2:
            {
                if (m_color_image_converter == nullptr)
                {
                    if (imaging::image_converter::create(COLOR_IMAGES_FORMAT, &m_color_image_converter) == false)
                         throw std::runtime_error("Failed to create color image converter");

                }
                return m_color_image_converter->apply(input, output);
            }
            case core::imaging::pixel_format::GRAY8:
            case core::imaging::pixel_format::GRAY16_LE:
            {
                if (m_gray_image_converter == nullptr)
                {
                    if (imaging::image_converter::create(GRAY8_IMAGES_FORMAT, &m_gray_image_converter) == false)
                        throw std::runtime_error("Failed to create color image converter");
                 }

                return m_gray_image_converter->apply(input, output);
            }

            default:
                throw std::runtime_error("format is not supported");
            }

            return false;
        }


        void save_image_file(const char* filePath, core::video::frame_interface* frame)
        {
            utils::ref_count_ptr<core::imaging::image_interface> converted_image;
            if (getConvertedImage(frame, &converted_image) == false)
                throw std::runtime_error("Failed to convert image");

            std::string local_file_path(filePath);
            QUtils::runAsyncWait([local_file_path, converted_image]()
            {
                try
                {
                    core::imaging::image_params image_params = {};
                    if (converted_image->query_image_params(image_params) == false)
                        throw std::runtime_error("Failed to query data from converted image");

                    utils::ref_count_ptr<core::buffer_interface> image_buffer;
                    if (converted_image->query_buffer(&image_buffer) == false)
                        throw std::runtime_error("Failed to query data from converted image");

                    QImage image;
                    if (image_params.format == COLOR_IMAGES_FORMAT)
                    {
                        image = QImage(
                                    image_buffer->data(),
                                    static_cast<int>(image_params.width),
                                    static_cast<int>(image_params.height),
                                    QImage::Format::Format_BGR888);
                    }
                    else if (image_params.format == GRAY8_IMAGES_FORMAT)
                    {
                        image = QImage(
                                    image_buffer->data(),
                                    static_cast<int>(image_params.width),
                                    static_cast<int>(image_params.height),
                                    QImage::Format::Format_Grayscale8);
                    }
                    else
                    {
                        throw std::runtime_error("Unexpected output image format");
                    }

                    image.save(QString(local_file_path.c_str()), "png");
                }
                catch (...)
                {
                    printf("error ----- save image \n");
                }
            });

        }
    };
}
