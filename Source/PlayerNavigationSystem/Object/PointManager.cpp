// Fill out your copyright notice in the Description page of Project Settings.


#include "PointManager.h"

#include "Kismet/GameplayStatics.h"
#include "PlayerNavigationSystem/Library/NavigationLibrary.h"

void UPointManager::SetCompassPoint(UCompassPoint* NewCompassPoint) { CompassPoint = NewCompassPoint; }
void UPointManager::SetNavPoint(UScenePoint* NewNavPoint){ NavPoint = NewNavPoint; };
void UPointManager::SetRegionMapPoint(URegionMapPoint* NewRegionMapPoint) { RegionMapPoint = NewRegionMapPoint; }
void UPointManager::SetGlobalMapPoint(UUserWidget* NewGlobalMapPoint){ GlobalMapPoint = NewGlobalMapPoint; };


void UPointManager::InitPointData(FDrawPointData InPointData)
{

	bool LNeedUpdate = (PointData.PointTag != InPointData.PointTag);
	PointData = InPointData;
	if (!GetWorld()) return;

	UNavigationLibrary::GetSettingViewPoint(ViewSetting, GetWorld(), PointData.PointTag);
	UNavigationLibrary::GetSettingSoundPoint(SoundSetting, GetWorld(), PointData.PointTag);
	if (SoundSetting.SoundCreate)
	{
		UGameplayStatics::PlaySound2D(GetWorld(), SoundSetting.SoundCreate);
	}
	
	OnUpdatePoint.Broadcast(this);
}

void UPointManager::UpdatePointData(FDrawPointData InPointData)
{
	if (!GetWorld()) return;

	bool LNeedUpdate = (PointData.PointTag != InPointData.PointTag);
	PointData = InPointData;

	UNavigationLibrary::GetSettingViewPoint(ViewSetting, GetWorld(), PointData.PointTag);
	UNavigationLibrary::GetSettingSoundPoint(SoundSetting, GetWorld(), PointData.PointTag);
	if (SoundSetting.SoundCreate)
	{
		UGameplayStatics::PlaySound2D(GetWorld(), SoundSetting.SoundCreate);
	}
	
	if (LNeedUpdate)
	{
		OnUpdatePoint.Broadcast(this);
	}
}


bool UPointManager::HasPointRule(EPointRule Rule) const
{	
	return (((uint8)ViewSetting.Rules) & ((uint8)Rule));
}

void UPointManager::RemovePoint()
{
	OnRemovePoint.Broadcast(this);
}
