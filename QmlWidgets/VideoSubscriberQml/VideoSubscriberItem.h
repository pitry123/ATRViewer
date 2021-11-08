#ifndef VIDEOSUBSCRIBERITEM_H
#define VIDEOSUBSCRIBERITEM_H

#include "OsDefinitions.h"

#include <QQuickItem>
#include <QAbstractVideoSurface>
#include <QAbstractVideoBuffer>
#include <QVideoSurfaceFormat>
#include <QTimer>

#include <video/sources/shared_memory_video_source.h>
#include <utils/ref_count_ptr.hpp>
#include <utils/video.hpp>

#include <chrono>

#define VIDEO_TIMEOUT_MS 2000

class VIDEO_SUBSCRIBER_PLUGIN_EXPORT VideoSourceBuffer : public QAbstractVideoBuffer
{
private:
    utils::ref_count_ptr<core::video::frame_interface> m_frame;
    static inline unsigned int BitsPerPixel(core::imaging::pixel_format format);

public:
    VideoSourceBuffer(core::video::frame_interface* frame);
    virtual ~VideoSourceBuffer() override;

    virtual MapMode mapMode() const override;
    virtual uchar* map(MapMode mode, int *numBytes, int *bytesPerLine) override;
    virtual void unmap() override;
};

class VIDEO_SUBSCRIBER_PLUGIN_EXPORT VideoSubscriberItem : public QQuickItem
{
    Q_OBJECT
    Q_DISABLE_COPY(VideoSubscriberItem)

    Q_PROPERTY(QAbstractVideoSurface* videoSurface READ videoSurface WRITE setVideoSurface)
    Q_PROPERTY(QString videoName READ videoName WRITE setVideoName)
    Q_PROPERTY(bool videoTimeout READ videoTimeout NOTIFY videoTimeoutChanged)

private:
    QAbstractVideoSurface* m_surface;
    QVideoSurfaceFormat m_format;
    QVideoFrame::FieldType m_field_type;
    QString m_video_name;
    bool m_video_timeout;

    utils::ref_count_ptr<core::imaging::image_algorithm_interface> m_image_converter;
    utils::ref_count_ptr<core::video::frame_callback> m_callback;
    utils::ref_count_ptr<core::video::video_source_interface> m_source;

    QTimer m_check_timeout_timer;
    std::chrono::time_point<std::chrono::high_resolution_clock> m_last_frame_timestamp;

    static inline QVideoFrame::PixelFormat ConvertPixelForamt(core::imaging::pixel_format format);
    bool CreateSource(QString video_name);

    void setVideoTimeout(bool timeout);

public:
    VideoSubscriberItem(QQuickItem *parent = nullptr);
    ~VideoSubscriberItem();

    QAbstractVideoSurface* videoSurface() const;
    void setVideoSurface(QAbstractVideoSurface *surface);

    QString videoName() const;
    void setVideoName(QString video_name);

    bool videoTimeout() const;

public slots:
    void onNewVideoContentReceived(QVideoFrame frame);

signals:
    void newFrame(QVideoFrame);
    void videoTimeoutChanged(bool);

private slots:
    void checkTimeout();
};

#endif // VIDEOSUBSCRIBERITEM_H
