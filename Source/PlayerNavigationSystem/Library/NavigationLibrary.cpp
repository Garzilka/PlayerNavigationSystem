// Fill out your copyright notice in the Description page of Project Settings.


#include "NavigationLibrary.h"

#include "GameplayTagContainer.h"
#include "PlayerNavigationSystem/Settings/NavigationSettings.h"

bool UNavigationLibrary::GetSettingVisibilityPoint(FPointVisibilityInfo& Info, const UObject* WorldContextObject, FGameplayTag InPointTag, FGameplayTag InPointTypeVisible)
{
	UNavigationPointData* NavPointData = UNavigationLibrary::GetNavigationPointData();
	if (!NavPointData) return false;

	FNavigationPointInfo* LTempInfoPoint = NavPointData->NavigationPointData.Find(InPointTag);
	if (!LTempInfoPoint) return false;
	
	FPointVisibilityInfo* LVisibilityInfoPoint = LTempInfoPoint->DrawSettings.Find(InPointTypeVisible);
	if (!LVisibilityInfoPoint) return false;
	
	Info = *LVisibilityInfoPoint;
	return true;
}

bool UNavigationLibrary::GetSettingViewPoint(FViewSetting& Info, const UObject* WorldContextObject,	FGameplayTag InPointTag)
{
	UNavigationPointData* NavPointData = UNavigationLibrary::GetNavigationPointData();
	if (!NavPointData) return false;
	
	FNavigationPointInfo* LTempInfoPoint = NavPointData->NavigationPointData.Find(InPointTag);
	if (!LTempInfoPoint) return false;
	
	Info = LTempInfoPoint->ViewSettings;
	return true;
}

bool UNavigationLibrary::GetSettingSoundPoint(FSoundSetting& Info, const UObject* WorldContextObject, FGameplayTag InPointTag)
{
	UNavigationPointData* NavPointData = UNavigationLibrary::GetNavigationPointData();
	if (!NavPointData) return false;
	
	FNavigationPointInfo* LTempInfoPoint = NavPointData->NavigationPointData.Find(InPointTag);
	if (!LTempInfoPoint) return false;
	
	Info = LTempInfoPoint->SoundSetting;
	return true;
}

UNavigationPointData* UNavigationLibrary::GetNavigationPointData()
{
	if (auto NavSetting = GetMutableDefault<UNavigationSetting>())
	{
		return Cast<UNavigationPointData>(NavSetting->NavPointData.LoadSynchronous());
	}
	
	return nullptr;
}

UMapSettings* UNavigationLibrary::GetMapSettings()
{
	if (auto NavSetting = GetMutableDefault<UNavigationSetting>())
	{
		return Cast<UMapSettings>(NavSetting->MapSetting.LoadSynchronous());
	}
	
	return nullptr;
}
