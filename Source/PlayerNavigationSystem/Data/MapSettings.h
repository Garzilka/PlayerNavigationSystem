// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NativeGameplayTags.h"
#include "Engine/DataAsset.h"
#include "MapSettings.generated.h"


namespace NavigationDrawTypeTags
{
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Compass);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(RegionMap);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(MainMap);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Scene);
}

USTRUCT(BlueprintType)
struct FMapInputSettings
{
	GENERATED_BODY()
				

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (DisplayName = "Скорость масштабирования (мышь)"))
	float ZoomSpeedMouse = 2.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (DisplayName = "Скорость масштабирования (геймпад)"))
	float ZoomSpeedGamepad = 0.01f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (DisplayName = "Скорость инерции перемещения (интерполяции)"))
	float InertiaLerpSpeed = 5.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (DisplayName = "Сила инерции перемещения"))
	float InertiaForce = 10.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (ClampMin = "1", ClampMax = "10",	DisplayName = "Кол-во степеней масштабирования"))
	int32 Extent = 3;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (DisplayName = "Скорость масштабирования (мышь) (глоб.)"))
	float GMZoomSpeedMouse = 2.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (DisplayName = "Скорость масштабирования (геймпад) (глоб.)"))
	float GMZoomSpeedGamepad = 0.01f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (
		DisplayName = "Скорость инерции перемещения (интерполяции) (глоб.)"))
	float GMInertiaLerpSpeed = 5.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (
		DisplayName = "Сила инерции перемещения (глоб.)"))
	float GMInertiaForce = 10.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (ClampMin = "1", ClampMax = "10",
	DisplayName = "Кол-во степеней масштабирования (глоб.)"))
		int32 GMExtent = 3;

};

USTRUCT(BlueprintType)
struct FRegionalMapSettings
{
	GENERATED_BODY()

public:	

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (DisplayName = "Текстура карты"))
	TSoftObjectPtr<UTexture2D> MapTexture;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (ClampMin = "0"), meta = (
		DisplayName = "Размер карты",	ToolTip = "Размер карты - это Ortho Width в Scene Capture 2D."))
	float Dimension;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (DisplayName = "Смещение по X",
		ToolTip = "Необходимо для правильного отображения позиций с глобальных координат на карте. X берётся из Location.X Scene Capture 2D"))
	float OffsetX;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (DisplayName = "Смещение по Y",
		ToolTip = "Необходимо для правильного отображения позиций с глобальных координат на карте. Y берётся из Location.Y Scene Capture 2D"))
	float OffsetY;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (ClampMin = "0", ClampMax = "10",	DisplayName = "Начальная степень масштабирования"))
	int32 StartExtent = 1;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (ClampMin = "0", ClampMax = "10",	DisplayName = "Мин. масштабирование"))
	float ZoomMin = 0.25f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (ClampMin = "0", ClampMax = "10",	DisplayName = "Макс. масштабирование"))
	float ZoomMax = 1.5f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (ClampMin = "0", ClampMax = "10",	DisplayName = "Мин. резмер иконок"))
	float ClampSizeMin = 0.6f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (ClampMin = "0", ClampMax = "10",	DisplayName = "Макс. резмер иконок"))
	float ClampSizeMax = 1.f;

};

USTRUCT(BlueprintType)
struct FGlobalMapSettings
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (DisplayName = "Текстура карты"))
	TSoftObjectPtr<UTexture2D> MapTexture;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (DisplayName = "Имя слота в материале"))
	FName MaterialSlot;


};

USTRUCT(BlueprintType)
struct FMapData
{
	GENERATED_BODY()
		
public:	

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (
		DisplayName = "Навзание карты"))
	FText MapName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (
		DisplayName = "Настройки региональной карты"))
	FRegionalMapSettings RegionalMapSettings;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (DisplayName = "Настройки глобальной карты"))
	FGlobalMapSettings GlobalMapSettings;
};

UCLASS(BlueprintType)
class PLAYERNAVIGATIONSYSTEM_API UMapSettings : public UDataAsset
{
	GENERATED_BODY()

protected:
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (DisplayName = "Настройки карт",
		ToolTip = "Массив ключ-значение, где ключ - ссылка на карту, значение - настройки глобальной карты для ключа."))
	TMap<TSoftObjectPtr<UWorld>, FMapData> MapsAndSettings;
	

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (DisplayName = "Стандартные настройки карт", AdvancedDisplay))
	FMapInputSettings GeneralMapSettings;

public:

	UFUNCTION(BlueprintPure)
	FMapData GetSettingsByMapName(const FString& InMapName) const;
};
