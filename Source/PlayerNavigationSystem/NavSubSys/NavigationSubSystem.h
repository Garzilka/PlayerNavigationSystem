// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "NavigationSubSystem.generated.h"

USTRUCT(BlueprintType)
struct FPointsGroup
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<UPointManager*> Points;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FUpdatePointSignature, class UPointManager*, PointManager);

UCLASS()
class PLAYERNAVIGATIONSYSTEM_API UNavigationSubSystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	UPROPERTY()
	FUpdatePointSignature OnDrawPoint;
	
protected:

	UPROPERTY()
	TMap<FName, FPointsGroup> Points;

	UPROPERTY()
	APlayerController* ChachePlayerController = nullptr;
public:
	UFUNCTION(BlueprintCallable, Category = "Navigation")
	UPointManager* DrawPoint(FName PointsGroup, FDrawPointData DrawPointData);
	
	UFUNCTION(BlueprintCallable, Category = "Navigation")
	bool RemovePoint(FName PointsGroup, FName PointUID);
	
	UFUNCTION(BlueprintCallable, Category = "Navigation")
	bool RemovePointsGroup(FName PointsGroup);

	UFUNCTION(BlueprintCallable, Category = "Navigation")
	const TMap<FName, FPointsGroup>& GetPoints() const {return Points;};

	APlayerController* GetPlayerController();
};
