#pragma once

#include "CoreMinimal.h"
#include "Backend/BackendTypes.h"
#include "ProgressionTypes.generated.h"

/** @brief Availability states used by nodes in the skill tree. */
UENUM(BlueprintType)
enum class ESkillNodeState : uint8
{
	/** Prerequisites have not been satisfied. */
	Locked,
	/** Prerequisites are satisfied and the node may be unlocked. */
	Available,
	/** The player has already unlocked the node. */
	Unlocked
};

/** @brief Backend payload describing one skill-tree node. */
USTRUCT(BlueprintType)
struct UIDEMO_API FSkillNodeData
{
	GENERATED_BODY()

	/** Stable identifier used by prerequisites and unlock requests. */
	UPROPERTY(BlueprintReadOnly, Category = "Progression")
	FName SkillId = NAME_None;

	/** Localized player-facing skill name. */
	UPROPERTY(BlueprintReadOnly, Category = "Progression")
	FText DisplayName;

	/** Localized player-facing skill description. */
	UPROPERTY(BlueprintReadOnly, Category = "Progression")
	FText Description;

	/** Number of skill points required to unlock the node. */
	UPROPERTY(BlueprintReadOnly, Category = "Progression")
	int32 Cost = 0;

	/** Skill identifiers that must be unlocked before this node. */
	UPROPERTY(BlueprintReadOnly, Category = "Progression")
	TArray<FName> PrerequisiteIds;

	/** Data-driven vertical coordinate in the skill-tree layout. */
	UPROPERTY(BlueprintReadOnly, Category = "Progression")
	int32 Row = 0;

	/** Data-driven horizontal coordinate in the skill-tree layout. */
	UPROPERTY(BlueprintReadOnly, Category = "Progression")
	int32 Column = 0;

	/** Current unlock availability for the node. */
	UPROPERTY(BlueprintReadOnly, Category = "Progression")
	ESkillNodeState State = ESkillNodeState::Locked;
};

/** @brief Complete progression snapshot required by the frontend. */
USTRUCT(BlueprintType)
struct UIDEMO_API FProgressionData
{
	GENERATED_BODY()

	/** Skill points currently available to the player. */
	UPROPERTY(BlueprintReadOnly, Category = "Progression")
	int32 SkillPoints = 0;

	/** Nodes composing the current skill tree. */
	UPROPERTY(BlueprintReadOnly, Category = "Progression")
	TArray<FSkillNodeData> Nodes;
};

/** @brief Result of an asynchronous progression query. */
USTRUCT(BlueprintType)
struct UIDEMO_API FProgressionResponse
{
	GENERATED_BODY()

	/** Whether the progression request completed successfully. */
	UPROPERTY(BlueprintReadOnly, Category = "Progression")
	bool bRequestSucceeded = false;

	/** Whether the response was served by the local backend cache. */
	UPROPERTY(BlueprintReadOnly, Category = "Progression")
	bool bFromCache = false;

	/** Progression payload returned on success. */
	UPROPERTY(BlueprintReadOnly, Category = "Progression")
	FProgressionData Progression;

	/** Failure details returned when the request did not succeed. */
	UPROPERTY(BlueprintReadOnly, Category = "Progression")
	FBackendError Error;
};

/** @brief Result of an asynchronous skill-unlock mutation. */
USTRUCT(BlueprintType)
struct UIDEMO_API FUnlockSkillResponse
{
	GENERATED_BODY()

	/** Whether the unlock mutation completed successfully. */
	UPROPERTY(BlueprintReadOnly, Category = "Progression")
	bool bRequestSucceeded = false;

	/** Identifier of the node unlocked on success. */
	UPROPERTY(BlueprintReadOnly, Category = "Progression")
	FName UnlockedSkillId = NAME_None;

	/** Skill-point balance after a successful unlock. */
	UPROPERTY(BlueprintReadOnly, Category = "Progression")
	int32 RemainingSkillPoints = 0;

	/** Failure details returned when the mutation did not succeed. */
	UPROPERTY(BlueprintReadOnly, Category = "Progression")
	FBackendError Error;
};
