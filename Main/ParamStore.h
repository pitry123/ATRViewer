#pragma once
#include <cstdint>
#include <cstring>


#define MAX_CHANNEL_VIDEO_STREAM_INPUT 4

class ParamStore 
{
public:
    enum XML_StoreDBEnum
	{
        DEBUG_ENV_PARAMS,               // t:CommonTypes::MonitorParams
        DDS_PARAMS,                     // t:ParamStore::DDSParams
        UI_PARAMS,                  	// t:ParamStore::UIParams
        COLORS_DETECTION,                // t:ParamStore::ColorsDetectionStruct
        FILTER_CLASSIFICATION,           // t:ParamStore::FilterClassificationStruct
        FILTER_CHANNEL_TYPE,             // t:ParamStore::FilterChannelTypeStruct
        CHANNELS_INPUT_STREAM,           // t:ParamStore::ChannelsInputStruct
        ACTIVE_NTP,                      // t:bool

        NUM_OF_PARAM_STORE_DB_XML
    };

#pragma pack(1)

	struct DDSParams
	{
        int  DomainId;
        char QosFilePathWindows[256];
        char QosFilePathLinux[256];
    };

    struct UIParams
    {
        bool FullScreen;
        char Title[128];
        int ScreenOffsetX;
        int ScreenOffsetY;
        int ScreenHeight;
        int ScreenWidth;
    };



    struct ColorsDetectionStruct
    {
        char unknownColor[16];
        char manColor[16];
        char vehicleColor[16];
        char armoredVehicleColor[16];
        char antiTankColor[16];
        char sniperColor[16];
    };

    struct FilterClassificationStruct
    {
        bool unknownValid;
        bool manValid;
        bool vehicleValid;
        bool armoredVehicleValid;
        bool antiTankValid;
        bool sniperValid;
    };

    struct FilterChannelTypeStruct
    {
        bool ATR;
        bool VMD;
        bool Tracker;
    };

    struct ChannelInputParams
    {
        char SourceName[32];
        int  Latency;
        bool IsActive;
    };

    struct ChannelsInputStruct
    {
        ChannelInputParams channels[MAX_CHANNEL_VIDEO_STREAM_INPUT]; //t:ParamStore::ChannelInputParams
    };

#pragma pack()
};

