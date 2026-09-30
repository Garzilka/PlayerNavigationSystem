// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "RegionMapPoint.h"
#include "Blueprint/UserWidget.h"
#include "Components/SizeBox.h"
#include "Components/CanvasPanel.h"
#include "RegionMap.generated.h"

/**
 * 
 */
UCLASS()
class PLAYERNAVIGATIONSYSTEM_API URegionMap : public UUserWidget
{
	GENERATED_BODY()
	
protected:

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UCanvasPanel* MainCanvasPanel;
	
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	USizeBox* SizeBoxMap;	
	
	UPROPERTY(BlueprintReadOnly)
	FGeometry MainCanvasPanelGeometry;



	UPROPERTY(Transient)
	TMap<UPointManager*, URegionMapPoint*> PointsMap;

	UPROPERTY(BlueprintReadOnly)
	URegionMapPoint* CurrentMoveTo;

	UPROPERTY(BlueprintReadWrite)
	URegionMapPoint* MainPlayerPoint;
	
	UPROPERTY(BlueprintReadWrite)
	FString GameLevelName;

	UPROPERTY(BlueprintReadWrite)
	FString OpenedLevelName;

	UPROPERTY(BlueprintReadWrite)
	bool bDraggingMap = false;

	UPROPERTY(BlueprintReadWrite)
	bool bGamepadNowMap = false;

	UPROPERTY(BlueprintReadWrite)
	int32 ExtentMap = 1;

	UPROPERTY(BlueprintReadWrite)
	float DimensionMap = 0.f;

	UPROPERTY(BlueprintReadWrite)
	float OffsetXMap = 0.f;

	UPROPERTY(BlueprintReadWrite)
	float OffsetYMap = 0.f;

	UPROPERTY(BlueprintReadWrite)
	float ZoomMap = 1.f;

	UPROPERTY(BlueprintReadWrite)
	float ZoomCurrentMap = 1.f;

	UPROPERTY(BlueprintReadWrite)
	float ZoomMinMap = 1.f;

	UPROPERTY(BlueprintReadWrite)
	float ZoomMaxMap = 3.f;

	UPROPERTY(BlueprintReadWrite)
	float XMap = 0.f;

	UPROPERTY(BlueprintReadWrite)
	float YMap = 0.f;

	UPROPERTY(BlueprintReadWrite)
	float CurrentXMap = 0.f;

	UPROPERTY(BlueprintReadWrite)
	float CurrentYMap = 0.f;

	UPROPERTY(BlueprintReadWrite)
	float ZoomSpeedMouseMap = 1.f;

	UPROPERTY(BlueprintReadWrite)
	float ZoomSpeedGamepadMap = 1.f;

	UPROPERTY(BlueprintReadWrite)
	float InertiaLerpSpeedMap = 1.f;

	UPROPERTY(BlueprintReadWrite)
	float InertiaForceMap = 1.f;

	UPROPERTY(BlueprintReadWrite)
	float MaxIconSizeMap = 1.f;

	UPROPERTY(BlueprintReadWrite)
	float MinIconSizeMap = 1.f;

public:

	UFUNCTION(BlueprintCallable)
	void SetTargetTo(URegionMapPoint* NewTarget, bool WitoutSound) { CurrentMoveTo = NewTarget; K2_NewTargetTo(WitoutSound); }

	UFUNCTION(BlueprintCallable)
	void ResetTarget() { CurrentMoveTo = nullptr; }

	UFUNCTION(BlueprintPure)
	bool IsMoveTo();

protected:

	UFUNCTION(BlueprintImplementableEvent, Category = "BaseMap", DisplayName = "Create Map Point")
	URegionMapPoint* K2_CreateMapPoint(UPointManager* InPointManager);

	UFUNCTION(BlueprintImplementableEvent, Category = "BaseMap", DisplayName = "NewTargetTo")
	void K2_NewTargetTo(bool WitoutSound);

	void NativeConstruct() override;

	void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	UFUNCTION()
	void OnDrawPoint(UPointManager* InPointManager);

	UFUNCTION()
	void OnRemovePoint(UPointManager* InPointManager);

	URegionMapPoint* CreateMapPoint(UPointManager* InPointManager);

	UFUNCTION(BlueprintCallable, Category = "BaseMap")
	void SetMapMaterial(UMaterialInstanceDynamic* InMaterialInstanceDynamic);

	UFUNCTION(BlueprintCallable, Category = "BaseMap")
	void SetBaseMapAllSettings(const FMapInputSettings& InGeneralMapSettings, const FRegionalMapSettings& InMapSettings);

	UFUNCTION(BlueprintCallable, Category = "BaseMap")
	void SetBaseMapSettings(const FRegionalMapSettings& InMapSettings);

	UFUNCTION(BlueprintCallable, Category = "BaseMap")
	void SetParamsMouseMove(const FPointerEvent& InMouseEvent);

	UFUNCTION(BlueprintPure, Category = "BaseMap")
	FVector2D LocationToMap(const FVector& InLocation) const;

	UFUNCTION(BlueprintPure, Category = "BaseMap")
	FVector2D TargetLocationToMap(const FVector& InLocation, const URegionMapPoint* InPoint, bool& RefIsClamped) const;

	UFUNCTION(BlueprintPure, Category = "BaseMap")
	FVector ReverseLocation(const FVector2D& InMapLocation) const;

#pragma region Tick
	
	void UpdateMapMaterialParams();

	void UpdateMainPlayerPoint();

	void UpdatePointsMap(float InDeltaTime);

	void UpdateZoomMouseMap(float InDeltaTime);

	void UpdateMoveMouseMap(float InDeltaTime);

#pragma endregion

	void RenderPoint(float InDeltaTime, TTuple<UPointManager*, URegionMapPoint*>& InPoint);

	void RenderVisibleDistance(TTuple<UPointManager*, URegionMapPoint*>& InPoint);

	FVector2D GetIconSizeByZoom() const;

	void SetIconSizeByZoom(URegionMapPoint* InPoint);

	UFUNCTION(BlueprintCallable)
	void AddModificationHide(FString InModification);

	UFUNCTION(BlueprintCallable)
	void RemoveModificationHide(FString InModification);

	bool IsGameLevelOpened() const { return GameLevelName == OpenedLevelName; }
	
	UPROPERTY(BlueprintReadWrite)
	FVector2D CursorDeltaMap = {};

	UPROPERTY(BlueprintReadWrite)
	UMaterialInstanceDynamic* MaterialDynamicMap = nullptr;

	UPROPERTY(BlueprintReadWrite)
	TArray<FString> ModificationsHideMapArray = {};

};
