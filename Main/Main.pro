TEMPLATE = app
QT += qml gui quick multimedia
CONFIG += c++11
QTPLUGIN += qsvg qsvgicon
# CONFIG += console
CONFIG -= debug_and_release debug_and_release_target
QT += quickcontrols2


QMAKE_CXXFLAGS_RELEASE += /Zi
QMAKE_LFLAGS_RELEASE += /DEBUG

USE_CONAN = true
USE_NGVA = true

$$USE_NGVA{
    DEFINES += USE_NGVA
}



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

    QMAKE_CXXFLAGS += /bigobj
}

BUILD_TYPE = ""

CONFIG(debug, debug|release){
    BUILD_TYPE += "Debug"
}else{
    BUILD_TYPE += "Release"
}

DEFINES += APP_VERSION=\"\\\"$$APP_VERSION\\\"\"

win32 {
# copying DE files into out_pwd for using binary parser loading scheme functionallity
DE_SOURCE_DIR = \"$$shell_path($$PWD/DE)\"
DE_OUTPUT_DIR = \"$$shell_path($$OUT_PWD/$$BUILD_TYPE/DE)\"
make_DE_directory.commands = $(CHK_DIR_EXISTS) $${DE_OUTPUT_DIR} $(MKDIR) $${DE_OUTPUT_DIR}
copy_DE_data.commands = $(COPY_DIR) $${DE_SOURCE_DIR} $${DE_OUTPUT_DIR}
first.depends = $(first) make_DE_directory copy_DE_data
export(first.depends)
export(make_DE_directory.commands)
export(copy_DE_data.commands)
QMAKE_EXTRA_TARGETS += first make_DE_directory copy_DE_data
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
       #system(conan install $$CONAN_FILE -s arch=$$ARCH -s build_type=Debug $$ADDITIONAL_SETTINGS)

        message(conan install $$CONAN_FILE -s arch=$$ARCH -s build_type=$$BUILD_TYPE  $$ADDITIONAL_SETTINGS)
        include(conanbuildinfo.pri)

#        message("^^^^00-0-0--- "+ $$CONAN_INFRASTRUCTURE_ROOT)

        INCLUDEPATH += $$CONAN_INFRASTRUCTURE_ROOT/Include
        INCLUDEPATH += $$CONAN_INFRASTRUCTURE_ROOT/ModernAPI
        INCLUDEPATH += $$CONAN_INFRASTRUCTURE_ROOT/Common

        INCLUDEPATH += $$CONAN_DDSADDON_ROOT/Include
        INCLUDEPATH += $$CONAN_DDSADDON_ROOT/ModernAPI
        INCLUDEPATH += $$CONAN_DDSADDON_ROOT/Common

        INCLUDEPATH += $$CONAN_OPENCV_ROOT/Include

        $$USE_NGVA{
            INCLUDEPATH += $$CONAN_NGVA_ROOT/Include
            LIBS += $$CONAN_LIBDIRS_NGVA  -lngva2db
            #INCLUDEPATH += "J:/LandSystems/Development/Framework_2.0/NGVA/Include"
            #LIBS += -L"J:/LandSystems/Development/Framework_2.0/NGVA/VSProject/x64-Release/bin/Release"  -lngva2db
#            message("^^^^^^^^^^^^^^^^^^^^^^^^^^^^" + $$CONAN_LIBDIRS_NGVA)

        }
        else{
            INCLUDEPATH += $$CONAN_INCLUDEPATH_ISGVA
            LIBS += $$CONAN_LIBDIRS_ISGVA  -lbarakgenerateddds2db
            message("^^^^^^^^^^^^^^^^^^^^^^^^^^^^" + $$CONAN_GENERATEDDDS_ROOT)
        }
        SENSORS_LIBS = \
         -lcommon_data_objects \
         -lvideo_sensor \
         -lvideo_data_objects \
         -lcompression_stream \
         -lcapture_sensors

        INCLUDEPATH += $$CONAN_SENSORS_ROOT/Include
        INCLUDEPATH += $$CONAN_SENSORS_ROOT/Common
        INCLUDEPATH += $$CONAN_SENSORS_ROOT/ModernAPI
        LIBS += $$CONAN_LIBDIRS_SENSORS $$SENSORS_LIBS


        LIBS += $$CONAN_LIBDIRS_INFRASTRUCTURE -lezframework -lmemory_database -lbinary_parser -lports -lprotocols -lcommon_files -lgstreamer_video -lshared_memory_video -lmemory_stream -llz4_compression -lfile_stream  -limage_converter
        LIBS += $$CONAN_LIBDIRS_DDSADDON -ldds_database
        LIBS += $$CONAN_LIBDIRS_OPENCV -lopencv_world345

    } else {
        INFRASTRUCTURE_ROOT = "C:/Development/Infrastructure/1.0.14/Framework2.development/unstable/package/9955e0f922b6b24185616f817d4f8f885235aefd"
        LIBDIRS_INFRASTRUCTURE= -L"C:/Development/Infrastructure/1.0.14/Framework2.development/unstable/package/9955e0f922b6b24185616f817d4f8f885235aefd/lib"

        DDSADDON_ROOT = "C:/Development/DDSAddon/1.0.14/Framework2.development/unstable/package/9955e0f922b6b24185616f817d4f8f885235aefd"
        LIBDIRS_DDSADDON = -L"C:/Development/DDSAddon/1.0.14/Framework2.development/unstable/package/9955e0f922b6b24185616f817d4f8f885235aefd/lib"

        INCLUDEPATH += $$INFRASTRUCTURE_ROOT/Include
        INCLUDEPATH += $$INFRASTRUCTURE_ROOT/ModernAPI
        INCLUDEPATH += $$INFRASTRUCTURE_ROOT/Common

        INCLUDEPATH += $$DDSADDON_ROOT/Include
        INCLUDEPATH += $$DDSADDON_ROOT/ModernAPI
        INCLUDEPATH += $$DDSADDON_ROOT/Common

        LIBS += $$LIBDIRS_INFRASTRUCTURE -lezframework -lmemory_database -lbinary_parser -lports -lprotocols -lcommon_files
        LIBS += $$LIBDIRS_DDSADDON -ldds_database
    }

}
unix
{
    INCLUDEPATH += "/usr/include/ezFramework_2.0/Infrastructure/Include"
    INCLUDEPATH += "/usr/include/ezFramework_2.0/Infrastructure/ModernAPI"
    INCLUDEPATH += "/usr/include/ezFramework_2.0/Infrastructure/Common"

    INCLUDEPATH += "/usr/include/ezFramework_2.0/Addons/DDSAddon/Include"
    INCLUDEPATH += "/usr/include/ezFramework_2.0/Addons/DDSAddon/ModernAPI"
    INCLUDEPATH += "/usr/include/ezFramework_2.0/Addons/DDSAddon/Common"

    LIBS += -L"/usr/lib/" -lezframework -lmemory_database -lbinary_parser -lports -lprotocols -lcommon_files
    LIBS += -L"/usr/lib/" -ldds_database
}


RESOURCES += \
    qml.qrc

SOURCES += \
    Helpers/VideoStreamAllocator.cpp \
    ViewModel/QAlgoSettings.cpp \
    ViewModel/QDatabaseConnector.cpp \
    ViewModel/QExportData.cpp \
    ViewModel/QGuiConnector.cpp \
    ViewModel/QGuiTypes.cpp \
    ViewModel/QConfiguration.cpp \
    ViewModel/QProcessRecord.cpp \
    ViewModel/QScreenRecord.cpp \
    ViewModel/QTracking.cpp \
    ViewModel/QVideo.cpp \
    main.cpp


HEADERS += \
    ProjectCommon.h \
    ParamStore.h \
    BarakIdentifiersStore.h \
    Helpers/Constants.h \
    Helpers/QQmlReloader.h \
    Helpers/QQmlHelpersCommon.h \
    Helpers/QQmlAutoPropertyHelpers.h \
    Helpers/Utils.h \
    Helpers/VideoStreamAllocator.h \
    ViewModel/QAlgoSettings.h \
    ViewModel/QDatabaseConnector.h \
    ViewModel/QExportData.h \
    ViewModel/QGuiConnector.h \
    ViewModel/QGuiTypes.h \
    ViewModel/QConfiguration.h \
    ViewModel/QProcessRecord.h \
    ViewModel/QScreenRecord.h \
    ViewModel/QTracking.h \
    ViewModel/QVideo.h
