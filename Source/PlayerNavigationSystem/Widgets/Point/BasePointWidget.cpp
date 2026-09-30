// Fill out your copyright notice in the Description page of Project Settings.


#include "BasePointWidget.h"

#include "PlayerNavigationSystem/Library/NavigationLibrary.h"

void UBasePointWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

FVector UBasePointWidget::GetLocation() const
{
	if (!PointManager) return FVector::ZeroVector;

	FVector Result = PointManager->GetPointData().LocationInfo.Position;	
	if (PointManager->GetPointData().LocationInfo.TargetActor.Get())
	{
		Result = PointManager->GetPointData().LocationInfo.TargetActor.Get()->GetActorLocation();
	}
	Result.Z += PointManager->GetPointData().LocationInfo.ZOffset;

	return Result;	
}

bool UBasePointWidget::HasPointRule(EPointRule Rule) const
{
	return (PointManager && PointManager->HasPointRule(Rule));
}

const FPointVisibilityInfo& UBasePointWidget::GetVisibilitySettings() const
{
	return VisibilitySettings;
}

float UBasePointWidget::GetDistanceToPoint() const
{
	if (!PointManager) return MAX_flt;
	if (!GetOwningPlayerPawn()) return MAX_flt;

	return FVector::Distance(GetOwningPlayerPawn()->GetActorLocation(),
		( GetOwningPlayer()->PlayerCameraManager ? GetOwningPlayer()->PlayerCameraManager->GetCameraLocation() : GetOwningPlayerPawn()->GetActorLocation()));
}

float UBasePointWidget::GetScale() const
{
	if (!(VisibilitySettings.bUseScale && VisibilitySettings.ScaleSettings)) return 1.f;

	float Result = VisibilitySettings.ScaleSettings->GetFloatValue(GetDistanceToPoint());
	return FMath::Clamp(Result, 0.0f, 1.0f);
}

float UBasePointWidget::GetOpacity() const
{
	if (GetLocation().IsNearlyZero())	return 0.f;	
	if (!(VisibilitySettings.bUseFade && VisibilitySettings.FadeSettings)) return 1.f;

	float Result = VisibilitySettings.FadeSettings->GetFloatValue(GetDistanceToPoint());
	return FMath::Clamp(Result, 0.0f, 1.0f);
}

bool UBasePointWidget::CheckDistance() const
{
	if (!(PointManager && GetOwningPlayerPawn())) return false;

	if (VisibilitySettings.MaxDrawDistance <= 0.f) return true;

	return (FVector::Distance(GetOwningPlayerPawn()->GetActorLocation(), GetLocation()) <= VisibilitySettings.MaxDrawDistance);
}

bool UBasePointWidget::IsInterpolate() const
{
	return (HasPointRule(EPointRule::EPR_UseInterpolate));
}

void UBasePointWidget::ScalePoint()
{
	float LScalePoint = GetScale();

	SetRenderScale(FVector2D(LScalePoint, LScalePoint));
}











