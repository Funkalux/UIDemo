#pragma once

#include "CoreMinimal.h"
#include "Backend/ProgressionTypes.h"
#include "MVVMViewModelBase.h"
#include "SkillNodeViewModel.generated.h"

/**
 * @brief Node-level presentation model for the data-driven skill tree.
 *
 * Each instance exposes immutable node layout and copy plus mutable state and
 * selection fields consumed by its corresponding CommonButton widget.
 */
UCLASS(BlueprintType)
class UIDEMO_API USkillNodeViewModel final : public UMVVMViewModelBase
{
	GENERATED_BODY()

public:
	/**
	 * @brief Initializes the ViewModel from one progression node payload.
	 * @param Node Source node copied into presentation fields.
	 */
	void Initialize(const FSkillNodeData& Node);

	/**
	 * @brief Applies a new unlock state and broadcasts when it changes.
	 * @param NewState New availability state for the node.
	 */
	void ApplyState(ESkillNodeState NewState);

	/**
	 * @brief Updates the selection state and broadcasts when it changes.
	 * @param bNewSelected New selection state.
	 */
	void SetSelected(bool bNewSelected);

	/** @return Stable identifier of the represented skill. */
	FName GetSkillId() const { return SkillId; }
	/** @return Skill-point cost required to unlock the node. */
	int32 GetCost() const { return Cost; }
	/** @return Current availability state of the node. */
	ESkillNodeState GetState() const { return State; }

private:
	/** Stable identifier used by prerequisites and unlock requests. */
	UPROPERTY(BlueprintReadOnly, FieldNotify, Category = "UIDemo|Progression", meta = (AllowPrivateAccess = "true"))
	FName SkillId = NAME_None;

	/** Localized player-facing skill name. */
	UPROPERTY(BlueprintReadOnly, FieldNotify, Category = "UIDemo|Progression", meta = (AllowPrivateAccess = "true"))
	FText DisplayName;

	/** Localized player-facing skill description. */
	UPROPERTY(BlueprintReadOnly, FieldNotify, Category = "UIDemo|Progression", meta = (AllowPrivateAccess = "true"))
	FText Description;

	/** Skill-point cost required to unlock the node. */
	UPROPERTY(BlueprintReadOnly, FieldNotify, Category = "UIDemo|Progression", meta = (AllowPrivateAccess = "true"))
	int32 Cost = 0;

	/** Stable identifiers of nodes that must be unlocked first. */
	UPROPERTY(BlueprintReadOnly, FieldNotify, Category = "UIDemo|Progression", meta = (AllowPrivateAccess = "true"))
	TArray<FName> PrerequisiteIds;

	/** Data-driven vertical coordinate in the skill-tree layout. */
	UPROPERTY(BlueprintReadOnly, FieldNotify, Category = "UIDemo|Progression", meta = (AllowPrivateAccess = "true"))
	int32 Row = 0;

	/** Data-driven horizontal coordinate in the skill-tree layout. */
	UPROPERTY(BlueprintReadOnly, FieldNotify, Category = "UIDemo|Progression", meta = (AllowPrivateAccess = "true"))
	int32 Column = 0;

	/** Current unlock availability for the node. */
	UPROPERTY(BlueprintReadOnly, FieldNotify, Category = "UIDemo|Progression", meta = (AllowPrivateAccess = "true"))
	ESkillNodeState State = ESkillNodeState::Locked;

	/** Whether the owning progression screen currently selects this node. */
	UPROPERTY(BlueprintReadOnly, FieldNotify, Category = "UIDemo|Progression", meta = (AllowPrivateAccess = "true"))
	bool bSelected = false;
};
