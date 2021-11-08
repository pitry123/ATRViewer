#include "ParamStore.h"
#include "Helpers/Constants.h"
#include "Helpers/QQmlReloader.h"
#include "ViewModel/QGuiConnector.h"

#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QPointer>
#include <QProcess>
#include <QTranslator>
#include <QFileInfo>

#include <Factories.hpp>
#include <Monitor.hpp>
#include <MonitorClient.hpp>
#include <XMLLoader.hpp>

#include <DDS/Factories.hpp>


#ifdef USE_NGVA
#include "ProjectCommon.h"
#include <Video_Extension_PSM_Gateway.hpp>
#include <Tactical_Sensor_Extension_PSM_Gateway.hpp>
#include <Tactical_Sensor_ATR_Config_Extension_PSM_Gateway.hpp>
#include <Vehicle_Configuration_PSM_Gateway.hpp>
#include <VideoTracking_Extension_PSM_Gateway.hpp>
#else
#include "BarakProjectCommon.h"
#include "BarakIdentifiersStore.h"
#include <Video_PSM_Gateway.hpp>
#include <Tactical_Sensor_ATR_Config_Extension_PSM_Gateway.hpp>
#include <Tactical_Sensor_PSM_Gateway.hpp>
#include <Maintenance_PSM_Gateway.hpp>
#endif


std::function<void()> QQmlReload::m_reloadFunc;
static Logging::Logger APP_LOGGER = Core::Framework::CreateLogger("ATRViewer", Logging::Severity::TRACE);


class AtrViewerBuilder : public Application::Builder
{
private:

    static Logging::Logger LOGGER;
    Database::DataSet m_dataset;

    Files::FilesHandler m_handler;
    Common::CommonTypes::MonitorParams m_debugEnvParams;
    ParamStore::DDSParams m_ddsParams;

    QPointer<AtrViewer::QGuiConnector> m_connector;
    QString m_directoryPath;

#ifndef USE_NGVA
    Files::FilesHandler m_handlerIdnt;
#endif

protected:
    virtual void BuildEnvironment() override
    {



    }


    virtual void BuildDispatchers() override
    {
    }

    void OnInitialized() override
    {
        m_connector->Init();

#ifdef USE_NGVA

        m_dataset[ProjectCommon::MonitorDBIndex::ResourceSpecification_States][NGVA::P_Vehicle_Configuration_PSM_Topics::C_Resource_Specification_Writer].
                Write(NGVA::DDSWriterState{ NGVA::DDSWriterMode::PERSISTENT, {0, 0} });
#else
        m_dataset[BarakProjectCommon::MonitorDBIndex::Resource_Specification_States][Barak::P_Maintenance_PSM_Topics::C_Resource_Specification_Writer].
                Write(Barak::DDSWriterState{ Barak::DDSWriterMode::PERSISTENT, {0, 0} });
#endif
    }

    void OnStarted() override
    {
        m_connector->Start();


#ifdef USE_NGVA
        NGVA::P_Vehicle_Configuration_PSM::C_Resource_Specification spec = {};
        spec.A_sourceID.A_resourceId = 12345;
                m_dataset[ProjectCommon::MonitorDBIndex::ResourceSpecification_Outgoing]
                        [NGVA::P_Vehicle_Configuration_PSM_Rows::C_Resource_Specification_Row].
                        Write(spec);
#else
        Barak::P_Maintenance_PSM::C_Resource_Specification spec = {};
        spec.A_sourceID.A_moduleId = 123;
                m_dataset[BarakProjectCommon::MonitorDBIndex::Resource_Specification_Outgoing]
                        [Barak::P_Maintenance_PSM_Rows::C_Resource_Specification_Row].
                        Write(spec);
#endif
    }

    void ReadParamsStore()
    {
        std::string  xmlFactorySetting = "./Configuration/AtrViewer.xml";
        std::string  xmlDeveloperSetting = "./Configuration/AtrViewer.xml";
        std::string  xmlUserSetting = "./Configuration/AtrViewer.xml";
        m_handler = Files::FilesHandler::Create();

#ifdef USE_NGVA

        AddRunnable<ConfigurationLoader::XmlStoreDB>(ProjectCommon::ProjectCommon::MonitorDBIndex::XML_StoreDB,
                                                     m_dataset,
                                                     xmlFactorySetting,
                                                     xmlFactorySetting,
                                                     xmlFactorySetting,
                                                     m_handler,
                                                     "AtrViewer",
                                                     "");



        auto ntp = m_dataset[ProjectCommon::MonitorDBIndex::XML_StoreDB][ParamStore::XML_StoreDBEnum::ACTIVE_NTP].Read<bool>();
        m_dataset[ProjectCommon::MonitorDBIndex::XML_StoreDB][ParamStore::XML_StoreDBEnum::DEBUG_ENV_PARAMS].Read<Common::CommonTypes::MonitorParams>(m_debugEnvParams);
        m_dataset[ProjectCommon::MonitorDBIndex::XML_StoreDB][ParamStore::XML_StoreDBEnum::DDS_PARAMS].Read<ParamStore::DDSParams>(m_ddsParams);



#else

        std::cout << "wait for \n";
        m_handlerIdnt = Files::FilesHandler::Create();
        std::string  xmlIdentifierSetting = "./Configuration/IdentifiersStore.xml";

        AddRunnable<ConfigurationLoader::XmlStoreDB>(BarakProjectCommon::MonitorDBIndex::IdentifierXml,
                                                     m_dataset,
                                                     xmlIdentifierSetting,
                                                     xmlIdentifierSetting,
                                                     xmlIdentifierSetting,
                                                     m_handlerIdnt,
                                                     "AtrViewer",
                                                     "");

        AddRunnable<ConfigurationLoader::XmlStoreDB>(BarakProjectCommon::MonitorDBIndex::XML_StoreDB,
                                                     m_dataset,
                                                     xmlFactorySetting,
                                                     xmlFactorySetting,
                                                     xmlFactorySetting,
                                                     m_handler,
                                                     "AtrViewer",
                                                     "");

        m_dataset[BarakProjectCommon::MonitorDBIndex::XML_StoreDB][ParamStore::XML_StoreDBEnum::DEBUG_ENV_PARAMS].Read<Common::CommonTypes::MonitorParams>(m_debugEnvParams);
        m_dataset[BarakProjectCommon::MonitorDBIndex::XML_StoreDB][ParamStore::XML_StoreDBEnum::DDS_PARAMS].Read<ParamStore::DDSParams>(m_ddsParams);

#endif



    }

    void InitDDSDataset(QString directory_path)
    {
        // init DDS dataset
        Common::CommonTypes::CHARBUFFER strDds = {};


#ifdef USE_NGVA

        AddRunnable(NGVA::P_Video_Extension_PSM_Gateway::Create(
                        m_dataset[ProjectCommon::MonitorDBIndex::VideoExtension_States],
                        m_dataset[ProjectCommon::MonitorDBIndex::VideoExtension_Incoming],
                        m_dataset[ProjectCommon::MonitorDBIndex::VideoExtension_Outgoing],
                        0,"./ELBIT_VG_QOS_PROFILES.xml")); //"./ELBIT_CARMEL_QOS_PROFILES.xml"));

        AddRunnable(NGVA::P_Tactical_Sensor_Extension_PSM_Gateway::Create(
                        m_dataset[ProjectCommon::MonitorDBIndex::P_Tactical_Sensor_Extension_PSM_States],
                        m_dataset[ProjectCommon::MonitorDBIndex::P_Tactical_Sensor_Extension_PSM_Incoming],
                        m_dataset[ProjectCommon::MonitorDBIndex::P_Tactical_Sensor_Extension_PSM_Outgoing],
                        0,"./ELBIT_VG_QOS_PROFILES.xml")); //"./ELBIT_CARMEL_QOS_PROFILES.xml"));

        AddRunnable(NGVA::P_Tactical_Sensor_ATR_Config_Extension_PSM_Gateway::Create(
                        m_dataset[ProjectCommon::MonitorDBIndex::Tactical_Sensor_ATR_Config_Extension_PSM_States],
                        m_dataset[ProjectCommon::MonitorDBIndex::Tactical_Sensor_ATR_Config_Extension_PSM_Incoming],
                        m_dataset[ProjectCommon::MonitorDBIndex::Tactical_Sensor_ATR_Config_Extension_PSM_Outgoing],
                        0,"./ELBIT_VG_QOS_PROFILES.xml")); //"./ELBIT_CARMEL_QOS_PROFILES.xml"));


        AddRunnable(NGVA::P_Vehicle_Configuration_PSM_Gateway::Create(
                        m_dataset[ProjectCommon::MonitorDBIndex::ResourceSpecification_States],
                        m_dataset[ProjectCommon::MonitorDBIndex::ResourceSpecification_Incoming],
                        m_dataset[ProjectCommon::MonitorDBIndex::ResourceSpecification_Outgoing],
                        0,"./ELBIT_VG_QOS_PROFILES.xml")); //"./ELBIT_CARMEL_QOS_PROFILES.xml"));

        AddRunnable(NGVA::P_VideoTracking_Extension_PSM_Gateway::Create(
                        m_dataset[ProjectCommon::MonitorDBIndex::VideoTracking_States],
                        m_dataset[ProjectCommon::MonitorDBIndex::VideoTracking_Incoming],
                        m_dataset[ProjectCommon::MonitorDBIndex::VideoTracking_Outgoing],
                        0,"./ELBIT_VG_QOS_PROFILES.xml")); //"./ELBIT_CARMEL_QOS_PROFILES.xml"));

#else

        AddRunnable(Barak::P_Video_PSM_Gateway::Create(
                    m_dataset[BarakProjectCommon::MonitorDBIndex::Video_States],
                m_dataset[BarakProjectCommon::MonitorDBIndex::Video_Incoming],
                m_dataset[BarakProjectCommon::MonitorDBIndex::Video_Outgoing],
                 0,"./ELBIT_VG_QOS_PROFILES.xml"));

        AddRunnable(Barak::P_Tactical_Sensor_PSM_Gateway::Create(
                    m_dataset[BarakProjectCommon::MonitorDBIndex::Tactical_Sensor_States],
                m_dataset[BarakProjectCommon::MonitorDBIndex::Tactical_Sensor_Incoming],
                m_dataset[BarakProjectCommon::MonitorDBIndex::Tactical_Sensor_Outgoing],
                 0,"./ELBIT_VG_QOS_PROFILES.xml"));

        AddRunnable(Barak::P_Tactical_Sensor_ATR_Config_Extension_PSM_Gateway::Create(
                        m_dataset[BarakProjectCommon::MonitorDBIndex::Tactical_Config_States],
            m_dataset[BarakProjectCommon::MonitorDBIndex::Tactical_Config_Incoming],
            m_dataset[BarakProjectCommon::MonitorDBIndex::Tactical_Config_Outgoing],
           0,"./ELBIT_VG_QOS_PROFILES.xml")); //"./ELBIT_CARMEL_QOS_PROFILES.xml"));


        AddRunnable(Barak::P_Maintenance_PSM_Gateway::Create(
                        m_dataset[BarakProjectCommon::MonitorDBIndex::Resource_Specification_States],
            m_dataset[BarakProjectCommon::MonitorDBIndex::Resource_Specification_Incoming],
            m_dataset[BarakProjectCommon::MonitorDBIndex::Resource_Specification_Outgoing],
           0,"./ELBIT_VG_QOS_PROFILES.xml")); //"./ELBIT_CARMEL_QOS_PROFILES.xml"));


#endif
    }

    void InitTrackingDatasets()
    {
        AddRunnable<Database::Monitor>("127.0.0.1", m_debugEnvParams.localPort, "127.0.0.1", m_debugEnvParams.remotePort, m_dataset);
    }

public:
    AtrViewerBuilder(QString directory_path) :
        m_dataset(Database::MemoryDatabase::Create("ATR")),
        m_directoryPath(directory_path)
    {
#ifdef USE_NGVA
        m_dataset.AddTable(ProjectCommon::MonitorDBIndex::VideoExtension_Incoming);
        m_dataset.AddTable(ProjectCommon::MonitorDBIndex::VideoExtension_Outgoing);
        m_dataset.AddTable(ProjectCommon::MonitorDBIndex::VideoExtension_States);

        m_dataset.AddTable(ProjectCommon::MonitorDBIndex::P_Tactical_Sensor_Extension_PSM_Incoming);
        m_dataset.AddTable(ProjectCommon::MonitorDBIndex::P_Tactical_Sensor_Extension_PSM_Outgoing);
        m_dataset.AddTable(ProjectCommon::MonitorDBIndex::P_Tactical_Sensor_Extension_PSM_States);

        m_dataset.AddTable(ProjectCommon::MonitorDBIndex::Tactical_Sensor_ATR_Config_Extension_PSM_Incoming);
        m_dataset.AddTable(ProjectCommon::MonitorDBIndex::Tactical_Sensor_ATR_Config_Extension_PSM_Outgoing);
        m_dataset.AddTable(ProjectCommon::MonitorDBIndex::Tactical_Sensor_ATR_Config_Extension_PSM_States);

        m_dataset.AddTable(ProjectCommon::MonitorDBIndex::ResourceSpecification_Incoming);
        m_dataset.AddTable(ProjectCommon::MonitorDBIndex::ResourceSpecification_Outgoing);
        m_dataset.AddTable(ProjectCommon::MonitorDBIndex::ResourceSpecification_States);

        m_dataset.AddTable(ProjectCommon::MonitorDBIndex::VideoTracking_Incoming);
        m_dataset.AddTable(ProjectCommon::MonitorDBIndex::VideoTracking_Outgoing);
        m_dataset.AddTable(ProjectCommon::MonitorDBIndex::VideoTracking_States);




        m_dataset.AddTable(ProjectCommon::MonitorDBIndex::XML_StoreDB);

#else
        m_dataset.AddTable(BarakProjectCommon::MonitorDBIndex::Video_Incoming);
        m_dataset.AddTable(BarakProjectCommon::MonitorDBIndex::Video_Outgoing);
        m_dataset.AddTable(BarakProjectCommon::MonitorDBIndex::Video_States);

        m_dataset.AddTable(BarakProjectCommon::MonitorDBIndex::Tactical_Sensor_Incoming);
        m_dataset.AddTable(BarakProjectCommon::MonitorDBIndex::Tactical_Sensor_Outgoing);
        m_dataset.AddTable(BarakProjectCommon::MonitorDBIndex::Tactical_Sensor_States);

        m_dataset.AddTable(BarakProjectCommon::MonitorDBIndex::Tactical_Config_Incoming);
        m_dataset.AddTable(BarakProjectCommon::MonitorDBIndex::Tactical_Config_Outgoing);
        m_dataset.AddTable(BarakProjectCommon::MonitorDBIndex::Tactical_Config_States);

        m_dataset.AddTable(BarakProjectCommon::MonitorDBIndex::Resource_Specification_Incoming);
        m_dataset.AddTable(BarakProjectCommon::MonitorDBIndex::Resource_Specification_Outgoing);
        m_dataset.AddTable(BarakProjectCommon::MonitorDBIndex::Resource_Specification_States);


        m_dataset.AddTable(BarakProjectCommon::MonitorDBIndex::IdentifierXml);
        m_dataset.AddTable(BarakProjectCommon::MonitorDBIndex::XML_StoreDB);
#endif

        try
        {
            ReadParamsStore();
        }
        catch (std::exception e)
        {
            std::string message = "Configuration files didn't found!\n";
            message += e.what();
#ifdef _WIN32
            throw std::exception(message.c_str());
#else
            throw std::exception();
#endif
        }

        InitDDSDataset(directory_path);
        InitTrackingDatasets();

        m_connector = new AtrViewer::QGuiConnector(m_dataset);
    }

    AtrViewer::QGuiConnector* GuiConnector()
    {
        return m_connector.data();
    }
};

Logging::Logger AtrViewerBuilder::LOGGER = Core::Framework::CreateLogger("ATRViewer.Builder", APP_LOGGER.Filter());


int main(int argc, char *argv[])
{    

#ifdef _WIN32
    setvbuf(stdout, nullptr, 4, 0);
#endif

#ifdef TIMER_PROPERTIES
    PropertyUpdateTimer::Instance().Start(1000.0 / TIMER_PROPERTIES_TICKS_PER_SECOND);
#endif

    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QCoreApplication::setAttribute(Qt::AA_DisableShaderDiskCache);

    QGuiApplication appGUI(argc, argv);
    appGUI.setOrganizationName("Elbit");
    appGUI.setOrganizationDomain("www.elbitsystems.com");

    QTranslator translator;
    QString locale = QLocale::system().name();
    if (locale.isNull() == false && locale.isEmpty() == false)
    {
        QString localeFile = locale + ".qm";

        if (translator.load(localeFile, "./locale") == true)
            appGUI.installTranslator(&translator);
    }

    // get running directory
#ifdef _WIN32
    QStringList splited_app_location = QString(argv[0]).split("\\");
    splited_app_location.pop_back();
    QString directory_path = splited_app_location.join("\\");
#else
    QStringList splited_app_location = QString(argv[0]).split("/");
    splited_app_location.pop_back();
    QString directory_path = splited_app_location.join("/");
#endif

   // Sleep(10000);
    Core::Console::ColorPrint(Core::Console::Colors::BLUE,"start builder\n");
    AtrViewerBuilder builder(directory_path);
    builder.Build();
    Core::Console::ColorPrint(Core::Console::Colors::BLUE,"end builder\n");

    qmlRegisterType<QQmlReload>("qmlReload", 1,0, "QmlReload");
    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty(QString("guiConnector"), builder.GuiConnector());

    engine.load("qrc:/qml/main.qml");

    if (engine.rootObjects().isEmpty())
        return -1;

    return appGUI.exec();
}
