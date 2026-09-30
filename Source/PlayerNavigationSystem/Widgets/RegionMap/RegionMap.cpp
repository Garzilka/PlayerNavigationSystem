// Fill out your copyright notice in the Description page of Project Settings.


#include "RegionMap.h"

#include "Blueprint/WidgetLayoutLibrary.h"
#include "Components/CanvasPanel.h"
#include "Components/SizeBox.h"
#include "Kismet/GameplayStatics.h"
#include "PlayerNavigationSystem/Data/MapSettings.h"
#include "PlayerNavigationSystem/Library/NavigationLibrary.h"
#include "PlayerNavigationSystem/NavSubSys/NavigationSubSystem.h"
#include "PlayerNavigationSystem/Object/PointManager.h"


#pragma region Engine

void URegionMap::NativeConstruct()
{
	OpenedLevelName = GameLevelName = UGameplayStatics::GetCurrentLevelName(GetWorld());

	UNavigationSubSystem* NavSys = GetWorld()->GetSubsystem<UNavigationSubSystem>();

	if (!NavSys) { Super::NativeConstruct(); return; }

	NavSys->OnDrawPoint.AddDynamic(this, &URegionMap::OnDrawPoint);

	for (auto Point : NavSys->GetPoints())
	{
		for (auto LPoint : Point.Value.Points)
		{
			if (!IsValid(LPoint)) continue;
			OnDrawPoint(LPoint);			
		}
	}

	Super::NativeConstruct();
}

void URegionMap::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	if (DimensionMap <= 0.f) { Super::NativeTick(MyGeometry, InDeltaTime); return; }

	if (MainCanvasPanel) { MainCanvasPanelGeometry = MainCanvasPanel->GetCachedGeometry(); }

	UpdateMapMaterialParams();
	UpdateMainPlayerPoint();
	UpdatePointsMap(InDeltaTime);
	UpdateZoomMouseMap(InDeltaTime);
	UpdateMoveMouseMap(InDeltaTime);

	Super::NativeTick(MyGeometry, InDeltaTime);
}

#pragma endregion

#pragma region WidgetEvents

void URegionMap::SetParamsMouseMove(const FPointerEvent& InMouseEvent)
{
	const FVector2D LCursorDelta = InMouseEvent.GetCursorDelta();
	const float LViewportSize = 540.f / UWidgetLayoutLibrary::GetViewportWidgetGeometry(GetWorld()).GetAbsoluteSize().Y;
	const float LClampValue = DimensionMap / 2.f;

	const float LX = DimensionMap / 2048.f * LCursorDelta.Y * (ZoomMap * LViewportSize);
	const float LY = DimensionMap / 2048.f * LCursorDelta.X * (ZoomMap * LViewportSize);

	XMap = FMath::Clamp<float>(LX + XMap, -LClampValue, LClampValue);
	YMap = FMath::Clamp<float>(LY + YMap, -LClampValue, LClampValue);
	CursorDeltaMap = FVector2D(LX, LY);
}

#pragma endregion

#pragma region Create_Remove_Point

void URegionMap::OnDrawPoint(UPointManager* InPointManager)
{
	if (!InPointManager) return;

	InPointManager->OnUpdatePoint.AddUniqueDynamic(this, &URegionMap::OnDrawPoint);
	InPointManager->OnRemovePoint.AddUniqueDynamic(this, &URegionMap::OnRemovePoint);
	
	if (InPointManager->GetPointData().TargetWorld != OpenedLevelName)
	{
		return;
	}

	FPointVisibilityInfo VisibilitySettings;
	if(!UNavigationLibrary::GetSettingVisibilityPoint(VisibilitySettings, GetWorld(), InPointManager->GetPointData().PointTag, NavigationDrawTypeTags::RegionMap))
	{
		if (auto Point = PointsMap.Find(InPointManager))
		{
			if (URegionMapPoint* LPoint = *Point)
			{
				LPoint->RemoveFromParent();
				PointsMap.Remove(InPointManager);
			}
		}
		return;
	}
	
	if (PointsMap.Contains(InPointManager)) return;
	
	CreateMapPoint(InPointManager);
}

void URegionMap::OnRemovePoint(UPointManager* InPointManager)
{
	if (!InPointManager) return;
	PointsMap.Remove(InPointManager);
	
	InPointManager->OnUpdatePoint.RemoveDynamic(this, &URegionMap::OnDrawPoint);
}

URegionMapPoint* URegionMap::CreateMapPoint(UPointManager* pointManager)
{
	URegionMapPoint* newPoint = K2_CreateMapPoint(pointManager);

	if (!pointManager || !newPoint) { return newPoint; }
	
	PointsMap.Add(pointManager, newPoint);
	return newPoint;
}

#pragma endregion

#pragma region UPDATE_MapMove

/* 
* PointsMove check in UpdatePointsMap 
*/

void URegionMap::UpdateMapMaterialParams()
{
	if (!MaterialDynamicMap) { UE_LOG(LogTemp, Error, TEXT("MaterialDynamicMap invalid!")); return; }

	MaterialDynamicMap->SetScalarParameterValue("X", XMap);
	MaterialDynamicMap->SetScalarParameterValue("Y", YMap * -1.f);
	MaterialDynamicMap->SetScalarParameterValue("Zoom", ZoomMap);
}

void URegionMap::UpdateZoomMouseMap(float InDeltaTime)
{
	ZoomMap = FMath::FInterpConstantTo(ZoomMap, ZoomCurrentMap, InDeltaTime, ZoomSpeedMouseMap);

	if (FMath::IsNearlyEqual(ZoomMap, ZoomCurrentMap, 0.001f))
	{
		RemoveModificationHide("MouseZoom");
	}
	else
	{
		AddModificationHide("MouseZoom");
	}
}

void URegionMap::UpdateMoveMouseMap(float InDeltaTime)
{
	if (bDraggingMap) { return; }

	XMap = FMath::FInterpTo(XMap, CurrentXMap, InDeltaTime, InertiaLerpSpeedMap);
	YMap = FMath::FInterpTo(YMap, CurrentYMap, InDeltaTime, InertiaLerpSpeedMap);
}

void URegionMap::SetIconSizeByZoom(URegionMapPoint* InPoint)
{
	if (!InPoint) return;
	InPoint->SetRenderScale(GetIconSizeByZoom());
}

#pragma endregion

#pragma region UPDATE_PlayerAndTeam

void URegionMap::UpdateMainPlayerPoint()
{
	if (!MainPlayerPoint) return;
	if (!IsGameLevelOpened() || !GetOwningPlayerPawn()) { MainPlayerPoint->SetVisibility(ESlateVisibility::Collapsed); return; }

	MainPlayerPoint->SetVisibility(ESlateVisibility::HitTestInvisible);
	MainPlayerPoint->SetRenderTranslation(LocationToMap(GetOwningPlayerPawn()->GetActorLocation()));
	MainPlayerPoint->SetRenderTransformAngle(GetOwningPlayerPawn()->GetActorRotation().Yaw + 180.f);
}

#pragma endregion

#pragma region UPDATE_Points

void URegionMap::UpdatePointsMap(float InDeltaTime)
{
	for (auto& LPoint : PointsMap)
	{
		if (!(GetOwningPlayerPawn() && LPoint.Key && LPoint.Value)) continue;

		if (!LPoint.Value->CheckDistance())
		{
			LPoint.Value->SetVisibility(ESlateVisibility::Collapsed);
		}
		
		RenderPoint(InDeltaTime, LPoint);
		if (LPoint.Value->GetVisibilitySettings().bUseFade) { RenderVisibleDistance(LPoint); }
		
		SetIconSizeByZoom(LPoint.Value);
	}
}

void URegionMap::RenderPoint(float InDeltaTime, TTuple<UPointManager*, URegionMapPoint*>& InPoint)
{
	if (!GetOwningPlayerPawn()) { return; }

	const FVector2D LLocationOnMap = LocationToMap(InPoint.Value->GetLocation());

	InPoint.Value->SetRenderTranslation(LLocationOnMap);
}

#pragma region UPDATE_RenderPoints

void URegionMap::RenderVisibleDistance(TTuple<UPointManager*, URegionMapPoint*>& InPoint)
{
	if (!GetOwningPlayerPawn()) { return; }
	const FVector LPointLocation = InPoint.Value->GetLocation();
	const FVector LPlayerLocation = GetOwningPlayerPawn()->GetActorLocation();

	InPoint.Value->SetRenderOpacity(InPoint.Value->GetOpacity());
}

#pragma endregion

#pragma endregion

#pragma region TOOLS

void URegionMap::SetMapMaterial(UMaterialInstanceDynamic* InMaterialInstanceDynamic)
{
	MaterialDynamicMap = InMaterialInstanceDynamic;
}


FVector2D URegionMap::GetIconSizeByZoom() const
{
	FVector2D LResult(FMath::GetMappedRangeValueClamped(FVector2D(ZoomMinMap, ZoomMaxMap), FVector2D(MaxIconSizeMap, MinIconSizeMap), ZoomMap));
	return LResult;
}

FVector2D URegionMap::TargetLocationToMap(const FVector& InLocation, const URegionMapPoint* InPoint, bool& RefIsClamped) const
{
	if (!InPoint) return {};

	FVector2D LResult = LocationToMap(InLocation);

	float LLocalSizeXMax = (MainCanvasPanelGeometry.GetLocalSize().X / 2.f) - (InPoint->GetDesiredSize() * 0.19f).X;
	float LLocalSizeXMin = LLocalSizeXMax * -1.f;

	LResult.X = FMath::Clamp<float>(LResult.X, LLocalSizeXMin, LLocalSizeXMax);

	return LResult;
}


FVector2D URegionMap::LocationToMap(const FVector& InLocation) const
{
	if (!SizeBoxMap) return {};
	
	const float LOrientation = ZoomMap * (DimensionMap / SizeBoxMap->GetWidthOverride());
	const float LX = (InLocation.Y - OffsetYMap + YMap) / LOrientation;
	const float LY = (InLocation.X - OffsetXMap - XMap) / LOrientation * -1.f;

	return FVector2D(LX, LY);
}


FVector URegionMap::ReverseLocation(const FVector2D& InMapLocation) const
{
	if (!SizeBoxMap) return {};
	if (!GetOwningPlayerPawn()) return {};

	const float LOrientation = DimensionMap / SizeBoxMap->WidthOverride * ZoomMap;

	FVector LResult;

	LResult.X = -(InMapLocation.Y * LOrientation - XMap - OffsetXMap);
	LResult.Y = -(-InMapLocation.X * LOrientation + YMap - OffsetYMap);
	LResult.Z = GetOwningPlayerPawn()->GetActorLocation().Z;

	return LResult;
}


void URegionMap::SetBaseMapAllSettings(const FMapInputSettings& InGeneralMapSettings, const FRegionalMapSettings& InMapSettings)
{
	ZoomSpeedMouseMap = InGeneralMapSettings.ZoomSpeedMouse;
	ZoomSpeedGamepadMap = InGeneralMapSettings.ZoomSpeedGamepad;
	InertiaLerpSpeedMap = InGeneralMapSettings.InertiaLerpSpeed;
	InertiaForceMap = InGeneralMapSettings.InertiaForce;
	ExtentMap = InGeneralMapSettings.Extent;

	SetBaseMapSettings(InMapSettings);
}

void URegionMap::SetBaseMapSettings(const FRegionalMapSettings& InMapSettings)
{
	DimensionMap = InMapSettings.Dimension;
	OffsetXMap = InMapSettings.OffsetX;
	OffsetYMap = InMapSettings.OffsetY;
	ZoomMinMap = InMapSettings.ZoomMin;
	ZoomMaxMap = InMapSettings.ZoomMax;
	ZoomMap = ZoomMaxMap - (ZoomMaxMap - ZoomMinMap) / ExtentMap * InMapSettings.StartExtent;
	ZoomCurrentMap = ZoomMap;
	MaxIconSizeMap = InMapSettings.ClampSizeMax;
	MinIconSizeMap = InMapSettings.ClampSizeMin;

	if (!MaterialDynamicMap) { UE_LOG(LogTemp, Error, TEXT("MaterialDynamicMap invalid!")); return; }

	MaterialDynamicMap->SetTextureParameterValue("TextureMap", InMapSettings.MapTexture.LoadSynchronous());
	MaterialDynamicMap->SetScalarParameterValue("Dimension", DimensionMap);
	MaterialDynamicMap->SetScalarParameterValue("Zoom", ZoomMap);
}

void URegionMap::AddModificationHide(FString InModification)
{
	ModificationsHideMapArray.AddUnique(InModification);
}

void URegionMap::RemoveModificationHide(FString InModification)
{
	ModificationsHideMapArray.Remove(InModification);
}

bool URegionMap::IsMoveTo()
{
	return (IsValid(CurrentMoveTo));
}


#pragma endregion