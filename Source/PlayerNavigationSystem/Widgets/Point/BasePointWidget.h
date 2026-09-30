// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerNavigationSystem/Object/PointManager.h"
#include "BasePointWidget.generated.h"

/**
 * 
 */
UCLASS()
class PLAYERNAVIGATIONSYSTEM_API UBasePointWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UImage* PointImage;
	
	UPROPERTY(BlueprintReadWrite, meta = (ExposeOnSpawn = true))
	UPointManager* PointManager;

	UPROPERTY(BlueprintReadOnly)
	FPointVisibilityInfo VisibilitySettings;
	
public:

	UFUNCTION(BlueprintPure, Category = "NavigationPoint", DisplayName = "Get Point Image")
	UImage* GetPointImage() const {return PointImage;};
	
	FVector LastLocation = FVector::ZeroVector;
	
	UFUNCTION(BlueprintPure)
	FVector GetLocation() const;
	
	UFUNCTION(BlueprintPure)
	bool HasPointRule(EPointRule Rule) const;

	const FPointVisibilityInfo& GetVisibilitySettings() const;

	UPointManager* GetPointManager() const { return PointManager; };

	float GetDistanceToPoint() const;

	float GetScale() const;
	void ScalePoint();

	float GetOpacity() const;
	
	bool CheckDistance() const;
	
	bool IsInterpolate() const;
	
protected:

	void NativeConstruct() override;
};
