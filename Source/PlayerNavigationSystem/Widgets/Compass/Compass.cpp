// Fill out your copyright notice in the Description page of Project Settings.


#include "Compass.h"

#include "CompassPoint.h"
#include "ScenePoint.h"
#include "Blueprint/SlateBlueprintLibrary.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Components/CanvasPanel.h"
#include "Components/Image.h"
#include "Components/Overlay.h"
#include "Kismet/KismetMathLibrary.h"
#include "PlayerNavigationSystem/Data/MapSettings.h"
#include "PlayerNavigationSystem/Library/NavigationLibrary.h"

#pragma region ENGINE

void UCompass::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	UpdateScenePoints(MyGeometry, InDeltaTime);
	UpdateCompassPoints(MyGeometry, InDeltaTime);
	
	if (IsValid(MaterialWNESDynamic))
	{
		if (GetOwningPlayer() && GetOwningPlayer()->PlayerCameraManager)
		{
			FRotator ControllRotation = GetOwningPlayer()->PlayerCameraManager->ViewTarget.POV.Rotation;
			MaterialWNESDynamic->SetScalarParameterValue(MaterialWNESOffsetParamName, UKismetMathLibrary::ClampAxis(ControllRotation.Yaw) / 360.f);
		}

	}
	
	Super::NativeTick(MyGeometry, InDeltaTime);
}

void UCompass::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	UNavigationSubSystem* NavSys = GetWorld()->GetSubsystem<UNavigationSubSystem>();
	if (!NavSys) return;

	NavSys->OnDrawPoint.AddDynamic(this, &UCompass::OnDrawPoint);

	WorldName = (GetWorld()->GetName());

	for (auto Pair : NavSys->GetPoints())
	{
		for (auto PointData : Pair.Value.Points)
		{
			if (!PointData) continue;
			OnDrawPoint(PointData);
		}
	}
}

void UCompass::NativeConstruct()
{
	Super::NativeConstruct();
	
	if (ImageWNES)
	{
		MaterialWNESDynamic = ImageWNES->GetDynamicMaterial();
		if (IsValid(MaterialWNESDynamic))
		{
			MaterialWNESDynamic->SetScalarParameterValue(MaterialWNESBaseRotateParamName, MatCompasOffset);
		}
	}
}

#pragma endregion ENGINE


#pragma region NAVIGATOR_UPDATE

void UCompass::UpdateScenePoints(const FGeometry& MyGeometry, float InDeltaTime)
{
	for (auto& Point : ScenePoints)
	{
		UPointManager* PointManager = Point.Key;
		if (!PointManager) return;

		UScenePoint* LNavigationPoint = Point.Value;
		if (!LNavigationPoint) continue;

		if (!LNavigationPoint->CheckDistance())
		{
			LNavigationPoint->SetVisibility(ESlateVisibility::Collapsed);
			continue;
		}

		LNavigationPoint->SetVisibility(ESlateVisibility::HitTestInvisible);
		UpdateNavigationPoint(InDeltaTime, LNavigationPoint);
	}
}

#pragma endregion NAVIGATOR_UPDATE

void UCompass::UpdateCompassPoints(const FGeometry& MyGeometry, float InDeltaTime)
{
	for (auto& Point : CompassPoints)
	{		
		UPointManager* PointManager = Point.Key;
		UCompassPoint* LCompassPoint = Point.Value;
		if (!(PointManager && LCompassPoint)) continue;

		// If no need to draw point - hide widget
		if (!LCompassPoint->CheckDistance())
		{
			LCompassPoint->SetVisibility(ESlateVisibility::Collapsed);
			continue;
		}
		UpdateCompassPoint(InDeltaTime, LCompassPoint);
	}
}

float UCompass::CalculatePointPositionX(const FVector& WorldTarget, const FGeometry& Geometry, const FVector2D& Position)
{
	constexpr float Distance = TNumericLimits<float>::Max();
	if (!(GetOwningPlayer() && GetOwningPlayer()->GetPawn())) return Distance;

	APawn* Pawn = GetOwningPlayer()->GetPawn();
	APlayerCameraManager* CamMng = GetOwningPlayer()->PlayerCameraManager;

	const FVector CameraRightVector = CamMng->GetActorRightVector();
	const FVector PawnForwardVector = Pawn->GetActorForwardVector();
	const FVector TargetDirection = (CamMng->GetCameraLocation() - WorldTarget).GetSafeNormal();
	
	const auto TargetDirection2D = FVector2D(TargetDirection.X, TargetDirection.Y).GetSafeNormal();
	const auto CameraRightVector2D = FVector2D(CameraRightVector.X, CameraRightVector.Y).GetSafeNormal();
	const auto PawnForwardVector2D = FVector2D(PawnForwardVector.X, PawnForwardVector.Y).GetSafeNormal();

	FVector2D Projected;
	FVector2D PixelPosition, ViewportPosition;
	UWidgetLayoutLibrary::ProjectWorldLocationToWidgetPosition(GetOwningPlayer(), WorldTarget, Projected, false);
	USlateBlueprintLibrary::LocalToViewport(GetOwningPlayer(), Geometry, Position, PixelPosition, ViewportPosition);
	Projected -= ViewportPosition;

	float DirectionDot;
	bool bBehind;
	{
		const float Dot = TargetDirection | PawnForwardVector;
		const float Angle = FMath::RadiansToDegrees(FMath::Acos(Dot));
		DirectionDot = FMath::Abs(Angle - 180.f) / 5.f;
		bBehind = FMath::Abs(Angle) < 90.f;
	}

	if (!bBehind)
		return Projected.X - 352.f;

	const float Dot1 = -FMath::Sign(TargetDirection2D | CameraRightVector2D);
	const float Dot2 = FMath::RadiansToDegrees(FMath::Acos( -1.f * (PawnForwardVector2D | TargetDirection2D) ));
	const float Dot3 = Dot1 * DirectionDot * Dot2;
	
	return Dot3 > 0.f ? 10000.f : -10000.f;
}
void UCompass::UpdateCompassPoint(float InDeltaTime, UCompassPoint* InCompassPoint)
{
	float LDistanceToPoint = InCompassPoint->GetDistanceToPoint();
	if (!PointsOverlay) return;

	FGeometry LCacheGeometry = PointsOverlay->GetCachedGeometry();
	float Size = ((LCacheGeometry.GetAbsoluteSize().X - 10.f) / 2.f);
	FVector2D LCacheTranslation = PointsOverlay->RenderTransform.Translation;

	float LPointPositionX = CalculatePointPositionX(InCompassPoint->GetLocation(), LCacheGeometry, LCacheTranslation);
	// LPointPositionX *= 0.8f;

	FWidgetTransform LPointTransform;
	LPointTransform.Translation = FVector2D(InCompassPoint->GetX(LPointPositionX), 0.f);
	LPointTransform.Scale = FVector2D(InCompassPoint->GetScale());

	InCompassPoint->SetRenderTransform(LPointTransform);
	
	float LPointOpacity = InCompassPoint->GetOpacity();
	LPointOpacity = (LPointOpacity * 1.4f) * (1.f - FMath::Clamp(FMath::Abs(LPointPositionX), 0.f, Size) / Size);
	
	if (InCompassPoint->GetPointManager() && InCompassPoint->GetPointManager()->HasPointRule(EPointRule::EPR_ClampCompass))
	{
		LPointOpacity = FMath::Clamp(LPointOpacity, 0.5f, 1.f);
	}
	InCompassPoint->SetRenderOpacity(LPointOpacity);
}

void UCompass::UpdateNavigationPoint(float InDeltaTime, UScenePoint* InNavigationPoint)
{
	if (!InNavigationPoint->CheckDistance())
	{
		InNavigationPoint->SetVisibility(ESlateVisibility::Collapsed);
	}
	
	FVector LPointLocation = InNavigationPoint->GetLocation();
	FVector LCurLoation;
	FVector LLastLoation;
	
	if (InNavigationPoint->IsInterpolate())
	{
		LCurLoation = InNavigationPoint->GetLocation();
		LLastLoation = InNavigationPoint->LastLocation;

		FVector LResult = UKismetMathLibrary::VInterpTo(LLastLoation, LCurLoation, InDeltaTime, TimeInterpolate);
		LPointLocation = LResult;
		InNavigationPoint->LastLocation = LResult;
	}
	
	FVector2D LOutPointTranslation;
	float LOutPointOpacity;

	bool IsNeedDraw = ProjectWorldToScreen(
		GetOwningPlayer(),
		GetPaintSpaceGeometry(),
		LPointLocation,
		InNavigationPoint,
		LOutPointTranslation,
		LOutPointOpacity
	);

	if (!IsNeedDraw)
	{
		InNavigationPoint->SetVisibility(ESlateVisibility::Collapsed);
		return;
	}

	FVector2D LPointImageSize = InNavigationPoint->GetPointImage()->Brush.ImageSize;
	FVector2D LFixedPointImageSize(LPointImageSize.X / 3.7f, LPointImageSize.Y / 2.0f);
	FVector2D LPointTranslation = (LOutPointTranslation - LFixedPointImageSize);	

	InNavigationPoint->SetRenderTranslation(LPointTranslation);

	FGeometry LNestedCanvasPanelGeometry = NestedCanvas->GetCachedGeometry();

	if (LNestedCanvasPanelGeometry.GetLocalSize().Y / 2.f >= LPointTranslation.Y)
	{
		FGeometry LMainCanvasPanelGeometry = MainCanvas->GetCachedGeometry();

		float LPointDesiredSize = InNavigationPoint->GetDesiredSize().X / 2.f;
		float LNestedCanvasPanelLocalSize = LNestedCanvasPanelGeometry.GetLocalSize().X / 2.f;
		float LMainCanvasPanelLocalSize = LMainCanvasPanelGeometry.GetLocalSize().X / 2.f;

		bool LIsPointOnCompassByXLeft = LPointTranslation.X >= LMainCanvasPanelLocalSize - LNestedCanvasPanelLocalSize - LPointDesiredSize;
		bool LIsPointOnCompassByXRight = LPointTranslation.X <= LMainCanvasPanelLocalSize + LNestedCanvasPanelLocalSize - LPointDesiredSize;

		if (LIsPointOnCompassByXLeft && LIsPointOnCompassByXRight)
			LOutPointOpacity = 0.3f;
	}

	InNavigationPoint->SetRenderOpacity(LOutPointOpacity);

	InNavigationPoint->ScalePoint();
}

bool UCompass::ProjectWorldToScreen(APlayerController* OwningPlayer, const FGeometry& Geometry, const FVector& WorldLocation,
	UBasePointWidget* Point, FVector2D& OutScreenPosition, float& OutAlpha)
{
	if (!OwningPlayer) return false;
	const float VisibilityAlpha = Point->GetOpacity();

	// If visibility is nearly zero, widget will be almost invisible. No need to project it to screen
	if (FMath::IsNearlyZero(VisibilityAlpha))
	{
		OutScreenPosition = FVector2D::ZeroVector;
		OutAlpha = 0.f;
		return false;
	}
	
	const bool bSuccess = UWidgetLayoutLibrary::ProjectWorldLocationToWidgetPosition(
		  OwningPlayer
		, WorldLocation
		, OutScreenPosition
		, false
	);

	if (!bSuccess)
	{
		OutScreenPosition = FVector2D::ZeroVector;
		OutAlpha = 0.f;
		return false;
	}
	
	// Get center of the widget in screen coordinates and calculate alpha based on this
	const FVector2D WidgetCenter = Geometry.GetLocalPositionAtCoordinates(FVector2D(.5, .5));
	const float DistanceToWidgetCenter = FVector2D::Distance(OutScreenPosition, WidgetCenter);
	const float ClampedDistance = FMath::Clamp(DistanceToWidgetCenter / 100.f, .55f, 2.f);
	const float WidgetCenterAlpha = DistanceToWidgetCenter < 200.f ? ClampedDistance : 2.f;
	
	// Final visibility
	const float CenterAlpha = WidgetCenterAlpha * VisibilityAlpha;
		
	OutAlpha = FMath::Clamp(CenterAlpha, 0.f, Point->GetVisibilitySettings().MaxDrawDistance <= 0.0f ? FLT_MAX : Point->GetVisibilitySettings().MaxDrawDistance);
	
	return !FMath::IsNearlyZero(OutAlpha);
}


#pragma region POINT_MANAGER_EVENTS

void UCompass::OnDrawPoint_Implementation(UPointManager* PointManager)
{
	if (!PointManager) return;

	PointManager->OnUpdatePoint.AddUniqueDynamic(this, &UCompass::OnDrawPoint);
	PointManager->OnRemovePoint.AddUniqueDynamic(this, &UCompass::OnRemovePoint);
	
	if (PointManager->GetPointData().TargetWorld != WorldName && PointManager->GetPointData().TargetWorld != "")
	{
		return;
	}
	
	FPointVisibilityInfo VisibilitySettings;
	if (UNavigationLibrary::GetSettingVisibilityPoint(VisibilitySettings, GetWorld(), PointManager->GetPointData().PointTag, NavigationDrawTypeTags::Compass))
	{
		CreateCompassPoint(PointManager);
	}
	else
	{
		if (auto Point = CompassPoints.Find(PointManager))
		{
			if (UCompassPoint* LPoint = *Point)
			{
				LPoint->RemoveFromParent();
				CompassPoints.Remove(PointManager);
			}
		}
	}

	if (UNavigationLibrary::GetSettingVisibilityPoint(VisibilitySettings, GetWorld(), PointManager->GetPointData().PointTag, NavigationDrawTypeTags::Scene))
	{
		CreateScenePoint(PointManager);
	}
	else
	{
		if (auto Point = ScenePoints.Find(PointManager))
		{
			if (UScenePoint* LPoint = *Point)
			{
				LPoint->RemoveFromParent();
				ScenePoints.Remove(PointManager);
			}
		}
	}
}

void UCompass::OnRemovePoint_Implementation(UPointManager* PointManager)
{
	if (!PointManager) return;

	PointManager->OnUpdatePoint.RemoveDynamic(this, &UCompass::OnDrawPoint);
	CompassPoints.Remove(PointManager);
	ScenePoints.Remove(PointManager);
}

#pragma endregion POINT_MANAGER_EVENTS

#pragma region TOOLS

UCompassPoint* UCompass::CreateCompassPoint(UPointManager* PointManager)
{
	UCompassPoint* LNewPoint = nullptr;
	
	if (auto LPoint = CompassPoints.Find(PointManager))
	{
		return *LPoint;
	}
	LNewPoint = K2_CreateCompassPoint(PointManager);
	if (LNewPoint)
	{
		CompassPoints.Add(PointManager, LNewPoint);
	}

	return LNewPoint;
}

UScenePoint* UCompass::CreateScenePoint(UPointManager* PointManager)
{
	UScenePoint* LNewPoint = nullptr;

	if (auto LPoint = ScenePoints.Find(PointManager))
	{
		return *LPoint;
	}
	
	LNewPoint = K2_CreateScenePoint(PointManager);
	if (LNewPoint)
	{
		ScenePoints.Add(PointManager, LNewPoint);
	}
	
	return LNewPoint;
}

void UCompass::OnUpdateNavigationPoint_Implementation(UScenePoint* InNavigationPoint, UPointManager* InPointManager){}
void UCompass::OnUpdateCompassPoint_Implementation(UCompassPoint* InCompassPoint, UPointManager* InPointManager){}

#pragma endregion TOOLS