#pragma once
#ifdef USE_NGVA
#include "Helpers/QQmlAutoPropertyHelpers.h"
#include "ViewModel/QDatabaseConnector.h"
#include "ViewModel/QGuiTypes.h"

#include "ProjectCommon.h"

#include "ParamStore.h"
#include "QMap"

namespace AtrViewer
{
class QGlobalConfiguration : public QDatabaseConnector
{
    Q_OBJECT

public:

    QML_WRITABLE_AUTO_PROPERTY(bool, fullScreen)
    QML_WRITABLE_AUTO_PROPERTY(QString, ScreenTitle)
    QML_WRITABLE_AUTO_PROPERTY(QList<int>, ScreenOffset)
    QML_WRITABLE_AUTO_PROPERTY(int, ScreenHeight)
    QML_WRITABLE_AUTO_PROPERTY(int, ScreenWidth)

private:
    Database::DataSet m_dataset;
    Database::SubscriptionsCollector m_subscriptions;

public:
    QGlobalConfiguration(const Database::DataSet& dataset, const Utils::Context& context = nullptr, QObject* parent = nullptr);
    explicit QGlobalConfiguration(QObject* parent = nullptr);
    virtual ~QGlobalConfiguration() override;
    virtual void Init() override;
    virtual void Start() override;
    virtual void Stop() override;
};
}
#endif
