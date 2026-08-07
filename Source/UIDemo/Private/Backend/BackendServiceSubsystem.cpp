#include "Backend/BackendServiceSubsystem.h"

#include "Engine/GameInstance.h"
#include "Engine/World.h"
#include "TimerManager.h"

DEFINE_LOG_CATEGORY_STATIC(LogUIDemoBackend, Log, All);

namespace UIDemoBackend
{
	constexpr float DefaultLatencySeconds = 0.75f;
	constexpr float DefaultTimeoutDelaySeconds = 4.0f;
	constexpr float MinimumTimerDelaySeconds = 0.001f;
	constexpr float MaximumSimulatedDelaySeconds = 30.0f;

	FBackendServiceStatusEntry MakeServiceStatus(
		const FName ServiceId,
		const EBackendServiceState State)
	{
		FBackendServiceStatusEntry Entry;
		Entry.ServiceId = ServiceId;
		Entry.State = State;
		return Entry;
	}
}

void UBackendServiceSubsystem::Initialize(
	FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	ResetSimulationSettings();

	UE_LOG(
		LogUIDemoBackend,
		Log,
		TEXT("Backend service subsystem initialized."));
}

void UBackendServiceSubsystem::Deinitialize()
{
	CancelAllRequests();

	UE_LOG(
		LogUIDemoBackend,
		Log,
		TEXT("Backend service subsystem deinitialized."));

	Super::Deinitialize();
}

FBackendRequestHandle UBackendServiceSubsystem::CheckServiceStatus(
	FOnBackendServiceStatusCompleted Completion)
{
	const EBackendSimulationScenario RequestScenario =
		SimulationScenario;

	const float RequestDelay =
		GetDelayForScenario(RequestScenario);

	FBackendServiceStatusResponse Response =
		BuildServiceStatusResponse(RequestScenario);

	UE_LOG(
		LogUIDemoBackend,
		Verbose,
		TEXT("Scheduling service-status request. Scenario: %s"),
		*UEnum::GetValueAsString(RequestScenario));

	return ScheduleRequest(
		RequestDelay,
		[
			Completion = MoveTemp(Completion),
			Response = MoveTemp(Response)
		]() mutable
		{
			Completion.ExecuteIfBound(Response);
		});
}

FBackendRequestHandle UBackendServiceSubsystem::FetchRemoteConfig(
	FOnRemoteConfigCompleted Completion)
{
	const EBackendSimulationScenario RequestScenario =
		SimulationScenario;

	const float RequestDelay =
		GetDelayForScenario(RequestScenario);

	FRemoteConfigResponse Response =
		BuildRemoteConfigResponse(RequestScenario);

	UE_LOG(
		LogUIDemoBackend,
		Verbose,
		TEXT("Scheduling remote-config request. Scenario: %s"),
		*UEnum::GetValueAsString(RequestScenario));

	return ScheduleRequest(
		RequestDelay,
		[
			Completion = MoveTemp(Completion),
			Response = MoveTemp(Response)
		]() mutable
		{
			Completion.ExecuteIfBound(Response);
		});
}

FBackendRequestHandle UBackendServiceSubsystem::FetchPlayerProfile(
	FOnPlayerProfileCompleted Completion)
{
	const EBackendSimulationScenario RequestScenario =
		SimulationScenario;

	const float RequestDelay =
		GetDelayForScenario(RequestScenario);

	FPlayerProfileResponse Response =
		BuildPlayerProfileResponse(RequestScenario);

	UE_LOG(
		LogUIDemoBackend,
		Verbose,
		TEXT("Scheduling player-profile request. Scenario: %s"),
		*UEnum::GetValueAsString(RequestScenario));

	return ScheduleRequest(
		RequestDelay,
		[
			Completion = MoveTemp(Completion),
			Response = MoveTemp(Response)
		]() mutable
		{
			Completion.ExecuteIfBound(Response);
		});
}

bool UBackendServiceSubsystem::CancelRequest(
	const FBackendRequestHandle& RequestHandle)
{
	if (!RequestHandle.IsValid())
	{
		return false;
	}

	FTimerHandle* TimerHandle =
		ActiveRequests.Find(RequestHandle.Id);

	if (TimerHandle == nullptr)
	{
		return false;
	}

	if (const UGameInstance* GameInstance = GetGameInstance())
	{
		if (UWorld* World = GameInstance->GetWorld())
		{
			World->GetTimerManager().ClearTimer(*TimerHandle);
		}
	}

	ActiveRequests.Remove(RequestHandle.Id);

	UE_LOG(
		LogUIDemoBackend,
		Verbose,
		TEXT("Cancelled backend request %d."),
		RequestHandle.Id);

	return true;
}

void UBackendServiceSubsystem::CancelAllRequests()
{
	if (const UGameInstance* GameInstance = GetGameInstance())
	{
		if (UWorld* World = GameInstance->GetWorld())
		{
			for (TPair<int32, FTimerHandle>& Request : ActiveRequests)
			{
				World->GetTimerManager().ClearTimer(Request.Value);
			}
		}
	}

	const int32 CancelledRequestCount = ActiveRequests.Num();
	ActiveRequests.Reset();

	if (CancelledRequestCount > 0)
	{
		UE_LOG(
			LogUIDemoBackend,
			Verbose,
			TEXT("Cancelled %d backend requests."),
			CancelledRequestCount);
	}
}

void UBackendServiceSubsystem::SetSimulationScenario(
	const EBackendSimulationScenario NewScenario)
{
	SimulationScenario = NewScenario;

	UE_LOG(
		LogUIDemoBackend,
		Log,
		TEXT("Backend simulation scenario set to %s."),
		*UEnum::GetValueAsString(SimulationScenario));
}

EBackendSimulationScenario
UBackendServiceSubsystem::GetSimulationScenario() const
{
	return SimulationScenario;
}

void UBackendServiceSubsystem::SetSimulatedLatencySeconds(
	const float NewLatencySeconds)
{
	SimulatedLatencySeconds = FMath::Clamp(
		NewLatencySeconds,
		0.0f,
		UIDemoBackend::MaximumSimulatedDelaySeconds);
}

float UBackendServiceSubsystem::GetSimulatedLatencySeconds() const
{
	return SimulatedLatencySeconds;
}

void UBackendServiceSubsystem::SetTimeoutDelaySeconds(
	const float NewTimeoutDelaySeconds)
{
	TimeoutDelaySeconds = FMath::Clamp(
		NewTimeoutDelaySeconds,
		0.0f,
		UIDemoBackend::MaximumSimulatedDelaySeconds);
}

float UBackendServiceSubsystem::GetTimeoutDelaySeconds() const
{
	return TimeoutDelaySeconds;
}

void UBackendServiceSubsystem::ResetSimulationSettings()
{
	SimulationScenario = EBackendSimulationScenario::Success;
	SimulatedLatencySeconds =
		UIDemoBackend::DefaultLatencySeconds;
	TimeoutDelaySeconds =
		UIDemoBackend::DefaultTimeoutDelaySeconds;
}

int32 UBackendServiceSubsystem::GetActiveRequestCount() const
{
	return ActiveRequests.Num();
}

FBackendRequestHandle UBackendServiceSubsystem::ScheduleRequest(
	const float DelaySeconds,
	TFunction<void()>&& Completion)
{
	FBackendRequestHandle RequestHandle;

	if (!Completion)
	{
		UE_LOG(
			LogUIDemoBackend,
			Error,
			TEXT("Cannot schedule a backend request without a completion callback."));

		return RequestHandle;
	}

	const UGameInstance* GameInstance = GetGameInstance();
	UWorld* World =
		GameInstance != nullptr
		? GameInstance->GetWorld()
		: nullptr;

	if (World == nullptr)
	{
		UE_LOG(
			LogUIDemoBackend,
			Error,
			TEXT("Cannot schedule a backend request because no valid world exists."));

		// Avoid leaving the caller permanently waiting.
		Completion();
		return RequestHandle;
	}

	RequestHandle.Id = NextRequestId++;

	const float EffectiveDelay = FMath::Max(
		DelaySeconds,
		UIDemoBackend::MinimumTimerDelaySeconds);

	const int32 RequestId = RequestHandle.Id;
	TWeakObjectPtr<UBackendServiceSubsystem> WeakThis(this);

	TFunction<void()> TimerCallback =
		[
			WeakThis,
			RequestId,
			Completion = MoveTemp(Completion)
		]() mutable
		{
			UBackendServiceSubsystem* StrongThis =
				WeakThis.Get();

			if (StrongThis == nullptr)
			{
				return;
			}

			StrongThis->ActiveRequests.Remove(RequestId);
			Completion();
		};

	FTimerHandle TimerHandle;

	World->GetTimerManager().SetTimer(
		TimerHandle,
		MoveTemp(TimerCallback),
		EffectiveDelay,
		false,
		EffectiveDelay);

	ActiveRequests.Add(RequestId, TimerHandle);

	UE_LOG(
		LogUIDemoBackend,
		Verbose,
		TEXT("Scheduled backend request %d with %.3f seconds of latency."),
		RequestId,
		EffectiveDelay);

	return RequestHandle;
}

float UBackendServiceSubsystem::GetDelayForScenario(
	const EBackendSimulationScenario Scenario) const
{
	return Scenario == EBackendSimulationScenario::Timeout
		? TimeoutDelaySeconds
		: SimulatedLatencySeconds;
}

FBackendServiceStatusResponse
UBackendServiceSubsystem::BuildServiceStatusResponse(
	const EBackendSimulationScenario Scenario) const
{
	FBackendServiceStatusResponse Response;
	Response.Environment = TEXT("Mock");
	Response.ServiceVersion = TEXT("1.0.0");

	if (Scenario == EBackendSimulationScenario::Timeout)
	{
		Response.Error = FBackendError::Make(
			EBackendEndpoint::ServiceStatus,
			EBackendErrorCode::Timeout,
			TEXT("The simulated service-status request timed out."),
			true);

		return Response;
	}

	if (Scenario ==
		EBackendSimulationScenario::ServiceUnavailable)
	{
		Response.bRequestSucceeded = true;
		Response.OverallState =
			EBackendServiceState::Unavailable;

		Response.Services.Add(
			UIDemoBackend::MakeServiceStatus(
				TEXT("Identity"),
				EBackendServiceState::Unavailable));

		Response.Services.Add(
			UIDemoBackend::MakeServiceStatus(
				TEXT("PlayerData"),
				EBackendServiceState::Unavailable));

		Response.Services.Add(
			UIDemoBackend::MakeServiceStatus(
				TEXT("Configuration"),
				EBackendServiceState::Unavailable));

		return Response;
	}

	Response.bRequestSucceeded = true;
	Response.OverallState =
		EBackendServiceState::Operational;

	Response.Services.Add(
		UIDemoBackend::MakeServiceStatus(
			TEXT("Identity"),
			EBackendServiceState::Operational));

	Response.Services.Add(
		UIDemoBackend::MakeServiceStatus(
			TEXT("PlayerData"),
			EBackendServiceState::Operational));

	Response.Services.Add(
		UIDemoBackend::MakeServiceStatus(
			TEXT("Configuration"),
			EBackendServiceState::Operational));

	return Response;
}

FRemoteConfigResponse
UBackendServiceSubsystem::BuildRemoteConfigResponse(
	const EBackendSimulationScenario Scenario) const
{
	FRemoteConfigResponse Response;

	if (Scenario == EBackendSimulationScenario::Timeout)
	{
		Response.Error = FBackendError::Make(
			EBackendEndpoint::RemoteConfig,
			EBackendErrorCode::Timeout,
			TEXT("The simulated remote-config request timed out."),
			true);

		return Response;
	}

	if (Scenario ==
		EBackendSimulationScenario::ServiceUnavailable)
	{
		Response.Error = FBackendError::Make(
			EBackendEndpoint::RemoteConfig,
			EBackendErrorCode::ServiceUnavailable,
			TEXT("Remote config is unavailable because backend services are offline."),
			true);

		return Response;
	}

	if (Scenario ==
		EBackendSimulationScenario::ConfigRequestFailed)
	{
		Response.Error = FBackendError::Make(
			EBackendEndpoint::RemoteConfig,
			EBackendErrorCode::RequestFailed,
			TEXT("The simulated remote-config request failed."),
			true);

		return Response;
	}

	Response.bRequestSucceeded = true;
	Response.Config.SeasonId = TEXT("season_01");
	Response.Config.SeasonDisplayName = TEXT("Season One");
	Response.Config.bIsMatchmakingEnabled = true;
	Response.Config.bIsInventoryEnabled = true;
	Response.Config.bIsProgressionEnabled = true;
	Response.Config.ProfileCacheTtlSeconds = 120;

	return Response;
}

FPlayerProfileResponse
UBackendServiceSubsystem::BuildPlayerProfileResponse(
	const EBackendSimulationScenario Scenario) const
{
	FPlayerProfileResponse Response;

	if (Scenario == EBackendSimulationScenario::Timeout)
	{
		Response.Error = FBackendError::Make(
			EBackendEndpoint::PlayerProfile,
			EBackendErrorCode::Timeout,
			TEXT("The simulated player-profile request timed out."),
			true);

		return Response;
	}

	if (Scenario ==
		EBackendSimulationScenario::ServiceUnavailable)
	{
		Response.Error = FBackendError::Make(
			EBackendEndpoint::PlayerProfile,
			EBackendErrorCode::ServiceUnavailable,
			TEXT("Player data is unavailable because backend services are offline."),
			true);

		return Response;
	}

	if (Scenario ==
		EBackendSimulationScenario::ProfileRequestFailed)
	{
		Response.Error = FBackendError::Make(
			EBackendEndpoint::PlayerProfile,
			EBackendErrorCode::RequestFailed,
			TEXT("The simulated player-profile request failed."),
			true);

		return Response;
	}

	Response.bRequestSucceeded = true;
	Response.Profile.PlayerId = TEXT("player_demo_001");
	Response.Profile.DisplayName = TEXT("DemoPlayer");
	Response.Profile.PlayerLevel = 24;
	Response.Profile.CurrentExperience = 675;
	Response.Profile.ExperienceForNextLevel = 1000;
	Response.Profile.SoftCurrency = 12500;
	Response.Profile.PremiumCurrency = 850;
	Response.Profile.SkillPoints = 3;

	return Response;
}