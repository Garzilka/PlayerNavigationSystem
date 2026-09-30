// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PlayerNavigationSystem/Data/MapSettings.h"
#include "PlayerNavigationSystem/Data/PointData.h"
#include "Engine/DeveloperSettings.h"
#include "NavigationSettings.generated.h"

/**
 * 
*/
UCLASS(Config = "Game", defaultconfig, meta = (DisplayName = "Player Navigation System"))
class PLAYERNAVIGATIONSYSTEM_API UNavigationSetting : public UDeveloperSettings
{
	GENERATED_BODY()
	
public:
	UPROPERTY(Config, EditAnywhere, Category = "Player Navigation System")
	int32 TestValue;
	
	UPROPERTY(Config, EditAnywhere, Category = "Player Navigation System")
	TSoftObjectPtr<UNavigationPointData> NavPointData;
	
	UPROPERTY(Config, EditAnywhere, Category = "Player Navigation System")
	TSoftObjectPtr<UMapSettings> MapSetting;
};
