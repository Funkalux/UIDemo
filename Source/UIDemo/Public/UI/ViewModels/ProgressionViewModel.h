#pragma once

#include "CoreMinimal.h"
#include "Backend/BackendServiceSubsystem.h"
#include "MVVMViewModelBase.h"
#include "UI/ViewModels/ViewModelTypes.h"
#include "ProgressionViewModel.generated.h"

class UFrontendSessionSubsystem;
class USkillNodeViewModel;

UCLASS(BlueprintType)
class UIDEMO_API UProgressionViewModel final : public UMVVMViewModelBase
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "UIDemo|Progression")
	bool Initialize(UObject* WorldContextObject);
	UFUNCTION(BlueprintCallable, Category = "UIDemo|Progression")
	void LoadProgression(bool bForceRefresh = false);
	UFUNCTION(BlueprintCallable, Category = "UIDemo|Progression")
	void Retry();
	UFUNCTION(BlueprintCallable, Category = "UIDemo|Progression")
	void SelectNode(USkillNodeViewModel* Node);
	UFUNCTION(BlueprintCallable, Category = "UIDemo|Progression")
	void UnlockSelectedNode();
	UFUNCTION(BlueprintCallable, Category = "UIDemo|Progression")
	void Cancel();

protected:
	virtual void BeginDestroy() override;

private:
	void HandleProgressionResponse(const FProgressionResponse& Response, int32 AttemptId);
	void HandleUnlockResponse(const FUnlockSkillResponse& Response, int32 AttemptId);
	void RebuildNodes(const FProgressionData& Data);
	void SetState(EAsyncViewState NewState);
	void SetError(const FText& NewError);
	void UpdateCanUnlock();
	void CancelRequests();

private:
	UPROPERTY(Transient)
	TObjectPtr<UBackendServiceSubsystem> BackendSubsystem;
	UPROPERTY(Transient)
	TObjectPtr<UFrontendSessionSubsystem> SessionSubsystem;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Category = "UIDemo|Progression", meta = (AllowPrivateAccess = "true"))
	EAsyncViewState State = EAsyncViewState::Idle;
	UPROPERTY(BlueprintReadOnly, FieldNotify, Category = "UIDemo|Progression", meta = (AllowPrivateAccess = "true"))
	TArray<TObjectPtr<USkillNodeViewModel>> Nodes;
	UPROPERTY(BlueprintReadOnly, FieldNotify, Category = "UIDemo|Progression", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USkillNodeViewModel> SelectedNode;
	UPROPERTY(BlueprintReadOnly, FieldNotify, Category = "UIDemo|Progression", meta = (AllowPrivateAccess = "true"))
	int32 SkillPoints = 0;
	UPROPERTY(BlueprintReadOnly, FieldNotify, Category = "UIDemo|Progression", meta = (AllowPrivateAccess = "true"))
	FText ErrorText;
	UPROPERTY(BlueprintReadOnly, FieldNotify, Category = "UIDemo|Progression", meta = (AllowPrivateAccess = "true"))
	bool bIsBusy = false;
	UPROPERTY(BlueprintReadOnly, FieldNotify, Category = "UIDemo|Progression", meta = (AllowPrivateAccess = "true"))
	bool bFromCache = false;
	UPROPERTY(BlueprintReadOnly, FieldNotify, Category = "UIDemo|Progression", meta = (AllowPrivateAccess = "true"))
	bool bCanUnlock = false;

	FBackendRequestHandle LoadRequest;
	FBackendRequestHandle UnlockRequest;
	int32 ActiveAttemptId = 0;
};
