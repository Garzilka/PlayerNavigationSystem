// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PlayerNavigationSystem/Data/PointData.h"
#include "PlayerNavigationSystem/NavSubSys/NavigationSubSystem.h"
#include "UObject/NoExportTypes.h"
#include "PointManager.generated.h"

/**
 * 
 */
UCLASS()
class PLAYERNAVIGATIONSYSTEM_API UPointManager : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable)
	FUpdatePointSignature OnUpdatePoint;
	
	UPROPERTY(BlueprintAssignable)
	FUpdatePointSignature OnRemovePoint;
	
protected:
	UPROPERTY()
	FDrawPointData PointData;

	UPROPERTY(BlueprintReadOnly)
	FViewSetting ViewSetting;
	
	UPROPERTY(BlueprintReadOnly)
	FSoundSetting SoundSetting;

	UPROPERTY()
	UCompassPoint* CompassPoint;

	UPROPERTY()
	UScenePoint* NavPoint;

	UPROPERTY()
	URegionMapPoint* RegionMapPoint;

	UPROPERTY()
	UUserWidget* GlobalMapPoint;

public:
	UFUNCTION(BlueprintPure)
	UCompassPoint* GetCompassPoint() const { return CompassPoint; };

	UFUNCTION(BlueprintPure)
	UScenePoint* GetNavPoint() const { return NavPoint; };

	UFUNCTION(BlueprintPure)
	URegionMapPoint* GetRegionMapPoint() const { return RegionMapPoint; };

	UFUNCTION(BlueprintPure)
	UUserWidget* GetGlobalMapPoint() const { return GlobalMapPoint; };

	UFUNCTION(BlueprintCallable)
	void SetCompassPoint(UCompassPoint* NewCompassPoint);

	UFUNCTION(BlueprintCallable)
	void SetNavPoint(UScenePoint* NewNavPoint);

	UFUNCTION(BlueprintCallable)
	void SetRegionMapPoint(URegionMapPoint* NewRegionMapPoint);

	UFUNCTION(BlueprintCallable)
	void SetGlobalMapPoint(UUserWidget* NewGlobalMapPoint);
	
	UFUNCTION(BlueprintPure)
	const FDrawPointData& GetPointData() const { return PointData; }
	bool HasPointRule(EPointRule Rule) const;
	
	void InitPointData(FDrawPointData InPointData);
	void UpdatePointData(FDrawPointData InPointData);

	void RemovePoint();
	
};
