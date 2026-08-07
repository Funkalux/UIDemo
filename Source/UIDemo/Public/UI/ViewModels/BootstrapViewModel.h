#pragma once

#include "CoreMinimal.h"
#include "Backend/BackendTypes.h"
#include "MVVMViewModelBase.h"
#include "BootstrapViewModel.generated.h"

class UBackendServiceSubsystem;
class UFrontendSessionSubsystem;

UENUM(BlueprintType)
enum class EBootstrapState : uint8
{
	Idle,
	CheckingServices,
	LoadingAccount,
	Ready,
	Error
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnBootstrapCompleted);

/**
 * Presentation state and orchestration for the initial bootstrap screen.
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
	 * Resolves the backend subsystem using the supplied world context.
	 *
	 * Call this once before StartBootstrap.
	 */
	UFUNCTION(BlueprintCallable, Category = "UIDemo|Bootstrap")
	bool Initialize(UObject* WorldContextObject);

	/**
	 * Starts a bootstrap attempt.
	 *
	 * Repeated calls while loading are ignored.
	 */
	UFUNCTION(BlueprintCallable, Category = "UIDemo|Bootstrap")
	void StartBootstrap();

	/**
	 * Starts a new attempt after an error.
	 */
	UFUNCTION(BlueprintCallable, Category = "UIDemo|Bootstrap")
	void Retry();

	/**
	 * Cancels pending requests and returns to Idle.
	 */
	UFUNCTION(BlueprintCallable, Category = "UIDemo|Bootstrap")
	void Cancel();

	UPROPERTY(BlueprintAssignable, Category = "UIDemo|Bootstrap")
	FOnBootstrapCompleted OnBootstrapCompleted;

protected:
	virtual void BeginDestroy() override;

private:
	void BeginBootstrapAttempt();
	void StartAccountRequests();

	void HandleServiceStatusResponse(
		const FBackendServiceStatusResponse& Response,
		int32 AttemptId);

	void HandleRemoteConfigResponse(
		const FRemoteConfigResponse& Response,
		int32 AttemptId);

	void HandlePlayerProfileResponse(
		const FPlayerProfileResponse& Response,
		int32 AttemptId);

	void HandleAccountRequestCompleted(int32 AttemptId);
	void CompleteAttempt(int32 AttemptId);

	void FailAttempt(
		const FBackendError& Error,
		int32 AttemptId);

	void CancelActiveRequests();
	void ResetLoadedData();

	void SetState(EBootstrapState NewState);
	void SetStatusText(const FText& NewStatusText);
	void SetErrorText(const FText& NewErrorText);
	void SetProgress(float NewProgress);

	bool IsCurrentAttempt(int32 AttemptId) const;

private:
	UPROPERTY(Transient)
	TObjectPtr<UBackendServiceSubsystem> BackendSubsystem;

	UPROPERTY(Transient)
	TObjectPtr<UFrontendSessionSubsystem> SessionSubsystem;

	UPROPERTY(
		BlueprintReadOnly,
		FieldNotify,
		Category = "UIDemo|Bootstrap",
		meta = (AllowPrivateAccess = "true"))
	EBootstrapState State = EBootstrapState::Idle;

	UPROPERTY(
		BlueprintReadOnly,
		FieldNotify,
		Category = "UIDemo|Bootstrap",
		meta = (AllowPrivateAccess = "true"))
	FText StatusText;

	UPROPERTY(
		BlueprintReadOnly,
		FieldNotify,
		Category = "UIDemo|Bootstrap",
		meta = (AllowPrivateAccess = "true"))
	FText ErrorText;

	UPROPERTY(
		BlueprintReadOnly,
		FieldNotify,
		Category = "UIDemo|Bootstrap",
		meta = (AllowPrivateAccess = "true"))
	float Progress = 0.0f;

	UPROPERTY(
		BlueprintReadOnly,
		FieldNotify,
		Category = "UIDemo|Bootstrap",
		meta = (AllowPrivateAccess = "true"))
	bool bIsLoading = false;

	UPROPERTY(
		BlueprintReadOnly,
		FieldNotify,
		Category = "UIDemo|Bootstrap",
		meta = (AllowPrivateAccess = "true"))
	bool bCanRetry = false;

	UPROPERTY(
		BlueprintReadOnly,
		FieldNotify,
		Category = "UIDemo|Bootstrap",
		meta = (AllowPrivateAccess = "true"))
	FPlayerProfile PlayerProfile;

	UPROPERTY(
		BlueprintReadOnly,
		FieldNotify,
		Category = "UIDemo|Bootstrap",
		meta = (AllowPrivateAccess = "true"))
	FRemoteConfig RemoteConfig;

	FBackendRequestHandle ServiceStatusRequest;
	FBackendRequestHandle RemoteConfigRequest;
	FBackendRequestHandle PlayerProfileRequest;

	int32 ActiveAttemptId = 0;

	bool bPlayerProfileCompleted = false;
	bool bRemoteConfigCompleted = false;
};
