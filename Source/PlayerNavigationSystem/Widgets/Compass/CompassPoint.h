// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerNavigationSystem/Widgets/Point/BasePointWidget.h"
#include "CompassPoint.generated.h"

/**
 * 
 */
UCLASS()
class PLAYERNAVIGATIONSYSTEM_API UCompassPoint : public UBasePointWidget
{
	GENERATED_BODY()

protected:

	void NativeConstruct() override;


	UFUNCTION()
	void Event_UpdatePoint(UPointManager* PointManagerRef);

	bool IsClampNavigation() const;
public:

	float GetX(float InX) const;
};
