#pragma once

class BarakProjectCommon
{
public:
	enum MonitorDBIndex
	{
        Video_Incoming,                    //r:P_Video_PSM_Rows g:Video
        Video_Outgoing,                    //r:P_Video_PSM_Rows g:Video
        Video_States,                      //r:P_Video_PSM_Topics g:Video

        Tactical_Sensor_Incoming,           //r:P_Tactical_Sensor_PSM_Rows g:TacticalSensor
        Tactical_Sensor_Outgoing,           //r:P_Tactical_Sensor_PSM_Rows g:TacticalSensor
        Tactical_Sensor_States,             //r:P_Tactical_Sensor_PSM_Topics g:TacticalSensor

        Tactical_Config_Incoming,           //r:P_Tactical_Sensor_PSM_Rows g:TacticalSensor
        Tactical_Config_Outgoing,           //r:P_Tactical_Sensor_PSM_Rows g:TacticalSensor
        Tactical_Config_States,             //r:P_Tactical_Sensor_PSM_Topics g:TacticalSensor

        Resource_Specification_Incoming,           //r:P_Maintenance_PSM_Rows g:Maintenance
        Resource_Specification_Outgoing,           //r:P_Maintenance_PSM_Rows g:Maintenance
        Resource_Specification_States,             //r:P_Maintenance_PSM_Topics g:Maintenance



        IdentifierXml,                      //r:IdentifiersStoreDBEnum g:Identifier

        // Memory Database
        XML_StoreDB,                                //r:XML_StoreDBEnum g:ParamStore

        NumberOfDatabases
    };

    enum ProjectLogID
    {
        NUM_OF_PROJECT_LOGS
    };
};
