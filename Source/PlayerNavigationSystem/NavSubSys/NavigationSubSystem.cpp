// Fill out your copyright notice in the Description page of Project Settings.


#include "NavigationSubSystem.h"
#include "PlayerNavigationSystem/Object/PointManager.h"
#include "PlayerNavigationSystem/Data/PointData.h"

UPointManager* UNavigationSubSystem::DrawPoint(FName PointsGroup, FDrawPointData DrawPointData)
{
	auto& GroupData = Points.FindOrAdd(PointsGroup);

	if (DrawPointData.TargetWorld == "")
	{
		DrawPointData.TargetWorld = GetWorld()->GetName();
	}
	
	for (auto Point : GroupData.Points)
	{
		if (Point->GetPointData().PointUID != DrawPointData.PointUID) continue;

		Point->UpdatePointData(DrawPointData);
		return Point;
	}
	
	UPointManager* Result = NewObject<UPointManager>(GetPlayerController(),UPointManager::StaticClass());
	Result->InitPointData(DrawPointData);	
	GroupData.Points.Add(Result);
	
	OnDrawPoint.Broadcast(Result);
	return Result;
}

bool UNavigationSubSystem::RemovePoint(FName PointsGroup, FName PointUID)
{
	auto GroupData = Points.Find(PointsGroup);
	if (!GroupData) return false;
	
	for (int32 Idx = 0; Idx < GroupData->Points.Num(); Idx++)
	{
		if (GroupData->Points[Idx]->GetPointData().PointUID != PointUID) continue;
		GroupData->Points[Idx]->RemovePoint();
		GroupData->Points[Idx]->MarkAsGarbage();
		GroupData->Points.RemoveAt(Idx);
		break;
	}
	
	if (GroupData->Points.Num() == 0)
	{
		Points.Remove(PointsGroup);
	}
	
	return true;
}

bool UNavigationSubSystem::RemovePointsGroup(FName PointsGroup)
{
	auto GroupData = Points.Find(PointsGroup);
	if (!GroupData) return false;
	
	for (int32 Idx = 0; Idx < GroupData->Points.Num(); Idx++)
	{
		GroupData->Points[0]->RemovePoint();
		GroupData->Points[0]->MarkAsGarbage();
		GroupData->Points.RemoveAt(0);
		Idx--;
	}
	Points.Remove(PointsGroup);	
	return true;
}

APlayerController* UNavigationSubSystem::GetPlayerController()
{
	if (IsValid(ChachePlayerController)) return ChachePlayerController;

	ChachePlayerController = GetWorld()->GetFirstPlayerController();
	return ChachePlayerController;
}
