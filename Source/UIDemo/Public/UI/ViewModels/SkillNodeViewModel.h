#pragma once

#include "CoreMinimal.h"
#include "Backend/ProgressionTypes.h"
#include "MVVMViewModelBase.h"
#include "SkillNodeViewModel.generated.h"

UCLASS(BlueprintType)
class UIDEMO_API USkillNodeViewModel final : public UMVVMViewModelBase
{
	GENERATED_BODY()

public:
	void Initialize(const FSkillNodeData& Node);
	void ApplyState(ESkillNodeState NewState);
	void SetSelected(bool bNewSelected);

	FName GetSkillId() const { return SkillId; }
	int32 GetCost() const { return Cost; }
	ESkillNodeState GetState() const { return State; }

private:
	UPROPERTY(BlueprintReadOnly, FieldNotify, Category = "UIDemo|Progression", meta = (AllowPrivateAccess = "true"))
	FName SkillId = NAME_None;
	UPROPERTY(BlueprintReadOnly, FieldNotify, Category = "UIDemo|Progression", meta = (AllowPrivateAccess = "true"))
	FText DisplayName;
	UPROPERTY(BlueprintReadOnly, FieldNotify, Category = "UIDemo|Progression", meta = (AllowPrivateAccess = "true"))
	FText Description;
	UPROPERTY(BlueprintReadOnly, FieldNotify, Category = "UIDemo|Progression", meta = (AllowPrivateAccess = "true"))
	int32 Cost = 0;
	UPROPERTY(BlueprintReadOnly, FieldNotify, Category = "UIDemo|Progression", meta = (AllowPrivateAccess = "true"))
	TArray<FName> PrerequisiteIds;
	UPROPERTY(BlueprintReadOnly, FieldNotify, Category = "UIDemo|Progression", meta = (AllowPrivateAccess = "true"))
	int32 Row = 0;
	UPROPERTY(BlueprintReadOnly, FieldNotify, Category = "UIDemo|Progression", meta = (AllowPrivateAccess = "true"))
	int32 Column = 0;
	UPROPERTY(BlueprintReadOnly, FieldNotify, Category = "UIDemo|Progression", meta = (AllowPrivateAccess = "true"))
	ESkillNodeState State = ESkillNodeState::Locked;
	UPROPERTY(BlueprintReadOnly, FieldNotify, Category = "UIDemo|Progression", meta = (AllowPrivateAccess = "true"))
	bool bSelected = false;
};
