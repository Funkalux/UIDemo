#pragma once

#include "CoreMinimal.h"
#include "Backend/BackendTypes.h"
#include "MVVMViewModelBase.h"
#include "BackendDebugViewModel.generated.h"

class UBackendServiceSubsystem;

/**
 * @brief Presentation model for deterministic backend simulation controls.
 *
 * The ViewModel exposes simulator settings to a development-only modal without
 * allowing the widget to depend directly on the backend subsystem.
 */
UCLASS(BlueprintType)
class UIDEMO_API UBackendDebugViewModel final : public UMVVMViewModelBase
{
	GENERATED_BODY()

public:
	/**
	 * @brief Resolves the backend subsystem from a world context.
	 * @param WorldContextObject Object used to locate the owning game instance.
	 * @return True when the backend subsystem was resolved successfully.
	 */
	UFUNCTION(BlueprintCallable, Category = "UIDemo|Debug")
	bool Initialize(UObject* WorldContextObject);

	/**
	 * @brief Applies the scenario used by future simulated requests.
	 * @param NewScenario Deterministic scenario selected by the developer.
	 */
	UFUNCTION(BlueprintCallable, Category = "UIDemo|Debug")
	void ApplyScenario(EBackendSimulationScenario NewScenario);

	/**
	 * @brief Applies normal simulated request latency.
	 * @param NewLatencySeconds Requested latency in seconds.
	 */
	UFUNCTION(BlueprintCallable, Category = "UIDemo|Debug")
	void ApplyLatency(float NewLatencySeconds);

	/** @brief Restores the backend simulator's deterministic default settings. */
	UFUNCTION(BlueprintCallable, Category = "UIDemo|Debug")
	void ResetSimulation();

	/** @brief Invalidates all cached feature responses. */
	UFUNCTION(BlueprintCallable, Category = "UIDemo|Debug")
	void ClearCache();

	/** @brief Refreshes every exposed field from the backend subsystem. */
	UFUNCTION(BlueprintCallable, Category = "UIDemo|Debug")
	void Refresh();

private:
	/** Backend facade controlled by this presentation model. */
	UPROPERTY(Transient)
	TObjectPtr<UBackendServiceSubsystem> BackendSubsystem;

	/** Scenario currently configured for future requests. */
	UPROPERTY(BlueprintReadOnly, FieldNotify, Category = "UIDemo|Debug", meta = (AllowPrivateAccess = "true"))
	EBackendSimulationScenario Scenario = EBackendSimulationScenario::Success;

	/** Normal simulated request latency in seconds. */
	UPROPERTY(BlueprintReadOnly, FieldNotify, Category = "UIDemo|Debug", meta = (AllowPrivateAccess = "true"))
	float LatencySeconds = 0.75f;

	/** Number of simulated requests currently awaiting completion. */
	UPROPERTY(BlueprintReadOnly, FieldNotify, Category = "UIDemo|Debug", meta = (AllowPrivateAccess = "true"))
	int32 ActiveRequestCount = 0;
};
