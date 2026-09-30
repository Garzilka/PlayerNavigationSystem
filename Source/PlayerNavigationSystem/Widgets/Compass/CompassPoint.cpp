// Fill out your copyright notice in the Description page of Project Settings.


#include "CompassPoint.h"

#include "PlayerNavigationSystem/Data/MapSettings.h"
#include "PlayerNavigationSystem/Library/NavigationLibrary.h"

void UCompassPoint::NativeConstruct()
{
	Super::NativeConstruct();
	
	if (PointManager)
	{
		PointManager->OnUpdatePoint.AddUniqueDynamic(this, &UCompassPoint::Event_UpdatePoint);
		Event_UpdatePoint(PointManager);
		PointManager->SetCompassPoint(this);
	}
}

void UCompassPoint::Event_UpdatePoint(UPointManager* PointManagerRef)
{
	UNavigationLibrary::GetSettingVisibilityPoint(VisibilitySettings, GetWorld(), PointManager->GetPointData().PointTag, NavigationDrawTypeTags::Compass);
}

bool UCompassPoint::IsClampNavigation() const
{
	return (PointManager && PointManager->HasPointRule(EPointRule::EPR_ClampCompass));
}

float UCompassPoint::GetX(float InX) const
{
	if (!IsClampNavigation()) return InX;

	return FMath::Clamp<float>(InX, -210.f, 210.f);
}
