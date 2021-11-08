#pragma once
#ifdef USE_NGVA
#include "ProjectCommon.h"
#else
#include "BarakProjectCommon.h"
#endif
#include "ParamStore.h"
#include "ViewModel/QGuiTypes.h"
#include "ViewModel/QDatabaseConnector.h"
#include "ViewModel/QConfiguration.h"
#include "ViewModel/QVideo.h"
#include "ViewModel/QTracking.h"
#include "ViewModel/QAlgoSettings.h"
#include "ViewModel/QProcessRecord.h"
#include "ViewModel/QScreenRecord.h"
#include "ViewModel/QExportData.h"
#include <QQmlEngine>


namespace AtrViewer
{
static constexpr int TYPES_VERSION_MAJOR = 1;
static constexpr int TYPES_VERSION_MINOR = 0;

class QGuiConnector : public QDatabaseConnector
{
    Q_OBJECT

    QML_WRITABLE_AUTO_PROPERTY(AtrViewer::QVideoChannels*, videos)
    QML_WRITABLE_AUTO_PROPERTY(AtrViewer::QTracking*, tracking)
    QML_WRITABLE_AUTO_PROPERTY(AtrViewer::QScreenRecord*, screen_rec)
    QML_WRITABLE_AUTO_PROPERTY(AtrViewer::QAlgoSettings*, algo_settings)
    QML_WRITABLE_AUTO_PROPERTY(AtrViewer::QProcessRecord*, process_record)
    QML_WRITABLE_AUTO_PROPERTY(AtrViewer::QExportData*, export_data)



private:
    Database::DataSet m_memoryDataset;
    Database::SubscriptionsCollector m_subscriptions;

private:
    template <typename T>
    inline void RegisterType(const char* name)
    {
        qmlRegisterType<T>("atrViewer", TYPES_VERSION_MAJOR, TYPES_VERSION_MINOR, name);
    }

    void RegisterTypes();

public:
    QGuiConnector(const Database::DataSet& memoryDataset,
                  QObject* parent = nullptr);
    QGuiConnector(QObject* parent = nullptr);
    virtual ~QGuiConnector() override;

    void Init() override;
    void Start() override;
    void Stop() override;
};
}
