#pragma once

#include "CoreMinimal.h"
#include "Backend/BackendServiceSubsystem.h"
#include "MVVMViewModelBase.h"
#include "UI/ViewModels/ViewModelTypes.h"
#include "ProgressionViewModel.generated.h"

class UFrontendSessionSubsystem;
class USkillNodeViewModel;

/**
 * @brief Screen-level presentation model for skill-tree progression.
 *
 * The ViewModel owns asynchronous request state, selection, and node
 * ViewModels while the Widget Blueprint owns data-driven node placement.
 */
UCLASS(BlueprintType)
class UIDEMO_API UProgressionViewModel final : public UMVVMViewModelBase
{
	GENERATED_BODY()

public:
	/**
	 * @brief Resolves backend and shared-session dependencies.
	 * @param WorldContextObject Object used to locate the owning game instance.
	 * @return True when every required subsystem was resolved successfully.
	 */
	UFUNCTION(BlueprintCallable, Category = "UIDemo|Progression")
	bool Initialize(UObject* WorldContextObject);

	/**
	 * @brief Starts an asynchronous progression query.
	 * @param bForceRefresh True to bypass a valid cached response.
	 */
	UFUNCTION(BlueprintCallable, Category = "UIDemo|Progression")
	void LoadProgression(bool bForceRefresh = false);

	/** @brief Retries a failed progression load while bypassing cache. */
	UFUNCTION(BlueprintCallable, Category = "UIDemo|Progression")
	void Retry();

	/**
	 * @brief Changes the node selected by the progression screen.
	 * @param Node Node ViewModel selected by the tree, or nullptr to clear selection.
	 */
	UFUNCTION(BlueprintCallable, Category = "UIDemo|Progression")
	void SelectNode(USkillNodeViewModel* Node);

	/** @brief Starts an unlock mutation for the selected node when permitted. */
	UFUNCTION(BlueprintCallable, Category = "UIDemo|Progression")
	void UnlockSelectedNode();

	/** @brief Cancels pending work and returns to the best available stable state. */
	UFUNCTION(BlueprintCallable, Category = "UIDemo|Progression")
	void Cancel();

protected:
	/** @brief Cancels pending requests before the ViewModel is destroyed. */
	virtual void BeginDestroy() override;

private:
	/**
	 * @brief Processes a progression response that belongs to the active attempt.
	 * @param Response Response returned by the backend facade.
	 * @param AttemptId Attempt identifier captured when the request started.
	 */
	void HandleProgressionResponse(const FProgressionResponse& Response, int32 AttemptId);
	/**
	 * @brief Processes an unlock response that belongs to the active attempt.
	 * @param Response Response returned by the backend facade.
	 * @param AttemptId Attempt identifier captured when the request started.
	 */
	void HandleUnlockResponse(const FUnlockSkillResponse& Response, int32 AttemptId);
	/**
	 * @brief Replaces node ViewModels from a backend progression snapshot.
	 * @param Data Progression data returned by a successful request.
	 */
	void RebuildNodes(const FProgressionData& Data);
	/**
	 * @brief Updates the async screen state and derived busy state.
	 * @param NewState New presentation state.
	 */
	void SetState(EAsyncViewState NewState);
	/**
	 * @brief Updates localized error copy and broadcasts when it changes.
	 * @param NewError New player-facing error text.
	 */
	void SetError(const FText& NewError);
	/** @brief Recomputes whether the selected node can currently be unlocked. */
	void UpdateCanUnlock();
	/** @brief Cancels and clears load and unlock request handles. */
	void CancelRequests();

private:
	/** Backend facade used for progression queries and mutations. */
	UPROPERTY(Transient)
	TObjectPtr<UBackendServiceSubsystem> BackendSubsystem;
	/** Shared session updated after a successful unlock mutation. */
	UPROPERTY(Transient)
	TObjectPtr<UFrontendSessionSubsystem> SessionSubsystem;

	/** Current asynchronous screen state. */
	UPROPERTY(BlueprintReadOnly, FieldNotify, Category = "UIDemo|Progression", meta = (AllowPrivateAccess = "true"))
	EAsyncViewState State = EAsyncViewState::Idle;
	/** Node-level ViewModels consumed by the data-driven skill tree. */
	UPROPERTY(BlueprintReadOnly, FieldNotify, Category = "UIDemo|Progression", meta = (AllowPrivateAccess = "true"))
	TArray<TObjectPtr<USkillNodeViewModel>> Nodes;
	/** Node currently selected by the progression screen. */
	UPROPERTY(BlueprintReadOnly, FieldNotify, Category = "UIDemo|Progression", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USkillNodeViewModel> SelectedNode;
	/** Skill-point balance returned by the latest successful response. */
	UPROPERTY(BlueprintReadOnly, FieldNotify, Category = "UIDemo|Progression", meta = (AllowPrivateAccess = "true"))
	int32 SkillPoints = 0;
	/** Localized load or mutation error displayed by the screen. */
	UPROPERTY(BlueprintReadOnly, FieldNotify, Category = "UIDemo|Progression", meta = (AllowPrivateAccess = "true"))
	FText ErrorText;
	/** Whether a load or unlock mutation currently blocks input. */
	UPROPERTY(BlueprintReadOnly, FieldNotify, Category = "UIDemo|Progression", meta = (AllowPrivateAccess = "true"))
	bool bIsBusy = false;
	/** Whether the latest successful load was served from cache. */
	UPROPERTY(BlueprintReadOnly, FieldNotify, Category = "UIDemo|Progression", meta = (AllowPrivateAccess = "true"))
	bool bFromCache = false;
	/** Whether the selected node can currently be unlocked. */
	UPROPERTY(BlueprintReadOnly, FieldNotify, Category = "UIDemo|Progression", meta = (AllowPrivateAccess = "true"))
	bool bCanUnlock = false;

	/** Handle for the active progression query. */
	FBackendRequestHandle LoadRequest;
	/** Handle for the active skill-unlock mutation. */
	FBackendRequestHandle UnlockRequest;
	/** Identifier used to reject stale asynchronous callbacks. */
	int32 ActiveAttemptId = 0;
};
