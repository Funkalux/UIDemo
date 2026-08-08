#pragma once

#include "CoreMinimal.h"
#include "BackendTypes.generated.h"

/**
 * Backend endpoints currently represented by the demo.
 * Additional endpoints can be added as their screens are implemented.
 */
UENUM(BlueprintType)
enum class EBackendEndpoint : uint8
{
	/** No endpoint is associated with the value. */
	None,
	/** Service-health endpoint used during bootstrap. */
	ServiceStatus,
	/** Remote feature-configuration endpoint. */
	RemoteConfig,
	/** Player-profile endpoint. */
	PlayerProfile,
	/** Inventory query endpoint. */
	Inventory,
	/** Inventory equipment mutation endpoint. */
	EquipItem,
	/** Skill-progression query endpoint. */
	Progression,
	/** Skill-unlock mutation endpoint. */
	UnlockSkill,
	/** Matchmaking request endpoint. */
	Matchmaking
};

/**
 * Transport or service-level error categories.
 * User-facing text should be created by the ViewModel, not the backend.
 */
UENUM(BlueprintType)
enum class EBackendErrorCode : uint8
{
	/** No error occurred. */
	None,
	/** The request failed for a generic transport or service reason. */
	RequestFailed,
	/** The request exceeded its configured timeout. */
	Timeout,
	/** A required service is unavailable. */
	ServiceUnavailable,
	/** The service returned data that could not be accepted. */
	InvalidResponse,
	/** The caller cancelled the request. */
	Cancelled,
	/** An unexpected internal failure occurred. */
	InternalError
};

/**
 * High-level state of a backend service.
 */
UENUM(BlueprintType)
enum class EBackendServiceState : uint8
{
	/** Service state has not been established. */
	Unknown,
	/** Service is healthy and fully available. */
	Operational,
	/** Service is available with reduced functionality. */
	Degraded,
	/** Service cannot currently handle requests. */
	Unavailable
};

/**
 * Deterministic simulation modes used by the development tools.
 *
 * Success is always the default. Failures are explicitly selected so that
 * tests, captures, and demonstrations remain reproducible.
 */
UENUM(BlueprintType)
enum class EBackendSimulationScenario : uint8
{
	/** Every supported request completes successfully. */
	Success,
	/** Required backend services report as unavailable. */
	ServiceUnavailable,
	/** Player-profile requests fail. */
	ProfileRequestFailed,
	/** Remote-configuration requests fail. */
	ConfigRequestFailed,
	/** Inventory requests succeed with no items. */
	InventoryEmpty,
	/** Inventory query requests fail. */
	InventoryRequestFailed,
	/** Equipment mutation requests fail. */
	EquipItemFailed,
	/** Progression query requests fail. */
	ProgressionRequestFailed,
	/** Skill-unlock mutation requests fail. */
	UnlockSkillFailed,
	/** Skill-unlock requests fail because the player lacks points. */
	InsufficientSkillPoints,
	/** Matchmaking requests fail. */
	MatchmakingFailed,
	/** Supported requests complete with a timeout error after a long delay. */
	Timeout
};

/**
 * Handle returned for every simulated request.
 */
USTRUCT(BlueprintType)
struct UIDEMO_API FBackendRequestHandle
{
	GENERATED_BODY()

public:
	/** Opaque request identifier; INDEX_NONE represents an invalid handle. */
	UPROPERTY(BlueprintReadOnly, Category = "Backend")
	int32 Id = INDEX_NONE;

	/**
	 * @brief Reports whether this handle identifies a scheduled request.
	 *
	 * @return True when the handle contains a valid request identifier.
	 */
	bool IsValid() const
	{
		return Id != INDEX_NONE;
	}

	/** @brief Invalidates the handle without cancelling its request. */
	void Reset()
	{
		Id = INDEX_NONE;
	}
};

/**
 * Machine-readable backend error.
 *
 * DebugMessage is intended for logs and diagnostics. The ViewModel should map
 * ErrorCode to localized, user-facing copy.
 */
USTRUCT(BlueprintType)
struct UIDEMO_API FBackendError
{
	GENERATED_BODY()

public:
	/** Endpoint that produced the error. */
	UPROPERTY(BlueprintReadOnly, Category = "Backend")
	EBackendEndpoint Endpoint = EBackendEndpoint::None;

	/** Machine-readable category of the failure. */
	UPROPERTY(BlueprintReadOnly, Category = "Backend")
	EBackendErrorCode ErrorCode = EBackendErrorCode::None;

	/** Diagnostic message intended for logs rather than player-facing UI. */
	UPROPERTY(BlueprintReadOnly, Category = "Backend")
	FString DebugMessage;

	/** Whether retrying the operation is a meaningful recovery action. */
	UPROPERTY(BlueprintReadOnly, Category = "Backend")
	bool bRetryable = false;

	/**
	 * @brief Reports whether the structure represents a failure.
	 *
	 * @return True when ErrorCode is not None.
	 */
	bool HasError() const
	{
		return ErrorCode != EBackendErrorCode::None;
	}

	/**
	 * @brief Creates a fully populated backend error value.
	 *
	 * @param InEndpoint Endpoint responsible for the error.
	 * @param InErrorCode Machine-readable failure category.
	 * @param InDebugMessage Diagnostic message moved into the result.
	 * @param bInRetryable Whether retry should be offered to the user.
	 * @return A populated backend error structure.
	 */
	static FBackendError Make(
		const EBackendEndpoint InEndpoint,
		const EBackendErrorCode InErrorCode,
		FString InDebugMessage,
		const bool bInRetryable)
	{
		FBackendError Error;
		Error.Endpoint = InEndpoint;
		Error.ErrorCode = InErrorCode;
		Error.DebugMessage = MoveTemp(InDebugMessage);
		Error.bRetryable = bInRetryable;
		return Error;
	}
};

/**
 * Status of one logical backend service.
 */
USTRUCT(BlueprintType)
struct UIDEMO_API FBackendServiceStatusEntry
{
	GENERATED_BODY()

public:
	/** Stable identifier for the represented backend service. */
	UPROPERTY(BlueprintReadOnly, Category = "Backend")
	FName ServiceId = NAME_None;

	/** Current health state reported by the service. */
	UPROPERTY(BlueprintReadOnly, Category = "Backend")
	EBackendServiceState State = EBackendServiceState::Unknown;
};

/**
 * Result returned by the service-status endpoint.
 */
USTRUCT(BlueprintType)
struct UIDEMO_API FBackendServiceStatusResponse
{
	GENERATED_BODY()

public:
	/**
	 * True when the request itself completed successfully.
	 * Individual services may still report Degraded or Unavailable.
	 */
	UPROPERTY(BlueprintReadOnly, Category = "Backend")
	bool bRequestSucceeded = false;

	/** Aggregate health state across every required service. */
	UPROPERTY(BlueprintReadOnly, Category = "Backend")
	EBackendServiceState OverallState = EBackendServiceState::Unknown;

	/** Per-service health details returned by the endpoint. */
	UPROPERTY(BlueprintReadOnly, Category = "Backend")
	TArray<FBackendServiceStatusEntry> Services;

	/** Backend environment name, such as development or production. */
	UPROPERTY(BlueprintReadOnly, Category = "Backend")
	FString Environment;

	/** Human-readable backend service version. */
	UPROPERTY(BlueprintReadOnly, Category = "Backend")
	FString ServiceVersion;

	/** Failure details when the request did not succeed. */
	UPROPERTY(BlueprintReadOnly, Category = "Backend")
	FBackendError Error;
};

/**
 * Remote feature and presentation configuration.
 */
USTRUCT(BlueprintType)
struct UIDEMO_API FRemoteConfig
{
	GENERATED_BODY()

public:
	/** Stable identifier for the active season. */
	UPROPERTY(BlueprintReadOnly, Category = "Backend")
	FString SeasonId;

	/** Localized or presentation-ready season name. */
	UPROPERTY(BlueprintReadOnly, Category = "Backend")
	FString SeasonDisplayName;

	/** Whether matchmaking is enabled by remote configuration. */
	UPROPERTY(BlueprintReadOnly, Category = "Backend")
	bool bIsMatchmakingEnabled = false;

	/** Whether inventory is enabled by remote configuration. */
	UPROPERTY(BlueprintReadOnly, Category = "Backend")
	bool bIsInventoryEnabled = false;

	/** Whether progression is enabled by remote configuration. */
	UPROPERTY(BlueprintReadOnly, Category = "Backend")
	bool bIsProgressionEnabled = false;

	/** Suggested player-profile cache lifetime in seconds. */
	UPROPERTY(BlueprintReadOnly, Category = "Backend")
	int32 ProfileCacheTtlSeconds = 0;
};

/**
 * Result returned by the remote-config endpoint.
 */
USTRUCT(BlueprintType)
struct UIDEMO_API FRemoteConfigResponse
{
	GENERATED_BODY()

public:
	/** Whether the remote-configuration request completed successfully. */
	UPROPERTY(BlueprintReadOnly, Category = "Backend")
	bool bRequestSucceeded = false;

	/** Configuration payload returned on success. */
	UPROPERTY(BlueprintReadOnly, Category = "Backend")
	FRemoteConfig Config;

	/** Failure details returned when the request did not succeed. */
	UPROPERTY(BlueprintReadOnly, Category = "Backend")
	FBackendError Error;
};

/**
 * Small player profile needed by the frontend shell.
 */
USTRUCT(BlueprintType)
struct UIDEMO_API FPlayerProfile
{
	GENERATED_BODY()

public:
	/** Stable backend identifier for the player. */
	UPROPERTY(BlueprintReadOnly, Category = "Backend")
	FString PlayerId;

	/** Player-facing display name. */
	UPROPERTY(BlueprintReadOnly, Category = "Backend")
	FString DisplayName;

	/** Current account level. */
	UPROPERTY(BlueprintReadOnly, Category = "Backend")
	int32 PlayerLevel = 0;

	/** Experience accumulated within the current level. */
	UPROPERTY(BlueprintReadOnly, Category = "Backend")
	int32 CurrentExperience = 0;

	/** Experience required to reach the next level. */
	UPROPERTY(BlueprintReadOnly, Category = "Backend")
	int32 ExperienceForNextLevel = 0;

	/** Balance of the earnable account currency. */
	UPROPERTY(BlueprintReadOnly, Category = "Backend")
	int32 SoftCurrency = 0;

	/** Balance of the premium account currency. */
	UPROPERTY(BlueprintReadOnly, Category = "Backend")
	int32 PremiumCurrency = 0;

	/** Number of progression points available for skill unlocks. */
	UPROPERTY(BlueprintReadOnly, Category = "Backend")
	int32 SkillPoints = 0;
};

/**
 * Result returned by the player-profile endpoint.
 */
USTRUCT(BlueprintType)
struct UIDEMO_API FPlayerProfileResponse
{
	GENERATED_BODY()

public:
	/** Whether the player-profile request completed successfully. */
	UPROPERTY(BlueprintReadOnly, Category = "Backend")
	bool bRequestSucceeded = false;

	/** Player-profile payload returned on success. */
	UPROPERTY(BlueprintReadOnly, Category = "Backend")
	FPlayerProfile Profile;

	/** Failure details returned when the request did not succeed. */
	UPROPERTY(BlueprintReadOnly, Category = "Backend")
	FBackendError Error;
};
