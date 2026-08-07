#pragma once

#include "CoreMinimal.h"
#include "Backend/BackendTypes.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "TimerManager.h"
#include "BackendServiceSubsystem.generated.h"

DECLARE_DELEGATE_OneParam(
	FOnBackendServiceStatusCompleted,
	const FBackendServiceStatusResponse&);

DECLARE_DELEGATE_OneParam(
	FOnRemoteConfigCompleted,
	const FRemoteConfigResponse&);

DECLARE_DELEGATE_OneParam(
	FOnPlayerProfileCompleted,
	const FPlayerProfileResponse&);

/**
 * Application-facing backend facade.
 *
 * The current implementation is a deterministic simulator. ViewModels interact
 * with this subsystem without needing to know whether the data comes from
 * timers, HTTP, platform services, or a local cache.
 */
UCLASS()
class UIDEMO_API UBackendServiceSubsystem final
	: public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	// USubsystem
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	// End USubsystem

	/**
	 * Requests the current backend service status.
	 */
	FBackendRequestHandle CheckServiceStatus(
		FOnBackendServiceStatusCompleted Completion);

	/**
	 * Requests frontend feature configuration.
	 */
	FBackendRequestHandle FetchRemoteConfig(
		FOnRemoteConfigCompleted Completion);

	/**
	 * Requests the local player's profile.
	 */
	FBackendRequestHandle FetchPlayerProfile(
		FOnPlayerProfileCompleted Completion);

	/**
	 * Cancels one pending request.
	 *
	 * Cancellation does not execute the completion delegate.
	 */
	bool CancelRequest(const FBackendRequestHandle& RequestHandle);

	/**
	 * Cancels every pending request owned by this subsystem.
	 */
	UFUNCTION(BlueprintCallable, Category = "UIDemo|Backend")
	void CancelAllRequests();

	/**
	 * Selects the deterministic response scenario used by future requests.
	 *
	 * Requests already in flight preserve the scenario with which they began.
	 */
	UFUNCTION(BlueprintCallable, Category = "UIDemo|Backend|Simulation")
	void SetSimulationScenario(EBackendSimulationScenario NewScenario);

	UFUNCTION(BlueprintPure, Category = "UIDemo|Backend|Simulation")
	EBackendSimulationScenario GetSimulationScenario() const;

	/**
	 * Sets normal request latency in seconds.
	 */
	UFUNCTION(BlueprintCallable, Category = "UIDemo|Backend|Simulation")
	void SetSimulatedLatencySeconds(float NewLatencySeconds);

	UFUNCTION(BlueprintPure, Category = "UIDemo|Backend|Simulation")
	float GetSimulatedLatencySeconds() const;

	/**
	 * Sets the delay used before a simulated timeout is returned.
	 */
	UFUNCTION(BlueprintCallable, Category = "UIDemo|Backend|Simulation")
	void SetTimeoutDelaySeconds(float NewTimeoutDelaySeconds);

	UFUNCTION(BlueprintPure, Category = "UIDemo|Backend|Simulation")
	float GetTimeoutDelaySeconds() const;

	/**
	 * Restores deterministic default settings.
	 */
	UFUNCTION(BlueprintCallable, Category = "UIDemo|Backend|Simulation")
	void ResetSimulationSettings();

	UFUNCTION(BlueprintPure, Category = "UIDemo|Backend|Simulation")
	int32 GetActiveRequestCount() const;

private:
	FBackendRequestHandle ScheduleRequest(
		float DelaySeconds,
		TFunction<void()>&& Completion);

	float GetDelayForScenario(
		EBackendSimulationScenario Scenario) const;

	FBackendServiceStatusResponse BuildServiceStatusResponse(
		EBackendSimulationScenario Scenario) const;

	FRemoteConfigResponse BuildRemoteConfigResponse(
		EBackendSimulationScenario Scenario) const;

	FPlayerProfileResponse BuildPlayerProfileResponse(
		EBackendSimulationScenario Scenario) const;

private:
	UPROPERTY(Transient)
	EBackendSimulationScenario SimulationScenario =
		EBackendSimulationScenario::Success;

	UPROPERTY(Transient)
	float SimulatedLatencySeconds = 0.75f;

	UPROPERTY(Transient)
	float TimeoutDelaySeconds = 4.0f;

	int32 NextRequestId = 1;

	TMap<int32, FTimerHandle> ActiveRequests;
};