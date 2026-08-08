#pragma once

#include "CoreMinimal.h"
#include "Backend/BackendTypes.h"
#include "Backend/InventoryTypes.h"
#include "Backend/MatchmakingTypes.h"
#include "Backend/ProgressionTypes.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "TimerManager.h"
#include "BackendServiceSubsystem.generated.h"

/** Completion delegate for service-status requests. */
DECLARE_DELEGATE_OneParam(
	FOnBackendServiceStatusCompleted,
	const FBackendServiceStatusResponse&);

/** Completion delegate for remote-configuration requests. */
DECLARE_DELEGATE_OneParam(
	FOnRemoteConfigCompleted,
	const FRemoteConfigResponse&);

/** Completion delegate for player-profile requests. */
DECLARE_DELEGATE_OneParam(
	FOnPlayerProfileCompleted,
	const FPlayerProfileResponse&);

/** Completion delegate for inventory queries. */
DECLARE_DELEGATE_OneParam(FOnInventoryCompleted, const FInventoryResponse&);
/** Completion delegate for equip-item mutations. */
DECLARE_DELEGATE_OneParam(FOnEquipItemCompleted, const FEquipItemResponse&);
/** Completion delegate for progression queries. */
DECLARE_DELEGATE_OneParam(FOnProgressionCompleted, const FProgressionResponse&);
/** Completion delegate for skill-unlock mutations. */
DECLARE_DELEGATE_OneParam(FOnUnlockSkillCompleted, const FUnlockSkillResponse&);
/** Completion delegate for matchmaking requests. */
DECLARE_DELEGATE_OneParam(FOnMatchmakingCompleted, const FMatchmakingResponse&);

/**
 * @brief Application-facing asynchronous backend facade.
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
	/**
	 * @brief Initializes simulator settings, mock data, and caches.
	 *
	 * @param Collection Subsystem collection that owns this instance.
	 */
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	/** @brief Cancels outstanding work and releases cached simulator data. */
	virtual void Deinitialize() override;

	/**
	 * @brief Requests the current backend service status.
	 *
	 * @param Completion Delegate executed when the request completes.
	 * @return Handle that can be used to cancel the request.
	 */
	FBackendRequestHandle CheckServiceStatus(
		FOnBackendServiceStatusCompleted Completion);

	/**
	 * @brief Requests frontend feature configuration.
	 *
	 * @param Completion Delegate executed when the request completes.
	 * @return Handle that can be used to cancel the request.
	 */
	FBackendRequestHandle FetchRemoteConfig(
		FOnRemoteConfigCompleted Completion);

	/**
	 * @brief Requests the local player's profile.
	 *
	 * @param Completion Delegate executed when the request completes.
	 * @return Handle that can be used to cancel the request.
	 */
	FBackendRequestHandle FetchPlayerProfile(
		FOnPlayerProfileCompleted Completion);

	/**
	 * @brief Requests the local player's inventory.
	 *
	 * @param bForceRefresh True to bypass any valid cached response.
	 * @param Completion Delegate executed when the request completes.
	 * @return Handle that can be used to cancel the request.
	 */
	FBackendRequestHandle FetchInventory(
		bool bForceRefresh,
		FOnInventoryCompleted Completion);

	/**
	 * @brief Requests that an inventory item become equipped.
	 *
	 * @param ItemId Stable identifier of the item to equip.
	 * @param Completion Delegate executed when the request completes.
	 * @return Handle that can be used to cancel the request.
	 */
	FBackendRequestHandle EquipItem(
		FName ItemId,
		FOnEquipItemCompleted Completion);

	/**
	 * @brief Requests the local player's progression snapshot.
	 *
	 * @param bForceRefresh True to bypass any valid cached response.
	 * @param Completion Delegate executed when the request completes.
	 * @return Handle that can be used to cancel the request.
	 */
	FBackendRequestHandle FetchProgression(
		bool bForceRefresh,
		FOnProgressionCompleted Completion);

	/**
	 * @brief Requests that an available skill node be unlocked.
	 *
	 * @param SkillId Stable identifier of the skill to unlock.
	 * @param Completion Delegate executed when the request completes.
	 * @return Handle that can be used to cancel the request.
	 */
	FBackendRequestHandle UnlockSkill(
		FName SkillId,
		FOnUnlockSkillCompleted Completion);

	/**
	 * @brief Starts a simulated matchmaking request.
	 *
	 * @param PlaylistId Playlist used to create the search ticket.
	 * @param Completion Delegate executed when the request completes.
	 * @return Handle that can be used to cancel the request.
	 */
	FBackendRequestHandle StartMatchmaking(
		FName PlaylistId,
		FOnMatchmakingCompleted Completion);

	/**
	 * @brief Cancels one pending request.
	 *
	 * Cancellation does not execute the completion delegate.
	 * @param RequestHandle Handle returned when the request was scheduled.
	 * @return True when an active request was found and cancelled.
	 */
	bool CancelRequest(const FBackendRequestHandle& RequestHandle);

	/**
	 * @brief Cancels every pending request owned by this subsystem.
	 */
	UFUNCTION(BlueprintCallable, Category = "UIDemo|Backend")
	void CancelAllRequests();

	/**
	 * @brief Selects the deterministic response scenario used by future requests.
	 *
	 * Requests already in flight preserve the scenario with which they began.
	 * @param NewScenario Scenario captured by subsequently scheduled requests.
	 */
	UFUNCTION(BlueprintCallable, Category = "UIDemo|Backend|Simulation")
	void SetSimulationScenario(EBackendSimulationScenario NewScenario);

	/**
	 * @brief Returns the active deterministic simulation scenario.
	 *
	 * @return Scenario used by future requests.
	 */
	UFUNCTION(BlueprintPure, Category = "UIDemo|Backend|Simulation")
	EBackendSimulationScenario GetSimulationScenario() const;

	/**
	 * @brief Sets normal request latency in seconds.
	 *
	 * @param NewLatencySeconds Requested latency, clamped by the subsystem.
	 */
	UFUNCTION(BlueprintCallable, Category = "UIDemo|Backend|Simulation")
	void SetSimulatedLatencySeconds(float NewLatencySeconds);

	/**
	 * @brief Returns the configured normal request latency.
	 *
	 * @return Simulated latency in seconds.
	 */
	UFUNCTION(BlueprintPure, Category = "UIDemo|Backend|Simulation")
	float GetSimulatedLatencySeconds() const;

	/**
	 * @brief Sets the delay used before a simulated timeout is returned.
	 *
	 * @param NewTimeoutDelaySeconds Requested timeout delay in seconds.
	 */
	UFUNCTION(BlueprintCallable, Category = "UIDemo|Backend|Simulation")
	void SetTimeoutDelaySeconds(float NewTimeoutDelaySeconds);

	/**
	 * @brief Returns the configured simulated timeout delay.
	 *
	 * @return Timeout delay in seconds.
	 */
	UFUNCTION(BlueprintPure, Category = "UIDemo|Backend|Simulation")
	float GetTimeoutDelaySeconds() const;

	/**
	 * @brief Restores deterministic default simulation settings.
	 */
	UFUNCTION(BlueprintCallable, Category = "UIDemo|Backend|Simulation")
	void ResetSimulationSettings();

	/**
	 * @brief Returns the number of requests currently waiting to complete.
	 *
	 * @return Number of active scheduled requests.
	 */
	UFUNCTION(BlueprintPure, Category = "UIDemo|Backend|Simulation")
	int32 GetActiveRequestCount() const;

	/** @brief Invalidates cached inventory and progression responses. */
	UFUNCTION(BlueprintCallable, Category = "UIDemo|Backend|Cache")
	void ClearCachedData();

private:
	/**
	 * @brief Schedules a callback on the game world's timer manager.
	 *
	 * @param DelaySeconds Delay before executing the callback.
	 * @param Completion Callback transferred into the scheduled request.
	 * @return Handle identifying the scheduled request.
	 */
	FBackendRequestHandle ScheduleRequest(
		float DelaySeconds,
		TFunction<void()>&& Completion);

	/**
	 * @brief Resolves the delay associated with a simulation scenario.
	 *
	 * @param Scenario Scenario captured by the request.
	 * @return Delay in seconds before the response completes.
	 */
	float GetDelayForScenario(
		EBackendSimulationScenario Scenario) const;

	/**
	 * @brief Builds a deterministic service-status response.
	 * @param Scenario Scenario captured by the request.
	 * @return Service-status response value.
	 */
	FBackendServiceStatusResponse BuildServiceStatusResponse(
		EBackendSimulationScenario Scenario) const;

	/**
	 * @brief Builds a deterministic remote-configuration response.
	 * @param Scenario Scenario captured by the request.
	 * @return Remote-configuration response value.
	 */
	FRemoteConfigResponse BuildRemoteConfigResponse(
		EBackendSimulationScenario Scenario) const;

	/**
	 * @brief Builds a deterministic player-profile response.
	 * @param Scenario Scenario captured by the request.
	 * @return Player-profile response value.
	 */
	FPlayerProfileResponse BuildPlayerProfileResponse(
		EBackendSimulationScenario Scenario) const;

	/**
	 * @brief Builds a deterministic inventory response.
	 * @param Scenario Scenario captured by the request.
	 * @return Inventory response value.
	 */
	FInventoryResponse BuildInventoryResponse(
		EBackendSimulationScenario Scenario) const;

	/**
	 * @brief Builds a deterministic progression response.
	 * @param Scenario Scenario captured by the request.
	 * @return Progression response value.
	 */
	FProgressionResponse BuildProgressionResponse(
		EBackendSimulationScenario Scenario) const;

	/**
	 * @brief Builds a deterministic matchmaking response.
	 * @param Scenario Scenario captured by the request.
	 * @param PlaylistId Playlist associated with the request.
	 * @return Matchmaking response value.
	 */
	FMatchmakingResponse BuildMatchmakingResponse(
		EBackendSimulationScenario Scenario,
		FName PlaylistId) const;

	/** @brief Populates the deterministic player, inventory, and progression data. */
	void InitializeMockData();

	/** @brief Recomputes skill availability from the current unlock graph. */
	void RecalculateSkillAvailability();

	/**
	 * @brief Tests whether a cached response is still within its TTL.
	 * @param CachedAtSeconds Platform timestamp at which the response was cached.
	 * @return True when the cached response remains valid.
	 */
	bool IsCacheValid(double CachedAtSeconds) const;

private:
	/** Scenario captured by future simulated requests. */
	UPROPERTY(Transient)
	EBackendSimulationScenario SimulationScenario =
		EBackendSimulationScenario::Success;

	/** Normal simulated network latency in seconds. */
	UPROPERTY(Transient)
	float SimulatedLatencySeconds = 0.75f;

	/** Delay in seconds before timeout scenarios complete. */
	UPROPERTY(Transient)
	float TimeoutDelaySeconds = 4.0f;

	/** Monotonically increasing identifier assigned to scheduled requests. */
	int32 NextRequestId = 1;

	/** Timer handles indexed by their public request identifier. */
	TMap<int32, FTimerHandle> ActiveRequests;

	/** Mutable player profile backing deterministic responses. */
	FPlayerProfile MockPlayerProfile;
	/** Mutable inventory backing deterministic responses. */
	TArray<FInventoryItem> MockInventory;
	/** Mutable progression graph backing deterministic responses. */
	FProgressionData MockProgression;

	/** Whether InventoryCache currently contains a valid response candidate. */
	bool bHasInventoryCache = false;
	/** Most recently cached successful inventory response. */
	FInventoryResponse InventoryCache;
	/** Platform timestamp at which InventoryCache was populated. */
	double InventoryCachedAtSeconds = 0.0;

	/** Whether ProgressionCache currently contains a valid response candidate. */
	bool bHasProgressionCache = false;
	/** Most recently cached successful progression response. */
	FProgressionResponse ProgressionCache;
	/** Platform timestamp at which ProgressionCache was populated. */
	double ProgressionCachedAtSeconds = 0.0;
};
