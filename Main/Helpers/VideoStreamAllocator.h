#pragma once

#include <QObject>
#include <QPointer>
#include <QProcess>
#include <QThread>
#include <QMap>
#include <QTime>

#include <qdebug.h>
#include <chrono>

namespace AtrViewer
{
    class QVideoChannel;
}

class VideoStreamAllocator : public QThread
{
    Q_OBJECT

signals:
    void openStream(const QString& name, const AtrViewer::QVideoChannel* videoChannel);
    void closeStream(const QString& name);

private slots:
    void onOpenStream(const QString& name, const AtrViewer::QVideoChannel* videoChannel);
    void onCloseStream(const QString& name);

public:
    explicit VideoStreamAllocator(QObject* parent = nullptr);
    virtual ~VideoStreamAllocator();

protected:
    virtual void run() override;
    void timerEvent(QTimerEvent* event) override;

private:
    struct Publisher
    {
        QPointer<QProcess> publisher;
        const AtrViewer::QVideoChannel* videoChannel;
        std::chrono::time_point<std::chrono::high_resolution_clock> time;
    };

    void createPublisher(const QString& name, const AtrViewer::QVideoChannel* videoChannel);

    QMap<QString, Publisher> m_aPublishers;
    QMap<QString, Publisher> m_aTemporaryPublishers;
};
