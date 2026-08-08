#pragma once

#include "CoreMinimal.h"
#include "Backend/BackendTypes.h"
#include "MVVMViewModelBase.h"
#include "BootstrapViewModel.generated.h"

class UBackendServiceSubsystem;
class UFrontendSessionSubsystem;

/** @brief Presentation states for the initial account bootstrap flow. */
UENUM(BlueprintType)
enum class EBootstrapState : uint8
{
	/** Bootstrap has not started. */
	Idle,
	/** Required backend services are being validated. */
	CheckingServices,
	/** Profile and remote configuration are loading in parallel. */
	LoadingAccount,
	/** All required frontend session data is available. */
	Ready,
	/** Bootstrap failed and may offer a retry action. */
	Error
};

/** Broadcast after bootstrap initializes the shared frontend session. */
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnBootstrapCompleted);

/**
 * @brief Presentation state and orchestration for the initial bootstrap screen.
 *
 * This ViewModel does not own or reference widgets. It coordinates backend
 * requests and exposes event-driven presentation fields through MVVM.
 */
UCLASS(BlueprintType)
class UIDEMO_API UBootstrapViewModel final : public UMVVMViewModelBase
{
	GENERATED_BODY()

public:
	/**
	 * @brief Resolves the required subsystems using the supplied world context.
	 *
	 * Call this once before StartBootstrap.
	 * @param WorldContextObject Object used to locate the owning game instance.
	 * @return True when every required subsystem was resolved successfully.
	 */
	UFUNCTION(BlueprintCallable, Category = "UIDemo|Bootstrap")
	bool Initialize(UObject* WorldContextObject);

	/**
	 * @brief Starts a bootstrap attempt.
	 *
	 * Repeated calls while loading are ignored.
	 */
	UFUNCTION(BlueprintCallable, Category = "UIDemo|Bootstrap")
	void StartBootstrap();

	/**
	 * @brief Starts a new attempt after an error.
	 */
	UFUNCTION(BlueprintCallable, Category = "UIDemo|Bootstrap")
	void Retry();

	/**
	 * @brief Cancels pending requests and returns to Idle.
	 */
	UFUNCTION(BlueprintCallable, Category = "UIDemo|Bootstrap")
	void Cancel();

	/** Event emitted after profile and configuration initialize the session. */
	UPROPERTY(BlueprintAssignable, Category = "UIDemo|Bootstrap")
	FOnBootstrapCompleted OnBootstrapCompleted;

protected:
	/** @brief Cancels pending requests before the ViewModel is destroyed. */
	virtual void BeginDestroy() override;

private:
	/** @brief Resets attempt state and begins the service-status request. */
	void BeginBootstrapAttempt();

	/** @brief Starts profile and remote-configuration requests in parallel. */
	void StartAccountRequests();

	/**
	 * @brief Processes a completed service-status request.
	 * @param Response Service-status response returned by the backend facade.
	 * @param AttemptId Attempt identifier captured when the request started.
	 */
	void HandleServiceStatusResponse(
		const FBackendServiceStatusResponse& Response,
		int32 AttemptId);

	/**
	 * @brief Processes a completed remote-configuration request.
	 * @param Response Remote-configuration response returned by the backend facade.
	 * @param AttemptId Attempt identifier captured when the request started.
	 */
	void HandleRemoteConfigResponse(
		const FRemoteConfigResponse& Response,
		int32 AttemptId);

	/**
	 * @brief Processes a completed player-profile request.
	 * @param Response Player-profile response returned by the backend facade.
	 * @param AttemptId Attempt identifier captured when the request started.
	 */
	void HandlePlayerProfileResponse(
		const FPlayerProfileResponse& Response,
		int32 AttemptId);

	/**
	 * @brief Updates aggregate progress after one account request completes.
	 * @param AttemptId Attempt identifier captured when the request started.
	 */
	void HandleAccountRequestCompleted(int32 AttemptId);

	/**
	 * @brief Commits loaded data to the session and broadcasts completion.
	 * @param AttemptId Attempt identifier captured when the request started.
	 */
	void CompleteAttempt(int32 AttemptId);

	/**
	 * @brief Converts a backend failure into the terminal bootstrap error state.
	 * @param Error Machine-readable backend error.
	 * @param AttemptId Attempt identifier captured when the request started.
	 */
	void FailAttempt(
		const FBackendError& Error,
		int32 AttemptId);

	/** @brief Cancels and invalidates every request owned by the active attempt. */
	void CancelActiveRequests();

	/** @brief Clears profile and configuration payloads from presentation state. */
	void ResetLoadedData();

	/**
	 * @brief Updates State and its derived loading and retry fields.
	 * @param NewState New bootstrap presentation state.
	 */
	void SetState(EBootstrapState NewState);
	/**
	 * @brief Updates localized status copy and broadcasts when it changes.
	 * @param NewStatusText New player-facing status text.
	 */
	void SetStatusText(const FText& NewStatusText);
	/**
	 * @brief Updates localized error copy and broadcasts when it changes.
	 * @param NewErrorText New player-facing error text.
	 */
	void SetErrorText(const FText& NewErrorText);
	/**
	 * @brief Clamps and updates normalized bootstrap progress.
	 * @param NewProgress Requested normalized progress value.
	 */
	void SetProgress(float NewProgress);

	/**
	 * @brief Tests whether a callback belongs to the active bootstrap attempt.
	 * @param AttemptId Attempt identifier captured by a callback.
	 * @return True when the callback may update presentation state.
	 */
	bool IsCurrentAttempt(int32 AttemptId) const;

private:
	/** Backend facade used to schedule bootstrap requests. */
	UPROPERTY(Transient)
	TObjectPtr<UBackendServiceSubsystem> BackendSubsystem;

	/** Shared session initialized after all bootstrap requests succeed. */
	UPROPERTY(Transient)
	TObjectPtr<UFrontendSessionSubsystem> SessionSubsystem;

	/** Current phase of the bootstrap flow. */
	UPROPERTY(
		BlueprintReadOnly,
		FieldNotify,
		Category = "UIDemo|Bootstrap",
		meta = (AllowPrivateAccess = "true"))
	EBootstrapState State = EBootstrapState::Idle;

	/** Localized progress message displayed by the bootstrap screen. */
	UPROPERTY(
		BlueprintReadOnly,
		FieldNotify,
		Category = "UIDemo|Bootstrap",
		meta = (AllowPrivateAccess = "true"))
	FText StatusText;

	/** Localized player-facing error message. */
	UPROPERTY(
		BlueprintReadOnly,
		FieldNotify,
		Category = "UIDemo|Bootstrap",
		meta = (AllowPrivateAccess = "true"))
	FText ErrorText;

	/** Normalized aggregate progress in the range [0, 1]. */
	UPROPERTY(
		BlueprintReadOnly,
		FieldNotify,
		Category = "UIDemo|Bootstrap",
		meta = (AllowPrivateAccess = "true"))
	float Progress = 0.0f;

	/** Whether the ViewModel is waiting for one or more backend requests. */
	UPROPERTY(
		BlueprintReadOnly,
		FieldNotify,
		Category = "UIDemo|Bootstrap",
		meta = (AllowPrivateAccess = "true"))
	bool bIsLoading = false;

	/** Whether the current error state permits a retry action. */
	UPROPERTY(
		BlueprintReadOnly,
		FieldNotify,
		Category = "UIDemo|Bootstrap",
		meta = (AllowPrivateAccess = "true"))
	bool bCanRetry = false;

	/** Player profile loaded by the active bootstrap attempt. */
	UPROPERTY(
		BlueprintReadOnly,
		FieldNotify,
		Category = "UIDemo|Bootstrap",
		meta = (AllowPrivateAccess = "true"))
	FPlayerProfile PlayerProfile;

	/** Remote configuration loaded by the active bootstrap attempt. */
	UPROPERTY(
		BlueprintReadOnly,
		FieldNotify,
		Category = "UIDemo|Bootstrap",
		meta = (AllowPrivateAccess = "true"))
	FRemoteConfig RemoteConfig;

	/** Handle for the active service-status request. */
	FBackendRequestHandle ServiceStatusRequest;
	/** Handle for the active remote-configuration request. */
	FBackendRequestHandle RemoteConfigRequest;
	/** Handle for the active player-profile request. */
	FBackendRequestHandle PlayerProfileRequest;

	/** Identifier used to reject stale asynchronous callbacks. */
	int32 ActiveAttemptId = 0;

	/** Whether the active attempt has received a successful profile response. */
	bool bPlayerProfileCompleted = false;
	/** Whether the active attempt has received a successful config response. */
	bool bRemoteConfigCompleted = false;
};
