#pragma once
#include <QObject>
#include <QQuickWindow>
#include <QMetaType>
#include <Helpers/QQmlAutoPropertyHelpers.h>
#include <opencv2/videoio.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>

#include <video/sinks/gstreamer_mfx_h264enc.h>


namespace AtrViewer
{
    class QScreenRecord : public QObject
    {
        Q_OBJECT
    public:
        QScreenRecord(QObject *parent = 0);

        Q_INVOKABLE bool record(QQuickItem* item, int w, int h);
        Q_INVOKABLE void endRecord();

    private:

             cv::VideoWriter m_writer; // opencv writing class video
             utils::ref_count_ptr<core::video::video_sink_interface> m_sink;

             bool m_isRecording;
             utils::dispatcher m_worker_dispo;

    };
}
