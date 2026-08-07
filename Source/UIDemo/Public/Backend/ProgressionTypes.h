#pragma once

#include "CoreMinimal.h"
#include "Backend/BackendTypes.h"
#include "ProgressionTypes.generated.h"

UENUM(BlueprintType)
enum class ESkillNodeState : uint8
{
	Locked,
	Available,
	Unlocked
};

USTRUCT(BlueprintType)
struct UIDEMO_API FSkillNodeData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Progression")
	FName SkillId = NAME_None;

	UPROPERTY(BlueprintReadOnly, Category = "Progression")
	FText DisplayName;

	UPROPERTY(BlueprintReadOnly, Category = "Progression")
	FText Description;

	UPROPERTY(BlueprintReadOnly, Category = "Progression")
	int32 Cost = 0;

	UPROPERTY(BlueprintReadOnly, Category = "Progression")
	TArray<FName> PrerequisiteIds;

	UPROPERTY(BlueprintReadOnly, Category = "Progression")
	int32 Row = 0;

	UPROPERTY(BlueprintReadOnly, Category = "Progression")
	int32 Column = 0;

	UPROPERTY(BlueprintReadOnly, Category = "Progression")
	ESkillNodeState State = ESkillNodeState::Locked;
};

USTRUCT(BlueprintType)
struct UIDEMO_API FProgressionData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Progression")
	int32 SkillPoints = 0;

	UPROPERTY(BlueprintReadOnly, Category = "Progression")
	TArray<FSkillNodeData> Nodes;
};

USTRUCT(BlueprintType)
struct UIDEMO_API FProgressionResponse
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Progression")
	bool bRequestSucceeded = false;

	UPROPERTY(BlueprintReadOnly, Category = "Progression")
	bool bFromCache = false;

	UPROPERTY(BlueprintReadOnly, Category = "Progression")
	FProgressionData Progression;

	UPROPERTY(BlueprintReadOnly, Category = "Progression")
	FBackendError Error;
};

USTRUCT(BlueprintType)
struct UIDEMO_API FUnlockSkillResponse
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Progression")
	bool bRequestSucceeded = false;

	UPROPERTY(BlueprintReadOnly, Category = "Progression")
	FName UnlockedSkillId = NAME_None;

	UPROPERTY(BlueprintReadOnly, Category = "Progression")
	int32 RemainingSkillPoints = 0;

	UPROPERTY(BlueprintReadOnly, Category = "Progression")
	FBackendError Error;
};
