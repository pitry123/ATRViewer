#pragma once

#include "OsDefinitions.h"
#include <QQmlExtensionPlugin>

class VIDEO_SUBSCRIBER_PLUGIN_EXPORT VideoSubscriberQmlPlugin : public QQmlExtensionPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID QQmlExtensionInterface_iid)

public:
    void registerTypes(const char *uri);
};
