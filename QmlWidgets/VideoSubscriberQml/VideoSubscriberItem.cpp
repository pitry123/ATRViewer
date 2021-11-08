#include "VideoSubscriberItem.h"
#include <imaging/image_converter.h>

unsigned int VideoSourceBuffer::BitsPerPixel(core::imaging::pixel_format format)
{
    unsigned int retVal = 0;

    switch (format)
    {
    case core::imaging::pixel_format::RGB:
        retVal = 24;
        break;
    case core::imaging::pixel_format::RGBA:
        retVal = 32;
        break;
    case core::imaging::pixel_format::BGR:
        retVal = 24;
        break;
    case core::imaging::pixel_format::BGRA:
        retVal = 32;
        break;
    case core::imaging::pixel_format::I420:
        retVal = 8;
        break;
    case core::imaging::pixel_format::NV12:
        retVal = 8;
        break;
    case core::imaging::pixel_format::YUY2:
        retVal = 8;
        break;
    case core::imaging::pixel_format::UYVY:
        retVal = 8;
        break;
    default:
        // Do nothing...
        break;
    }

    return retVal;
}

VideoSourceBuffer::VideoSourceBuffer(core::video::frame_interface *frame) : QAbstractVideoBuffer(QAbstractVideoBuffer::NoHandle),
    m_frame(frame)
{
}

VideoSourceBuffer::~VideoSourceBuffer()
{
}

QAbstractVideoBuffer::MapMode VideoSourceBuffer::mapMode() const
{
    if (m_frame == nullptr)
        return MapMode::NotMapped;

    return MapMode::ReadOnly;
}

uchar* VideoSourceBuffer::map(QAbstractVideoBuffer::MapMode mode, int* numBytes, int* bytesPerLine)
{
    if (m_frame == nullptr)
        return nullptr;

    if (mode != MapMode::ReadOnly)
        return nullptr;

    core::imaging::image_params frame_params = {};
    if (m_frame->query_image_params(frame_params) == false)
        return nullptr;

    utils::ref_count_ptr<core::buffer_interface> buffer;
    if (m_frame->query_buffer(&buffer) == false)
        return nullptr;

    *bytesPerLine = static_cast<int>((frame_params.width * BitsPerPixel(frame_params.format)) / 8);
    if ((*bytesPerLine) == 0)
        return nullptr;

    *numBytes = static_cast<int>(buffer->size());
    return static_cast<uchar*>(buffer->data());
}

void VideoSourceBuffer::unmap()
{
    // Do nothing...
}

QVideoFrame::PixelFormat VideoSubscriberItem::ConvertPixelForamt(core::imaging::pixel_format format)
{
    QVideoFrame::PixelFormat retval = QVideoFrame::PixelFormat::Format_Invalid;

    switch (format)
    {
    case core::imaging::pixel_format::RGB:
        retval = QVideoFrame::PixelFormat::Format_BGR24;
        break;
    case core::imaging::pixel_format::RGBA:
        retval = QVideoFrame::PixelFormat::Format_BGR32;
        break;
    case core::imaging::pixel_format::BGR:
        retval = QVideoFrame::PixelFormat::Format_RGB24;
        break;
    case core::imaging::pixel_format::BGRA:
        retval = QVideoFrame::PixelFormat::Format_RGB32;
        break;
    case core::imaging::pixel_format::I420:
        retval = QVideoFrame::PixelFormat::Format_YUV420P;
        break;
    case core::imaging::pixel_format::NV12:
        retval = QVideoFrame::PixelFormat::Format_NV12;
        break;
    case core::imaging::pixel_format::YUY2:
        retval = QVideoFrame::PixelFormat::Format_YUYV;
        break;
    case core::imaging::pixel_format::UYVY:
        retval = QVideoFrame::PixelFormat::Format_UYVY;
        break;
    default:
        // Do Nothing...
        break;
    }

    return retval;
}

bool VideoSubscriberItem::CreateSource(QString video_name)
{
    m_source.release();
    m_callback.release();

    std::string video_name_str = video_name.toStdString();
    if (video::sources::shared_memory_video_source::create(video_name_str.c_str(), &m_source) == false)
        return false;

    m_callback = utils::make_ref_count_ptr<utils::video::smart_frame_callback>([this](core::video::frame_interface* frame)
    {
            utils::ref_count_ptr<core::video::frame_interface> local_frame = frame;

            core::imaging::image_params image_params = {};
            if (local_frame->query_image_params(image_params) == false)
                return;

            core::video::display_params display_params = {};
            if (local_frame->query_display_params(display_params) == false)
                return;

            //printf("time %lld", display_params.timestamp);

             //uint64_t timestamp_now_micro = static_cast<uint64_t>(std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch()).count());

            // printf("atr wrapper[%lld]   atr viewer[%lld]   wrapper-view[%lld]\n",display_params.timestamp, timestamp_now_micro, display_params.timestamp - timestamp_now_micro);



            core::video::video_params video_params = {};
            if (local_frame->query_video_params(video_params) == false)
                return; // TODO: Log...

            // Hack
            if (image_params.format == core::imaging::pixel_format::BGR)
            {
                if (m_image_converter == nullptr)
                {
                    if (imaging::image_converter::create(core::imaging::pixel_format::BGRA, &m_image_converter) == false)
                        throw std::runtime_error("Failed to create image algorithm");
                }

                utils::ref_count_ptr<core::imaging::image_interface> image;
                if (m_image_converter->apply(local_frame, &image) == false || image->query_image_params(image_params) == false)
                    return; // TODO: Log...

                utils::ref_count_ptr<core::buffer_interface> image_buffer;
                if (image->query_buffer(&image_buffer) == false)
                    return; // TODO: Log...

                utils::ref_count_ptr<core::video::frame_interface> converted_frame =
                    utils::make_ref_count_ptr<utils::video::frame_base<core::video::frame_interface>>(image_params, display_params, video_params, image_buffer);

                local_frame = converted_frame;
            }

            QVideoFrame::PixelFormat qFormat = ConvertPixelForamt(image_params.format);
            if (qFormat == QVideoFrame::PixelFormat::Format_Invalid)
                return;

            VideoSourceBuffer* buffer = new VideoSourceBuffer(local_frame);
            QVideoFrame qframe(buffer, QSize(static_cast<int>(image_params.width), static_cast<int>(image_params.height)), qFormat);

            qframe.setFieldType((video_params.interlace_mode == core::video::interlace_mode::PROGRESSIVE) ?
                                    QVideoFrame::FieldType::ProgressiveFrame :
                                    QVideoFrame::FieldType::InterlacedFrame);

            qframe.setStartTime(static_cast<qint64>(display_params.pts));
            qframe.setEndTime(static_cast<qint64>(display_params.pts + display_params.duration));

            emit newFrame(qframe);
    });

    if (m_source->add_frame_callback(m_callback) == false)
        return false;

    m_source->start();
    return true;
}

void VideoSubscriberItem::setVideoTimeout(bool timeout)
{
    if (m_video_timeout == timeout)
        return;

    m_video_timeout = timeout;
    emit videoTimeoutChanged(timeout);
}

VideoSubscriberItem::VideoSubscriberItem(QQuickItem *parent) :
    QQuickItem(parent),
    m_surface(nullptr),
    m_field_type(QVideoFrame::FieldType::ProgressiveFrame),
    m_video_timeout(true)
{
    connect(this, SIGNAL(newFrame(QVideoFrame)), this, SLOT(onNewVideoContentReceived(QVideoFrame)), Qt::ConnectionType::QueuedConnection);
    connect(&m_check_timeout_timer, SIGNAL(timeout()), this, SLOT(checkTimeout()));
}

VideoSubscriberItem::~VideoSubscriberItem()
{
    disconnect(&m_check_timeout_timer, SIGNAL(timeout()), this, SLOT(checkTimeout()));
    disconnect(this, SIGNAL(newFrame(QVideoFrame)), this, SLOT(onNewVideoContentReceived(QVideoFrame)));
}

QAbstractVideoSurface *VideoSubscriberItem::videoSurface() const
{
    return m_surface;
}

void VideoSubscriberItem::setVideoSurface(QAbstractVideoSurface *surface)
{
    if (m_surface != surface && m_surface && m_surface->isActive()) {
        m_surface->stop();
    }

    m_surface = surface;
}

QString VideoSubscriberItem::videoName() const
{
    return m_video_name;
}

void VideoSubscriberItem::setVideoName(QString video_name)
{
    if (m_video_name.isEmpty() == false && m_video_name == video_name)
        return;

    m_check_timeout_timer.stop();
    setVideoTimeout(true);

    m_source.release();
    m_callback.release();

    m_video_name = video_name;
    if (m_video_name.isEmpty() == false)
    {
        CreateSource(m_video_name);
        m_check_timeout_timer.start(VIDEO_TIMEOUT_MS);
    }
}

bool VideoSubscriberItem::videoTimeout() const
{
    return m_video_timeout;
}

void VideoSubscriberItem::onNewVideoContentReceived(QVideoFrame qframe)
{
    if(m_surface == nullptr)
        return;

    if (m_surface->isActive() == false)
    {
        m_format = QVideoSurfaceFormat(QSize(qframe.width(), qframe.height()), qframe.pixelFormat());

        if (m_surface->start(m_format) == false)
        {
            // That usually means that the pixel format is not supported!
            // Known unsupported formats are: RGB, BGR
            // TODO: Log...
            return;
        }
    }

    m_last_frame_timestamp = std::chrono::high_resolution_clock::now();
    setVideoTimeout(false);
    m_surface->present(qframe);
}

void VideoSubscriberItem::checkTimeout()
{
    if (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - m_last_frame_timestamp).count() < VIDEO_TIMEOUT_MS)
        return;

    setVideoTimeout(true);
}
