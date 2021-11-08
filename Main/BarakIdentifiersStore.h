//creation_date 01-17-2021_17-03-08 generated_by TC34677
#pragma once

#include <cstdint>
class IdentifiersStore
{
public:
	enum MiscellaneousEnum
	{
		VERY_SHORT_STRING = 10,
		SHORT_STRING = 20,
		MEDIUM_STRING = 100,
		LONG_STRING = 256
	};
	enum IdentifiersStoreDBEnum
	{
		NOGA_CAM_D,//t:ResourceStruct
		NOGA_CAM_N,//t:ResourceStruct
		NOGA_CAM_S,//t:ResourceStruct
		NOGA_LRF,//t:ResourceStruct
		NOGA_VIDEO_SOURCE,//t:ResourceStruct
		NOGA_MOUNT,//t:ResourceStruct
		NOGA_MARKER,//t:ResourceStruct
		NOGA_TACT_SENSOR,//t:ResourceStruct
		NOGA_PANORAMA,//t:ResourceStruct
		TAMAR_DISPLAY,//t:ResourceStruct
		FNC_ROUTING,//t:ResourceStruct
		FNC_TRACKER_GS,//t:ResourceStruct
		FNC_TRACKER_CS,//t:ResourceStruct
		GS_MOUNT,//t:ResourceStruct
		GS_LESHEM_CAM_D,//t:ResourceStruct
		GS_VIDEO_SOURCE,//t:ResourceStruct
		GS_IDAN_CAM_N,//t:ResourceStruct
		HANDLE_COMMANDER,//t:ResourceStruct
		TURRET_MOUNT,//t:ResourceStruct
		TURRET_LRF,//t:ResourceStruct
		TURRET_FCS,//t:ResourceStruct
		TURRET_GUN,//t:ResourceStruct
		TURRET_MIKLA,//t:ResourceStruct
		TURRET_MAG,//t:ResourceStruct
		HULL_MOUNT,//t:ResourceStruct
		HULL_DRV_CAM_FRONT,//t:ResourceStruct
		HULL_DRV_CAM_BACK,//t:ResourceStruct
		HULL_VID_SRC_FRONT,//t:ResourceStruct
		HULL_VID_SRC_BACK,//t:ResourceStruct
		NAVIGATION_TMAPS,//t:ResourceStruct
		SAPIR_DISPLAY,//t:ResourceStruct
		SAPIR_MOUNT,//t:ResourceStruct
		SAPIR_ROUTING,//t:ResourceStruct
		SAPIR_VIDEO_SRC,//t:ResourceStruct
		SAPIR_DIRECT_CAM_D,//t:ResourceStruct
		SAPIR_DIRECT_CAM_N,//t:ResourceStruct
		SAPIR_F_VIDEO_SRC,//t:ResourceStruct
		SAPIR_F_CAM_D,//t:ResourceStruct
		SAPIR_F_CAM_N,//t:ResourceStruct
		SAPIR_F_ATR_DAY,//t:ResourceStruct
		SAPIR_F_ATR_NIGHT,//t:ResourceStruct
		SAPIR_F_ATR_FUSION,//t:ResourceStruct
		SAPIR_R_VIDEO_SRC,//t:ResourceStruct
		SAPIR_R_CAM_D,//t:ResourceStruct
		SAPIR_R_CAM_N,//t:ResourceStruct
		SAPIR_R_ATR_DAY,//t:ResourceStruct
		SAPIR_R_ATR_NIGHT,//t:ResourceStruct
		SAPIR_R_ATR_FUSION,//t:ResourceStruct
		SAPIR_L_VIDEO_SRC,//t:ResourceStruct
		SAPIR_L_CAM_D,//t:ResourceStruct
		SAPIR_L_CAM_N,//t:ResourceStruct
		SAPIR_L_ATR_DAY,//t:ResourceStruct
		SAPIR_L_ATR_NIGHT,//t:ResourceStruct
		SAPIR_L_ATR_FUSION,//t:ResourceStruct
		SAPIR_B_VIDEO_SRC,//t:ResourceStruct
		SAPIR_B_CAM_D,//t:ResourceStruct
		SAPIR_B_CAM_N,//t:ResourceStruct
		SAPIR_B_ATR_DAY,//t:ResourceStruct
		SAPIR_B_ATR_NIGHT,//t:ResourceStruct
		SAPIR_B_ATR_FUSION,//t:ResourceStruct
		GUNNER_DISPLAY,//t:ResourceStruct
		LOADER_DISPLAY,//t:ResourceStruct
		AC_SYSTEM,//t:ResourceStruct
		RENAISSANCE_SYSTEM,//t:ResourceStruct
		MISSION_NOGA_FUSION,//t:ResourceStruct
		MISSION_PRIORITY,//t:ResourceStruct
		MISSION_FUSION_AREA,//t:ResourceStruct
		RADAR_SENSOR,//t:ResourceStruct
		RADAR_FUSION,//t:ResourceStruct
		NAMER_TACT_SENSOR,//t:ResourceStruct
		NUM_OF_IDENTIFIERS
	};

#pragma pack(1)
	struct ResourceStruct
	{
		char strSystemDescriptor[SHORT_STRING];
		char strModuleDescriptor[SHORT_STRING];
		uint16_t nSystemID;
		uint16_t nModuleID;
		char strTypeName[SHORT_STRING];
		char strClassification[LONG_STRING];
	};
#pragma pack()

};
