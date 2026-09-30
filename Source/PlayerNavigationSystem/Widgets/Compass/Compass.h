// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerNavigationSystem/Widgets/Point/BasePointWidget.h"
#include "Compass.generated.h"

/**
 * 
 */
UCLASS()
class PLAYERNAVIGATIONSYSTEM_API UCompass : public UUserWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditDefaultsOnly, Category="Data", meta=(AllowPrivateAccess="true"))
	float TimeInterpolate = 3.0f;

	UPROPERTY(EditDefaultsOnly, Category="Data", meta=(AllowPrivateAccess="true"))
	float FieldOfView = 110.f;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UCanvasPanel* MainCanvas;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UCanvasPanel* NestedCanvas;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UOverlay* PointsOverlay;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UImage* ImageWNES;

	UPROPERTY(BlueprintReadOnly, Category = "WNES")
	class UMaterialInstanceDynamic* MaterialWNESDynamic = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "WNES", meta = (AllowPrivateAccess = "true"))
	FName MaterialWNESOffsetParamName = "Uoffset";

	UPROPERTY(EditDefaultsOnly, Category = "WNES", meta = (AllowPrivateAccess = "true"))
	FName MaterialWNESBaseRotateParamName = "BaseRotate";

	UPROPERTY(EditDefaultsOnly, Category = "WNES", meta = (AllowPrivateAccess = "true"))
	float MatCompasOffset = 0.f;
	
	
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	virtual void NativeOnInitialized() override;
	virtual void NativeConstruct() override;
	
	FString WorldName;
	
	void UpdateScenePoints(const FGeometry& MyGeometry, float InDeltaTime);
	void UpdateCompassPoints(const FGeometry& MyGeometry, float InDeltaTime);
	
	UCompassPoint* CreateCompassPoint(UPointManager* PointManager);
	UScenePoint* CreateScenePoint(UPointManager* PointManager);
	
	UFUNCTION(BlueprintImplementableEvent, Category="Compass")
	UCompassPoint* K2_CreateCompassPoint(UPointManager* PointManager);

	UFUNCTION(BlueprintImplementableEvent, Category="Compass")
	UScenePoint* K2_CreateScenePoint(UPointManager* PointManager);

	UFUNCTION(BlueprintImplementableEvent, Category = "Compass", DisplayName = "Get Quest Compass")
	UOverlay* K2_GetQuestCompas();

	UFUNCTION(BlueprintImplementableEvent, Category = "Compass", DisplayName = "Get Main Canvas Panel")
	UCanvasPanel* K2_GetMainCanvasPanel();

	UFUNCTION(BlueprintImplementableEvent, Category = "Compass", DisplayName = "Get Nested Canvas Panel")
	UCanvasPanel* K2_GetNestedCanvasPanel();

	UFUNCTION(BlueprintNativeEvent, Category="Compass")
	void OnUpdateCompassPoint(UCompassPoint* InCompassPoint, UPointManager* InPointManager);

	UFUNCTION(BlueprintNativeEvent, Category="Compass")
	void OnUpdateNavigationPoint(UScenePoint* InNavigationPoint, UPointManager* InPointManager);
	
	UFUNCTION(BlueprintNativeEvent, Category="Compass")
	void OnDrawPoint(UPointManager* PointManager);

	UFUNCTION(BlueprintNativeEvent, Category="Compass")
	void OnRemovePoint(UPointManager* PointManager);
	
	UFUNCTION(BlueprintCallable, Category="Compass")
	float CalculatePointPositionX( const FVector& WorldTarget, const FGeometry& Geometry, const FVector2D& Position);

	void UpdateCompassPoint(float InDeltaTime, UCompassPoint* InCompassPoint);

	void UpdateNavigationPoint(float InDeltaTime, UScenePoint* InNavigationPoint);
	
	/**
	 * Project point to screen
	 * @param [in]	WorldLocation		World location to project to screen
	 * @param [in]	MaxDistance			Visibility settings
	 * @param [in]	bIsTarget			Is target?
	 * @param [in]	TargetMarkDeviation Z-axis world location offset
	 * @param [out] OutScreenPosition	Calculated screen position
	 * @param [out] OutAlpha			Calculated visibility alpha value
	 * @return True if actually projected
	 */
	UFUNCTION(BlueprintCallable, Category="Compass")
	static bool ProjectWorldToScreen(APlayerController* OwningPlayer, const FGeometry& Geometry, const FVector& WorldLocation, UBasePointWidget* Point, FVector2D& OutScreenPosition, float& OutAlpha);

private:
	/** Compass points */
	UPROPERTY(Transient)
	TMap<UPointManager*, UCompassPoint*> CompassPoints;
	
	/** Navigation points */
	UPROPERTY(Transient)
	TMap<UPointManager*, UScenePoint*> ScenePoints;
};
