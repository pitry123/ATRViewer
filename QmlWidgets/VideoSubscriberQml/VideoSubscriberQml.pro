CONFIG -= debug_and_release debug_and_release_target

TEMPLATE = lib
TARGET = VideoSubscriberQml
QT += qml quick multimedia widgets
CONFIG += plugin c++11
CONFIG -= debug_and_release debug_and_release_target

DEFINES += VIDEO_SUBSCRIBER_PLUGIN_LIBRARY

QMAKE_CXXFLAGS_RELEASE += /Zi
QMAKE_LFLAGS_RELEASE += /DEBUG

USE_CONAN = true

TARGET = elbit/qml/video/VideoSubscriberQml
uri = elbit.qml.video

win32 {
    QMAKE_CXXFLAGS_WARN_ON -= -w34100
}

ARCH = ""
win32 {
    !contains(QT_ARCH, x86_64) {
        ARCH += "x86"

    } else {
        ARCH += "x86_64"
    }
}

BUILD_TYPE = ""
CONFIG(debug, debug|release) {
    BUILD_TYPE += "Debug"
} else {
    BUILD_TYPE += "Release"
}

CONAN_FILE = "conanfile.txt"
ADDITIONAL_SETTINGS = ""
win32{
    MSVC_VERSION = $$(VisualStudioVersion)
    MSVC_VERSIONS = $$split(MSVC_VERSION, ".")
    MSVC_MAJOR = $$member(MSVC_VERSIONS, 0)
    ADDITIONAL_SETTINGS += "-s compiler.version=$$first(MSVC_MAJOR)"
}

win32
{
    $$USE_CONAN {
        system(conan install $$CONAN_FILE -s arch=$$ARCH -s build_type=$$BUILD_TYPE $$ADDITIONAL_SETTINGS)
        include(conanbuildinfo.pri)

        INCLUDEPATH += $$CONAN_INCLUDEPATH_INFRASTRUCTURE
        DEPENDPATH += $$CONAN_INCLUDE_PATH_INFRASTRUCTURE
        LIBS += $$CONAN_LIBDIRS_INFRASTRUCTURE -lshared_memory_video
    } else {
        INFRASTRUCTURE_ROOT = "C:/Development/Infrastructure/1.0.14/Framework2.development/unstable/package/9955e0f922b6b24185616f817d4f8f885235aefd"
        LIBDIRS_INFRASTRUCTURE= -L"C:/Development/Infrastructure/1.0.14/Framework2.development/unstable/package/9955e0f922b6b24185616f817d4f8f885235aefd/lib"

        INCLUDEPATH += $$INFRASTRUCTURE_ROOT/Include
        DEPENDPATH += $$INFRASTRUCTURE_ROOT/Include
        LIBS += $$LIBDIRS_INFRASTRUCTURE -lshared_memory_video
    }
}

unix
{
    INCLUDEPATH += "/usr/include/ezFramework_2.0/Infrastructure/Include/"
    DEPENDPATH += "/usr/include/ezFramework_2.0/Infrastructure/Include/"
    LIBS += -L"/usr/lib/" -lshared_memory_video -limage_converter
}

copydata.commands = $(COPY_FILE) \"$$shell_path($$PWD/qmldir)\" \"$$shell_path($$OUT_PWD/elbit/qml/video/)\"
first.depends = $(first) copydata
export(first.depends)
export(copydata.commands)
QMAKE_EXTRA_TARGETS += first copydata


# Input
SOURCES += \
    VideoSubscriberItem.cpp \
    VideoSubscriberQmlPlugin.cpp

HEADERS += \
    OsDefinitions.h \
    VideoSubscriberItem.h \
    VideoSubscriberQmlPlugin.h

DISTFILES = qmldir

!equals(_PRO_FILE_PWD_, $$OUT_PWD) {
    copy_qmldir.target = $$OUT_PWD/qmldir
    copy_qmldir.depends = $$_PRO_FILE_PWD_/qmldir
    copy_qmldir.commands = $(COPY_FILE) \"$$replace(copy_qmldir.depends, /, $$QMAKE_DIR_SEP)\" \"$$replace(copy_qmldir.target, /, $$QMAKE_DIR_SEP)\"
    QMAKE_EXTRA_TARGETS += copy_qmldir
    PRE_TARGETDEPS += $$copy_qmldir.target
}

qmldir.files = qmldir
unix {
    installPath = $$[QT_INSTALL_QML]/$$replace(uri, \\., /)
    qmldir.path = $$installPath
    target.path = $$installPath
    INSTALLS += target qmldir
}
