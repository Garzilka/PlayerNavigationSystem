// Fill out your copyright notice in the Description page of Project Settings.


#include "MapSettings.h"


namespace NavigationDrawTypeTags
{
	UE_DEFINE_GAMEPLAY_TAG(Compass,			TEXT("NavSys.DrawType.Compass"));
	UE_DEFINE_GAMEPLAY_TAG(RegionMap,		TEXT("NavSys.DrawType.RegionMap"));
	UE_DEFINE_GAMEPLAY_TAG(MainMap,			TEXT("NavSys.DrawType.MainMap"));
	UE_DEFINE_GAMEPLAY_TAG(Scene,			TEXT("NavSys.DrawType.Scene"));
}

FMapData UMapSettings::GetSettingsByMapName(const FString& InMapName) const
{
	if (InMapName.IsEmpty()) return {};

	for (auto& MapAndSettings : MapsAndSettings)
	{
		if (MapAndSettings.Key.GetAssetName() == InMapName)
		{
			return MapAndSettings.Value;
		}
	}
	return {};
}
