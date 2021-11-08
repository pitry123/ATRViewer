#pragma once
#ifdef USE_NGVA
#include <Video_Extension_PSM_Gateway.hpp>
#include "ProjectCommon.h"
#else
#include "BarakProjectCommon.h"
#include "BarakIdentifiersStore.h"
#include <Video_PSM_Gateway.hpp>
#endif
#include "ParamStore.h"
#include "Helpers/QQmlAutoPropertyHelpers.h"
#include "Helpers/Constants.h"
#include "Helpers/Utils.h"
#include "Helpers/VideoStreamAllocator.h"
#include "ViewModel/QDatabaseConnector.h"
#include "ViewModel/QGuiTypes.h"



#include <chrono>
#include <QVector>
#include <mutex>

namespace AtrViewer
{
    class QVideoChannel : public QObject
    {
        Q_OBJECT
        Q_PROPERTY(QString url READ get_url WRITE set_url NOTIFY urlChanged)
        QML_WRITABLE_AUTO_PROPERTY(QString, name)

    private:
        mutable std::mutex m_videoArgsMutex;
        QString m_videoArgs;

        std::chrono::time_point<std::chrono::high_resolution_clock> m_lastUpdateTime;

    signals:
        void urlChanged(const QString&);

    public:
        QVideoChannel(QString name,
                      QString uri,
                      QObject* parent = nullptr);

        QVideoChannel(QString name,
                      QObject* parent = nullptr);
        explicit QVideoChannel(QObject* parent = nullptr);
        QString get_url() const;
        void set_url(const QString& val);
        bool update();
    };

    class QVideoChannels : public QDatabaseConnector
    {
        Q_OBJECT

        QML_WRITABLE_AUTO_PROPERTY(QVideoChannel*, video_channel_intput0)
        QML_WRITABLE_AUTO_PROPERTY(QVideoChannel*, video_channel_intput1)
        QML_WRITABLE_AUTO_PROPERTY(QVideoChannel*, video_channel_intput2)
        QML_WRITABLE_AUTO_PROPERTY(QVideoChannel*, video_channel_intput3)

        QML_WRITABLE_AUTO_PROPERTY(bool, channel_0_active)
        QML_WRITABLE_AUTO_PROPERTY(bool, channel_1_active)
        QML_WRITABLE_AUTO_PROPERTY(bool, channel_2_active)
        QML_WRITABLE_AUTO_PROPERTY(bool, channel_3_active)

        QML_WRITABLE_AUTO_PROPERTY(QVariantList, videoChannelList)


    private:
        Database::DataSet m_dataset;

        std::map<uint32_t, QVideoChannel*> m_video_channels_map;
        VideoStreamAllocator m_video_stream_allocator;


        bool existance_video_channel_by_name(QString& name);
        uint32_t get_video_resource_id_by_name(QString& name);

        static std::map<QString, QString> splitVideoStream(const QString& videoPublisherStream);

#ifndef USE_NGVA
        std::map<uint32_t, std::string> m_id_to_src_name;

#endif

    public:
        QVideoChannels(const Database::DataSet& dataset,
                       const Utils::Context& context = nullptr,
                       QObject* parent = nullptr);
        explicit QVideoChannels(QObject* parent = nullptr);
        virtual void Start() override;
        virtual void Stop() override;



        Q_INVOKABLE int get_video_resource_id(QString name);

        Q_INVOKABLE void changeChannelInuput(int ch, QString appSrcName);

   signals:
        void videoChannelInputChanged(int ch, int resource_video);
        void newVideoDetection(QString video_name, int resource_id);



    };
}
