// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "NativeGameplayTags.h"
#include "PlayerNavigationSystem/Widgets/Tooltip/PointTooltip.h"
#include "UObject/NoExportTypes.h"
#include "PointData.generated.h"

namespace NavigationPointTypeTags
{
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Default);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(PlayerLocation);
}



USTRUCT(BlueprintType)
struct FPointLocationInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector Position = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ZOffset = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSoftObjectPtr<AActor> TargetActor;
};

USTRUCT(BlueprintType)
struct FDrawPointData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName PointUID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FPointLocationInfo LocationInfo;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText Name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText Description;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayTag PointTag;	

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString TargetWorld = FString();
};


USTRUCT(BlueprintType)
struct FPointVisibilityInfo
{
	GENERATED_BODY();

public:

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (
		DisplayName = "Максимальная дистанция отображения", ToolTip = "-1.f = Виден всегда. Если больше 0, то сразу после этого значения виджет будет скрыт"))
	float MaxDrawDistance = -1.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bUseFade = false;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (EditCondition = "bUseFade", EditConditionHides,
		DisplayName = "Прозрачность к дистанции", XAxisName = "Distance", YAxisName = "Opacity"))
	UCurveFloat* FadeSettings = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bUseScale = false;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (EditCondition = "bUseScale", EditConditionHides,
		DisplayName = "Размер к дистанции", XAxisName = "Distance", YAxisName = "Opacity"))
	UCurveFloat* ScaleSettings = nullptr;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "Виджет тултипа"))
	TSubclassOf<UPointTooltip> TooltipClass;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (DisplayName = "Максимальная дистанция поиска пути", 
		ToolTip="Если количество переходов до нужной карты больше чем указано, тогда точка рисоваться не будет\n -1 без ограничений по дистанции"))
	int32 MaxPathTrace = 1;

	FPointVisibilityInfo() {};
};

UENUM(BlueprintType, meta = (Bitflags, UseEnumValuesAsMaskValuesInEditor = "true"))
enum class EPointRule : uint8
{
	EPR_None				= 0			UMETA(Hidden),
	EPR_CanHover			= 1 << 0	UMETA(DisplayName = "Подсвечивается"),
	EPR_CanTeleport			= 1 << 2	UMETA(DisplayName = "Можно телепортироваться"),
	EPR_ClampCompass		= 1 << 3	UMETA(DisplayName = "Не уходит за пределы компаса"),
	EPR_UseInterpolate		= 1 << 4	UMETA(DisplayName = "Интерполяция позиции"),
	EPR_ViewOnTrailman		= 1 << 5	UMETA(DisplayName = "Отображается на проводнике"),
};

USTRUCT(BlueprintType)
struct FViewSetting
{
	GENERATED_BODY();

public:

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (InlineEditConditionToggle))
	bool bUseMaterial = false;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (EditCondition = "!bUseMaterial", EditConditionHides))
	TSoftObjectPtr<UTexture2D> Texture;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (EditCondition = "!bUseMaterial", EditConditionHides))
	FLinearColor BackgroundColor = FLinearColor::Black;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (EditCondition = "bUseMaterial", EditConditionHides))
	TSoftObjectPtr<UMaterialInstance> Material;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (Bitmask, BitmaskEnum = "/Script/PlayerNavigationSystem.EPointRule", DisplayName = "Правила"))
	int32 Rules = 0;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "Приоритет метки"))
	int32 ZOrder = false;

};

USTRUCT(BlueprintType)
struct FSoundSetting
{
	GENERATED_BODY();

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	USoundBase* SoundCreate = nullptr;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	USoundBase* SoundUpdate = nullptr;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	USoundBase* SoundRemove = nullptr;
};

USTRUCT(BlueprintType)
struct FNavigationPointInfo
{
	GENERATED_BODY();

public:

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FViewSetting ViewSettings;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FSoundSetting SoundSetting;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "PointInfo", meta = (ForceInlineRow))
	TMap<FGameplayTag, FPointVisibilityInfo> DrawSettings;

	FNavigationPointInfo() {};
};

UCLASS(BlueprintType)
class PLAYERNAVIGATIONSYSTEM_API UNavigationPointData : public UDataAsset
{
	GENERATED_BODY()
	

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (ForceInlineRow))
	TMap<FGameplayTag, FNavigationPointInfo> NavigationPointData;
};
