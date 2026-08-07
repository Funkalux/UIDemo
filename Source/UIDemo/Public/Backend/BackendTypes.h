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
	None,
	ServiceStatus,
	RemoteConfig,
	PlayerProfile,
	Inventory,
	EquipItem,
	Progression,
	UnlockSkill,
	Matchmaking
};

/**
 * Transport or service-level error categories.
 * User-facing text should be created by the ViewModel, not the backend.
 */
UENUM(BlueprintType)
enum class EBackendErrorCode : uint8
{
	None,
	RequestFailed,
	Timeout,
	ServiceUnavailable,
	InvalidResponse,
	Cancelled,
	InternalError
};

/**
 * High-level state of a backend service.
 */
UENUM(BlueprintType)
enum class EBackendServiceState : uint8
{
	Unknown,
	Operational,
	Degraded,
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
	Success,
	ServiceUnavailable,
	ProfileRequestFailed,
	ConfigRequestFailed,
	InventoryEmpty,
	InventoryRequestFailed,
	EquipItemFailed,
	ProgressionRequestFailed,
	UnlockSkillFailed,
	InsufficientSkillPoints,
	MatchmakingFailed,
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
	UPROPERTY(BlueprintReadOnly, Category = "Backend")
	int32 Id = INDEX_NONE;

	bool IsValid() const
	{
		return Id != INDEX_NONE;
	}

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
	UPROPERTY(BlueprintReadOnly, Category = "Backend")
	EBackendEndpoint Endpoint = EBackendEndpoint::None;

	UPROPERTY(BlueprintReadOnly, Category = "Backend")
	EBackendErrorCode ErrorCode = EBackendErrorCode::None;

	UPROPERTY(BlueprintReadOnly, Category = "Backend")
	FString DebugMessage;

	UPROPERTY(BlueprintReadOnly, Category = "Backend")
	bool bRetryable = false;

	bool HasError() const
	{
		return ErrorCode != EBackendErrorCode::None;
	}

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
	UPROPERTY(BlueprintReadOnly, Category = "Backend")
	FName ServiceId = NAME_None;

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

	UPROPERTY(BlueprintReadOnly, Category = "Backend")
	EBackendServiceState OverallState = EBackendServiceState::Unknown;

	UPROPERTY(BlueprintReadOnly, Category = "Backend")
	TArray<FBackendServiceStatusEntry> Services;

	UPROPERTY(BlueprintReadOnly, Category = "Backend")
	FString Environment;

	UPROPERTY(BlueprintReadOnly, Category = "Backend")
	FString ServiceVersion;

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
	UPROPERTY(BlueprintReadOnly, Category = "Backend")
	FString SeasonId;

	UPROPERTY(BlueprintReadOnly, Category = "Backend")
	FString SeasonDisplayName;

	UPROPERTY(BlueprintReadOnly, Category = "Backend")
	bool bIsMatchmakingEnabled = false;

	UPROPERTY(BlueprintReadOnly, Category = "Backend")
	bool bIsInventoryEnabled = false;

	UPROPERTY(BlueprintReadOnly, Category = "Backend")
	bool bIsProgressionEnabled = false;

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
	UPROPERTY(BlueprintReadOnly, Category = "Backend")
	bool bRequestSucceeded = false;

	UPROPERTY(BlueprintReadOnly, Category = "Backend")
	FRemoteConfig Config;

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
	UPROPERTY(BlueprintReadOnly, Category = "Backend")
	FString PlayerId;

	UPROPERTY(BlueprintReadOnly, Category = "Backend")
	FString DisplayName;

	UPROPERTY(BlueprintReadOnly, Category = "Backend")
	int32 PlayerLevel = 0;

	UPROPERTY(BlueprintReadOnly, Category = "Backend")
	int32 CurrentExperience = 0;

	UPROPERTY(BlueprintReadOnly, Category = "Backend")
	int32 ExperienceForNextLevel = 0;

	UPROPERTY(BlueprintReadOnly, Category = "Backend")
	int32 SoftCurrency = 0;

	UPROPERTY(BlueprintReadOnly, Category = "Backend")
	int32 PremiumCurrency = 0;

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
	UPROPERTY(BlueprintReadOnly, Category = "Backend")
	bool bRequestSucceeded = false;

	UPROPERTY(BlueprintReadOnly, Category = "Backend")
	FPlayerProfile Profile;

	UPROPERTY(BlueprintReadOnly, Category = "Backend")
	FBackendError Error;
};
