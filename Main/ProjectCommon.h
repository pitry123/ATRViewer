#pragma once

class ProjectCommon
{
public:
	enum MonitorDBIndex
	{
        VideoExtension_Incoming,                    //r:P_Video_Extension_PSM_Rows g:Video
        VideoExtension_Outgoing,                    //r:P_Video_Extension_PSM_Rows g:Video
        VideoExtension_States,                      //r:P_Video_Extension_PSM_Topics g:Video

        P_Tactical_Sensor_Extension_PSM_Incoming, //r:P_Tactical_Sensor_Extension_PSM_Rows g:TacticalSensor
        P_Tactical_Sensor_Extension_PSM_Outgoing, //r:P_Tactical_Sensor_Extension_PSM_Rows g:TacticalSensor
        P_Tactical_Sensor_Extension_PSM_States, //r:P_Tactical_Sensor_Extension_PSM_Topics g:TacticalSensor

        Tactical_Sensor_ATR_Config_Extension_PSM_Incoming, //r:P_Tactical_Sensor_Extension_PSM_Rows g:TacticalSensor
        Tactical_Sensor_ATR_Config_Extension_PSM_Outgoing, //r:P_Tactical_Sensor_Extension_PSM_Rows g:TacticalSensor
        Tactical_Sensor_ATR_Config_Extension_PSM_States, //r:P_Tactical_Sensor_Extension_PSM_Topics g:TacticalSensor

        ResourceSpecification_Incoming, //r:P_Vehicle_Configuration_PSM_Rows g:resourceSpec
        ResourceSpecification_Outgoing, //r:P_Vehicle_Configuration_PSM_Rows g:resourceSpec
        ResourceSpecification_States, //r:P_Vehicle_Configuration_PSM_Topics g:resourceSpec

        VideoTracking_Incoming, //r:P_VideoTracking_Extension_PSM_Rows g:VideoTrackingEx
        VideoTracking_Outgoing, //r:P_VideoTracking_Extension_PSM_Rows g:VideoTrackingEx
        VideoTracking_States, //r:P_VideoTracking_Extension_PSM_Topics g:VideoTrackingEx

        // Memory Database
        XML_StoreDB,                                //r:XML_StoreDBEnum g:ParamStore

        NumberOfDatabases
    };

    enum ProjectLogID
    {
        NUM_OF_PROJECT_LOGS
    };
};
