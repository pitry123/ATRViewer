#ifdef USE_NGVA

#include "QConfiguration.h"
#include <QQmlEngine>

AtrViewer::QGlobalConfiguration::QGlobalConfiguration(const Database::DataSet &dataset,
                                                   const Utils::Context &context,
                                                   QObject *parent) :
    QDatabaseConnector(context, parent),
    m_dataset(dataset)
{}

AtrViewer::QGlobalConfiguration::QGlobalConfiguration(QObject *parent) :
    QGlobalConfiguration(nullptr, nullptr, parent)
{
}

AtrViewer::QGlobalConfiguration::~QGlobalConfiguration()
{   
}

void AtrViewer::QGlobalConfiguration::Init()
{
    const ParamStore::UIParams& UIParams = m_dataset[ProjectCommon::MonitorDBIndex::XML_StoreDB]
            [ParamStore::XML_StoreDBEnum::UI_PARAMS].Read<ParamStore::UIParams>();

    this->set_ScreenTitle(UIParams.Title);
    this->set_fullScreen(UIParams.FullScreen);
    if (UIParams.FullScreen == false)
    {
        this->set_ScreenOffset(QList<int>({UIParams.ScreenOffsetX, UIParams.ScreenOffsetY}));
    }
    this->set_ScreenWidth(UIParams.ScreenWidth);
    this->set_ScreenHeight(UIParams.ScreenHeight);
}

void AtrViewer::QGlobalConfiguration::Start()
{}

void AtrViewer::QGlobalConfiguration::Stop()
{
    m_subscriptions.Clear();
}
#endif
