// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "NavigationLibrary.generated.h"

/**
 * 
 */
UCLASS()
class PLAYERNAVIGATIONSYSTEM_API UNavigationLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintPure, Category = "NavSubsys Functions | Config", meta = (WorldContext = "WorldContextObject"))
	static bool GetSettingVisibilityPoint(FPointVisibilityInfo& Info, const UObject* WorldContextObject, FGameplayTag InPointTag, FGameplayTag InPointTypeVisible);

	UFUNCTION(BlueprintPure, Category = "NavSubsys Functions | Config", meta = (WorldContext = "WorldContextObject"))
	static bool GetSettingViewPoint(FViewSetting& Info, const UObject* WorldContextObject, FGameplayTag InPointTag);

	UFUNCTION(BlueprintPure, Category = "NavSubsys Functions | Config", meta = (WorldContext = "WorldContextObject"))
	static bool GetSettingSoundPoint(FSoundSetting& Info, const UObject* WorldContextObject, FGameplayTag InPointTag);

	UFUNCTION(BlueprintPure, Category = "NavSubsys Functions | Config", meta = (WorldContext = "WorldContextObject"))
	static UNavigationPointData* GetNavigationPointData();
	
	UFUNCTION(BlueprintPure, Category = "NavSubsys Functions | Config", meta = (WorldContext = "WorldContextObject"))
	static UMapSettings* GetMapSettings();
};
