#pragma once

#include "CoreMinimal.h"
#include "Backend/BackendServiceSubsystem.h"
#include "MVVMViewModelBase.h"
#include "UI/ViewModels/ViewModelTypes.h"
#include "MatchmakingViewModel.generated.h"

UCLASS(BlueprintType)
class UIDEMO_API UMatchmakingViewModel final : public UMVVMViewModelBase
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "UIDemo|Matchmaking")
	bool Initialize(UObject* WorldContextObject);
	UFUNCTION(BlueprintCallable, Category = "UIDemo|Matchmaking")
	void StartSearch(FName InPlaylistId);
	UFUNCTION(BlueprintCallable, Category = "UIDemo|Matchmaking")
	void Retry();
	UFUNCTION(BlueprintCallable, Category = "UIDemo|Matchmaking")
	void CancelSearch();

protected:
	virtual void BeginDestroy() override;

private:
	void HandleMatchmakingResponse(const FMatchmakingResponse& Response, int32 AttemptId);
	void SetState(EMatchmakingViewState NewState);
	void SetError(const FText& NewError);

	UPROPERTY(Transient)
	TObjectPtr<UBackendServiceSubsystem> BackendSubsystem;
	UPROPERTY(BlueprintReadOnly, FieldNotify, Category = "UIDemo|Matchmaking", meta = (AllowPrivateAccess = "true"))
	EMatchmakingViewState State = EMatchmakingViewState::Idle;
	UPROPERTY(BlueprintReadOnly, FieldNotify, Category = "UIDemo|Matchmaking", meta = (AllowPrivateAccess = "true"))
	FName PlaylistId = TEXT("standard");
	UPROPERTY(BlueprintReadOnly, FieldNotify, Category = "UIDemo|Matchmaking", meta = (AllowPrivateAccess = "true"))
	FText StatusText;
	UPROPERTY(BlueprintReadOnly, FieldNotify, Category = "UIDemo|Matchmaking", meta = (AllowPrivateAccess = "true"))
	FText ErrorText;
	UPROPERTY(BlueprintReadOnly, FieldNotify, Category = "UIDemo|Matchmaking", meta = (AllowPrivateAccess = "true"))
	FString SessionId;
	UPROPERTY(BlueprintReadOnly, FieldNotify, Category = "UIDemo|Matchmaking", meta = (AllowPrivateAccess = "true"))
	int32 EstimatedWaitSeconds = 0;
	UPROPERTY(BlueprintReadOnly, FieldNotify, Category = "UIDemo|Matchmaking", meta = (AllowPrivateAccess = "true"))
	bool bIsSearching = false;

	FBackendRequestHandle SearchRequest;
	int32 ActiveAttemptId = 0;
};
