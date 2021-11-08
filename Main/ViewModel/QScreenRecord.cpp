#include "QScreenRecord.h"
#include <QDateTime>
#include <QDir>
#include <QImage>
#include <QQuickItem>
#include <QQuickItemGrabResult>

AtrViewer::QScreenRecord::QScreenRecord(QObject *parent): QObject(parent)
{
    m_isRecording = false;
}

class qimage_buffer : public utils::ref_count_base<core::buffer_interface>
{
private:
    QImage m_image;

public:
    qimage_buffer(const QImage& image) :
        m_image(image)
    {
    }

    virtual size_t size() const override
    {
        return static_cast<size_t>(m_image.height() * m_image.bytesPerLine());
    }

    virtual uint8_t* data() override
    {
        return reinterpret_cast<uint8_t*>(m_image.bits());
    }
};

class qimage_frame : public utils::ref_count_base<core::video::frame_interface>
{
private:
    core::imaging::image_params m_image_params;
    utils::ref_count_ptr<core::buffer_interface> m_buffer;
    core::video::display_params m_display_params;
    core::video::video_params m_video_params;

public:
    qimage_frame(const QImage& image) :
        m_image_params({}),
        m_display_params({}),
        m_video_params({})
    {
        m_buffer = utils::make_ref_count_ptr<qimage_buffer>(image);

        m_image_params.width = static_cast<uint32_t>(image.width());
        m_image_params.height = static_cast<uint32_t>(image.height());
        m_image_params.size = static_cast<uint32_t>(m_buffer->size());
        m_image_params.format = core::imaging::pixel_format::RGB; // TODO: Fix me!!!

        m_video_params.framerate.numerator = 50;
        m_video_params.framerate.denominator = 1;
        m_video_params.data_type = core::video::video_data_type::RAW;
        m_video_params.interlace_mode = core::video::interlace_mode::PROGRESSIVE;
    }

    virtual bool query_image_params(core::imaging::image_params& image_params) const override
    {
        image_params = m_image_params;
        return true;
    }

    virtual bool query_buffer(core::buffer_interface** buffer) const override
    {
        if (buffer == nullptr)
            return false;

        (*buffer) = m_buffer;
        (*buffer)->add_ref();
        return true;
    }

    virtual bool query_display_params(core::video::display_params& display_params) const override
    {
        display_params = m_display_params;
        return true;
    }

    virtual bool query_video_params(core::video::video_params& video_params) const override
    {
        video_params = m_video_params;
        return true;
    }
};

bool AtrViewer::QScreenRecord::record(QQuickItem* item, int w, int h)
{
    QDir dir;
    // Create a directory to store video
    if (false == dir.exists("/myPath/"))
    {
        dir.mkpath("/myPath/");
    }

    if (m_isRecording == false)
    {
        QString filePathName = "./";
        filePathName += QDateTime::currentDateTime().toString("yyyy-MM-dd_hh-mm-ss-zzz");
        filePathName += QString(".avi");

        //m_writer.open(filePathName.toStdString(), cv::VideoWriter::fourcc('M','J','P','G'), 20, cv::Size(1000,1000), true);
        //m_writer.open(filePathName.toStdString(), cv::VideoWriter::fourcc('X', 'V', 'I', 'D'), 20, cv::Size(w,h), true);
        m_sink.release();
        if (video::sinks::gstreamer_mfx_h264enc::create(filePathName.toStdString().c_str(), 0, &m_sink) == false)
            throw std::runtime_error("Failed to create video encoder");

        m_sink->start();
        m_isRecording = true;
    }


    //QImage image = rootWindow->grabWindow().copy();
    auto pimage = item->grabToImage();
    connect(pimage.data(), &QQuickItemGrabResult::ready, [=]() mutable
    {
        utils::scope_guard releaser([&]()
        {
            pimage = nullptr;
        });

//        if(m_worker_dispo.idle() == false)
//        {
//            printf("frame droped\n");
//            return;
//        }

        QImage image = pimage->image();

        m_worker_dispo.begin_invoke([=]()
        {
            auto cimage = image.convertToFormat(QImage::Format_RGB888);
            utils::ref_count_ptr<core::video::frame_interface> frame = utils::make_ref_count_ptr<qimage_frame>(cimage);

            // QImage converted to mat
            //cv::Mat mat(cimage.height(), cimage.width(), CV_8UC3, (void*)cimage.constBits(), cimage.bytesPerLine());
            //cv::cvtColor(mat, mat, CV_RGB2BGR); // opencv stored as BGR, it needs to be converted

            // write a picture
            //m_writer.write(mat);
            if(m_sink != nullptr)
                m_sink->set_frame(frame);
        });
    });


    return true;
}



void AtrViewer::QScreenRecord::endRecord()
{    
    m_isRecording = false;
    m_worker_dispo.sync();
    m_sink->stop();
    m_sink.release();
    //m_writer.release();
}
