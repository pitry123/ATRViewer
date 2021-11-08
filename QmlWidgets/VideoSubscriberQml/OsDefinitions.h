#ifndef OSDEFINITIONS_H
#define OSDEFINITIONS_H

#include <QtCore/QtGlobal>

#if defined(VIDEO_SUBSCRIBER_PLUGIN_LIBRARY)
    #define VIDEO_SUBSCRIBER_PLUGIN_EXPORT Q_DECL_EXPORT
#else
    #define VIDEO_SUBSCRIBER_PLUGIN_EXPORT Q_DECL_IMPORT
#endif

#endif // OSDEFINITIONS_H