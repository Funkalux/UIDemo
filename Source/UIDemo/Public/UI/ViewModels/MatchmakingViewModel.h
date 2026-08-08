#pragma once

#include "CoreMinimal.h"
#include "Backend/BackendServiceSubsystem.h"
#include "MVVMViewModelBase.h"
#include "UI/ViewModels/ViewModelTypes.h"
#include "MatchmakingViewModel.generated.h"

/**
 * @brief Presentation model for the simulated matchmaking flow.
 *
 * The ViewModel exposes a deterministic state machine and owns cancellation of
 * its asynchronous request without referencing the Play screen widget.
 */
UCLASS(BlueprintType)
class UIDEMO_API UMatchmakingViewModel final : public UMVVMViewModelBase
{
	GENERATED_BODY()

public:
	/**
	 * @brief Resolves the backend subsystem from a world context.
	 * @param WorldContextObject Object used to locate the owning game instance.
	 * @return True when the backend subsystem was resolved successfully.
	 */
	UFUNCTION(BlueprintCallable, Category = "UIDemo|Matchmaking")
	bool Initialize(UObject* WorldContextObject);

	/**
	 * @brief Starts matchmaking for the requested playlist.
	 * @param InPlaylistId Playlist identifier; NAME_None falls back to standard.
	 */
	UFUNCTION(BlueprintCallable, Category = "UIDemo|Matchmaking")
	void StartSearch(FName InPlaylistId);

	/** @brief Restarts matchmaking after an error using the previous playlist. */
	UFUNCTION(BlueprintCallable, Category = "UIDemo|Matchmaking")
	void Retry();

	/** @brief Cancels the active search and invalidates its callback. */
	UFUNCTION(BlueprintCallable, Category = "UIDemo|Matchmaking")
	void CancelSearch();

protected:
	/** @brief Cancels pending matchmaking before the ViewModel is destroyed. */
	virtual void BeginDestroy() override;

private:
	/**
	 * @brief Processes a matchmaking response belonging to the active attempt.
	 * @param Response Response returned by the backend facade.
	 * @param AttemptId Attempt identifier captured when the request started.
	 */
	void HandleMatchmakingResponse(const FMatchmakingResponse& Response, int32 AttemptId);
	/**
	 * @brief Updates State and its derived searching flag.
	 * @param NewState New matchmaking presentation state.
	 */
	void SetState(EMatchmakingViewState NewState);
	/**
	 * @brief Updates localized error copy and broadcasts when it changes.
	 * @param NewError New player-facing error text.
	 */
	void SetError(const FText& NewError);

	/** Backend facade used to schedule matchmaking requests. */
	UPROPERTY(Transient)
	TObjectPtr<UBackendServiceSubsystem> BackendSubsystem;
	/** Current phase of the matchmaking flow. */
	UPROPERTY(BlueprintReadOnly, FieldNotify, Category = "UIDemo|Matchmaking", meta = (AllowPrivateAccess = "true"))
	EMatchmakingViewState State = EMatchmakingViewState::Idle;
	/** Playlist associated with the current or most recent search. */
	UPROPERTY(BlueprintReadOnly, FieldNotify, Category = "UIDemo|Matchmaking", meta = (AllowPrivateAccess = "true"))
	FName PlaylistId = TEXT("standard");
	/** Localized status copy for the active state. */
	UPROPERTY(BlueprintReadOnly, FieldNotify, Category = "UIDemo|Matchmaking", meta = (AllowPrivateAccess = "true"))
	FText StatusText;
	/** Localized player-facing matchmaking error. */
	UPROPERTY(BlueprintReadOnly, FieldNotify, Category = "UIDemo|Matchmaking", meta = (AllowPrivateAccess = "true"))
	FText ErrorText;
	/** Session identifier returned after a successful match. */
	UPROPERTY(BlueprintReadOnly, FieldNotify, Category = "UIDemo|Matchmaking", meta = (AllowPrivateAccess = "true"))
	FString SessionId;
	/** Estimated wait duration displayed while searching. */
	UPROPERTY(BlueprintReadOnly, FieldNotify, Category = "UIDemo|Matchmaking", meta = (AllowPrivateAccess = "true"))
	int32 EstimatedWaitSeconds = 0;
	/** Whether a matchmaking request is currently active. */
	UPROPERTY(BlueprintReadOnly, FieldNotify, Category = "UIDemo|Matchmaking", meta = (AllowPrivateAccess = "true"))
	bool bIsSearching = false;

	/** Handle for the active matchmaking request. */
	FBackendRequestHandle SearchRequest;
	/** Identifier used to reject stale asynchronous callbacks. */
	int32 ActiveAttemptId = 0;
};
