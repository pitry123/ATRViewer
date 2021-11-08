#include "VideoSubscriberQmlPlugin.h"
#include "VideoSubscriberItem.h"

#include <qqml.h>

void VideoSubscriberQmlPlugin::registerTypes(const char* uri)
{
    Q_ASSERT(uri == QLatin1String("elbit.qml.video"));
    qRegisterMetaType<QVideoFrame>();
    qmlRegisterType<VideoSubscriberItem>(uri, 1, 0, "VideoSubscriberItem");
}

