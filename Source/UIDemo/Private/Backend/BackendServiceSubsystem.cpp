#include "Backend/BackendServiceSubsystem.h"

#include "Algo/AllOf.h"
#include "Engine/GameInstance.h"
#include "Engine/World.h"
#include "ProfilingDebugging/CpuProfilerTrace.h"
#include "TimerManager.h"

#define LOCTEXT_NAMESPACE "BackendServiceSubsystem"

DEFINE_LOG_CATEGORY_STATIC(LogUIDemoBackend, Log, All);

namespace UIDemoBackend
{
	constexpr float DefaultLatencySeconds = 0.75f;
	constexpr float DefaultTimeoutDelaySeconds = 4.0f;
	constexpr float MinimumTimerDelaySeconds = 0.001f;
	constexpr float CachedResponseDelaySeconds = 0.08f;
	constexpr float MatchmakingDelaySeconds = 2.5f;
	constexpr float MaximumSimulatedDelaySeconds = 30.0f;
	constexpr double CacheTtlSeconds = 60.0;

	FBackendServiceStatusEntry MakeServiceStatus(
		const FName ServiceId,
		const EBackendServiceState State)
	{
		FBackendServiceStatusEntry Entry;
		Entry.ServiceId = ServiceId;
		Entry.State = State;
		return Entry;
	}

	FInventoryItem MakeItem(
		const FName ItemId,
		const FText& DisplayName,
		const FText& Description,
		const FName IconId,
		const EInventoryItemCategory Category,
		const EInventoryItemRarity Rarity,
		const int32 Power,
		const bool bEquipped)
	{
		FInventoryItem Item;
		Item.ItemId = ItemId;
		Item.DisplayName = DisplayName;
		Item.Description = Description;
		Item.IconId = IconId;
		Item.Category = Category;
		Item.Rarity = Rarity;
		Item.Power = Power;
		Item.bEquipped = bEquipped;
		return Item;
	}

	FSkillNodeData MakeSkill(
		const FName SkillId,
		const FText& DisplayName,
		const FText& Description,
		const int32 Cost,
		const TArray<FName>& Prerequisites,
		const int32 Row,
		const int32 Column,
		const ESkillNodeState State)
	{
		FSkillNodeData Node;
		Node.SkillId = SkillId;
		Node.DisplayName = DisplayName;
		Node.Description = Description;
		Node.Cost = Cost;
		Node.PrerequisiteIds = Prerequisites;
		Node.Row = Row;
		Node.Column = Column;
		Node.State = State;
		return Node;
	}

	FBackendError MakeScenarioError(
		const EBackendEndpoint Endpoint,
		const EBackendSimulationScenario Scenario,
		const FString& RequestName)
	{
		if (Scenario == EBackendSimulationScenario::Timeout)
		{
			return FBackendError::Make(
				Endpoint,
				EBackendErrorCode::Timeout,
				FString::Printf(TEXT("The simulated %s request timed out."), *RequestName),
				true);
		}

		if (Scenario == EBackendSimulationScenario::ServiceUnavailable)
		{
			return FBackendError::Make(
				Endpoint,
				EBackendErrorCode::ServiceUnavailable,
				FString::Printf(TEXT("%s is unavailable because services are offline."), *RequestName),
				true);
		}

		return FBackendError();
	}
}

void UBackendServiceSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	ResetSimulationSettings();
	InitializeMockData();
	ClearCachedData();

	UE_LOG(LogUIDemoBackend, Log, TEXT("Backend service subsystem initialized."));
}

void UBackendServiceSubsystem::Deinitialize()
{
	CancelAllRequests();
	ClearCachedData();
	UE_LOG(LogUIDemoBackend, Log, TEXT("Backend service subsystem deinitialized."));
	Super::Deinitialize();
}

FBackendRequestHandle UBackendServiceSubsystem::CheckServiceStatus(
	FOnBackendServiceStatusCompleted Completion)
{
	const EBackendSimulationScenario Scenario = SimulationScenario;
	FBackendServiceStatusResponse Response = BuildServiceStatusResponse(Scenario);

	return ScheduleRequest(
		GetDelayForScenario(Scenario),
		[Completion = MoveTemp(Completion), Response = MoveTemp(Response)]() mutable
		{
			Completion.ExecuteIfBound(Response);
		});
}

FBackendRequestHandle UBackendServiceSubsystem::FetchRemoteConfig(
	FOnRemoteConfigCompleted Completion)
{
	const EBackendSimulationScenario Scenario = SimulationScenario;
	FRemoteConfigResponse Response = BuildRemoteConfigResponse(Scenario);

	return ScheduleRequest(
		GetDelayForScenario(Scenario),
		[Completion = MoveTemp(Completion), Response = MoveTemp(Response)]() mutable
		{
			Completion.ExecuteIfBound(Response);
		});
}

FBackendRequestHandle UBackendServiceSubsystem::FetchPlayerProfile(
	FOnPlayerProfileCompleted Completion)
{
	const EBackendSimulationScenario Scenario = SimulationScenario;
	FPlayerProfileResponse Response = BuildPlayerProfileResponse(Scenario);

	return ScheduleRequest(
		GetDelayForScenario(Scenario),
		[Completion = MoveTemp(Completion), Response = MoveTemp(Response)]() mutable
		{
			Completion.ExecuteIfBound(Response);
		});
}

FBackendRequestHandle UBackendServiceSubsystem::FetchInventory(
	const bool bForceRefresh,
	FOnInventoryCompleted Completion)
{
	const bool bUseCache = !bForceRefresh && bHasInventoryCache &&
		IsCacheValid(InventoryCachedAtSeconds);

	if (bUseCache)
	{
		FInventoryResponse Response = InventoryCache;
		Response.bFromCache = true;
		return ScheduleRequest(
			UIDemoBackend::CachedResponseDelaySeconds,
			[Completion = MoveTemp(Completion), Response = MoveTemp(Response)]() mutable
			{
				Completion.ExecuteIfBound(Response);
			});
	}

	const EBackendSimulationScenario Scenario = SimulationScenario;
	FInventoryResponse Response = BuildInventoryResponse(Scenario);

	return ScheduleRequest(
		GetDelayForScenario(Scenario),
		[this, Completion = MoveTemp(Completion), Response = MoveTemp(Response)]() mutable
		{
			if (Response.bRequestSucceeded)
			{
				InventoryCache = Response;
				InventoryCache.bFromCache = false;
				bHasInventoryCache = true;
				InventoryCachedAtSeconds = FPlatformTime::Seconds();
			}
			Completion.ExecuteIfBound(Response);
		});
}

FBackendRequestHandle UBackendServiceSubsystem::EquipItem(
	const FName ItemId,
	FOnEquipItemCompleted Completion)
{
	const EBackendSimulationScenario Scenario = SimulationScenario;
	return ScheduleRequest(
		GetDelayForScenario(Scenario),
		[this, Scenario, ItemId, Completion = MoveTemp(Completion)]() mutable
		{
			FEquipItemResponse Response;
			Response.Error = UIDemoBackend::MakeScenarioError(
				EBackendEndpoint::EquipItem, Scenario, TEXT("equip-item"));

			if (!Response.Error.HasError() && Scenario == EBackendSimulationScenario::EquipItemFailed)
			{
				Response.Error = FBackendError::Make(
					EBackendEndpoint::EquipItem,
					EBackendErrorCode::RequestFailed,
					TEXT("The simulated equip-item request failed."),
					true);
			}

			FInventoryItem* SelectedItem = MockInventory.FindByPredicate(
				[ItemId](const FInventoryItem& Item) { return Item.ItemId == ItemId; });

			if (!Response.Error.HasError() && SelectedItem == nullptr)
			{
				Response.Error = FBackendError::Make(
					EBackendEndpoint::EquipItem,
					EBackendErrorCode::InvalidResponse,
					TEXT("The requested inventory item does not exist."),
					false);
			}

			if (!Response.Error.HasError())
			{
				const EInventoryItemCategory Category = SelectedItem->Category;
				for (FInventoryItem& Item : MockInventory)
				{
					if (Item.Category == Category)
					{
						Item.bEquipped = Item.ItemId == ItemId;
					}
				}
				Response.bRequestSucceeded = true;
				Response.EquippedItemId = ItemId;
				bHasInventoryCache = false;
			}

			Completion.ExecuteIfBound(Response);
		});
}

FBackendRequestHandle UBackendServiceSubsystem::FetchProgression(
	const bool bForceRefresh,
	FOnProgressionCompleted Completion)
{
	const bool bUseCache = !bForceRefresh && bHasProgressionCache &&
		IsCacheValid(ProgressionCachedAtSeconds);

	if (bUseCache)
	{
		FProgressionResponse Response = ProgressionCache;
		Response.bFromCache = true;
		return ScheduleRequest(
			UIDemoBackend::CachedResponseDelaySeconds,
			[Completion = MoveTemp(Completion), Response = MoveTemp(Response)]() mutable
			{
				Completion.ExecuteIfBound(Response);
			});
	}

	const EBackendSimulationScenario Scenario = SimulationScenario;
	FProgressionResponse Response = BuildProgressionResponse(Scenario);

	return ScheduleRequest(
		GetDelayForScenario(Scenario),
		[this, Completion = MoveTemp(Completion), Response = MoveTemp(Response)]() mutable
		{
			if (Response.bRequestSucceeded)
			{
				ProgressionCache = Response;
				ProgressionCache.bFromCache = false;
				bHasProgressionCache = true;
				ProgressionCachedAtSeconds = FPlatformTime::Seconds();
			}
			Completion.ExecuteIfBound(Response);
		});
}

FBackendRequestHandle UBackendServiceSubsystem::UnlockSkill(
	const FName SkillId,
	FOnUnlockSkillCompleted Completion)
{
	const EBackendSimulationScenario Scenario = SimulationScenario;
	return ScheduleRequest(
		GetDelayForScenario(Scenario),
		[this, Scenario, SkillId, Completion = MoveTemp(Completion)]() mutable
		{
			FUnlockSkillResponse Response;
			Response.Error = UIDemoBackend::MakeScenarioError(
				EBackendEndpoint::UnlockSkill, Scenario, TEXT("unlock-skill"));

			if (!Response.Error.HasError() && Scenario == EBackendSimulationScenario::UnlockSkillFailed)
			{
				Response.Error = FBackendError::Make(
					EBackendEndpoint::UnlockSkill,
					EBackendErrorCode::RequestFailed,
					TEXT("The simulated unlock-skill request failed."),
					true);
			}

			FSkillNodeData* Node = MockProgression.Nodes.FindByPredicate(
				[SkillId](const FSkillNodeData& Candidate) { return Candidate.SkillId == SkillId; });

			if (!Response.Error.HasError() &&
				(Node == nullptr || Node->State != ESkillNodeState::Available))
			{
				Response.Error = FBackendError::Make(
					EBackendEndpoint::UnlockSkill,
					EBackendErrorCode::InvalidResponse,
					TEXT("The selected skill is not available."),
					false);
			}

			if (!Response.Error.HasError() &&
				(Scenario == EBackendSimulationScenario::InsufficientSkillPoints ||
				 MockProgression.SkillPoints < Node->Cost))
			{
				Response.Error = FBackendError::Make(
					EBackendEndpoint::UnlockSkill,
					EBackendErrorCode::RequestFailed,
					TEXT("The player does not have enough skill points."),
					false);
			}

			if (!Response.Error.HasError())
			{
				MockProgression.SkillPoints -= Node->Cost;
				MockPlayerProfile.SkillPoints = MockProgression.SkillPoints;
				Node->State = ESkillNodeState::Unlocked;
				RecalculateSkillAvailability();
				Response.bRequestSucceeded = true;
				Response.UnlockedSkillId = SkillId;
				Response.RemainingSkillPoints = MockProgression.SkillPoints;
				bHasProgressionCache = false;
			}

			Completion.ExecuteIfBound(Response);
		});
}

FBackendRequestHandle UBackendServiceSubsystem::StartMatchmaking(
	const FName PlaylistId,
	FOnMatchmakingCompleted Completion)
{
	const EBackendSimulationScenario Scenario = SimulationScenario;
	FMatchmakingResponse Response = BuildMatchmakingResponse(Scenario, PlaylistId);
	const float Delay = Scenario == EBackendSimulationScenario::Timeout
		? TimeoutDelaySeconds
		: FMath::Max(SimulatedLatencySeconds, UIDemoBackend::MatchmakingDelaySeconds);

	return ScheduleRequest(
		Delay,
		[Completion = MoveTemp(Completion), Response = MoveTemp(Response)]() mutable
		{
			Completion.ExecuteIfBound(Response);
		});
}

bool UBackendServiceSubsystem::CancelRequest(const FBackendRequestHandle& RequestHandle)
{
	if (!RequestHandle.IsValid())
	{
		return false;
	}

	FTimerHandle* TimerHandle = ActiveRequests.Find(RequestHandle.Id);
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

	const int32 CancelledCount = ActiveRequests.Num();
	ActiveRequests.Reset();
	UE_LOG(LogUIDemoBackend, Verbose, TEXT("Cancelled %d backend requests."), CancelledCount);
}

void UBackendServiceSubsystem::SetSimulationScenario(
	const EBackendSimulationScenario NewScenario)
{
	if (SimulationScenario != NewScenario)
	{
		ClearCachedData();
	}
	SimulationScenario = NewScenario;
	UE_LOG(LogUIDemoBackend, Log, TEXT("Simulation scenario: %s"),
		*UEnum::GetValueAsString(SimulationScenario));
}

EBackendSimulationScenario UBackendServiceSubsystem::GetSimulationScenario() const
{
	return SimulationScenario;
}

void UBackendServiceSubsystem::SetSimulatedLatencySeconds(const float NewLatencySeconds)
{
	SimulatedLatencySeconds = FMath::Clamp(
		NewLatencySeconds, 0.0f, UIDemoBackend::MaximumSimulatedDelaySeconds);
}

float UBackendServiceSubsystem::GetSimulatedLatencySeconds() const
{
	return SimulatedLatencySeconds;
}

void UBackendServiceSubsystem::SetTimeoutDelaySeconds(const float NewTimeoutDelaySeconds)
{
	TimeoutDelaySeconds = FMath::Clamp(
		NewTimeoutDelaySeconds, 0.0f, UIDemoBackend::MaximumSimulatedDelaySeconds);
}

float UBackendServiceSubsystem::GetTimeoutDelaySeconds() const
{
	return TimeoutDelaySeconds;
}

void UBackendServiceSubsystem::ResetSimulationSettings()
{
	SimulationScenario = EBackendSimulationScenario::Success;
	SimulatedLatencySeconds = UIDemoBackend::DefaultLatencySeconds;
	TimeoutDelaySeconds = UIDemoBackend::DefaultTimeoutDelaySeconds;
}

int32 UBackendServiceSubsystem::GetActiveRequestCount() const
{
	return ActiveRequests.Num();
}

void UBackendServiceSubsystem::ClearCachedData()
{
	bHasInventoryCache = false;
	InventoryCache = FInventoryResponse();
	InventoryCachedAtSeconds = 0.0;
	bHasProgressionCache = false;
	ProgressionCache = FProgressionResponse();
	ProgressionCachedAtSeconds = 0.0;
}

FBackendRequestHandle UBackendServiceSubsystem::ScheduleRequest(
	const float DelaySeconds,
	TFunction<void()>&& Completion)
{
	TRACE_CPUPROFILER_EVENT_SCOPE(UIDemo_Backend_ScheduleRequest);

	FBackendRequestHandle RequestHandle;
	if (!Completion)
	{
		return RequestHandle;
	}

	const UGameInstance* GameInstance = GetGameInstance();
	UWorld* World = GameInstance != nullptr ? GameInstance->GetWorld() : nullptr;
	if (World == nullptr)
	{
		UE_LOG(LogUIDemoBackend, Error, TEXT("Cannot schedule request without a world."));
		Completion();
		return RequestHandle;
	}

	RequestHandle.Id = NextRequestId++;
	const int32 RequestId = RequestHandle.Id;
	const float EffectiveDelay = FMath::Max(
		DelaySeconds, UIDemoBackend::MinimumTimerDelaySeconds);
	TWeakObjectPtr<UBackendServiceSubsystem> WeakThis(this);

	TFunction<void()> TimerCallback =
		[WeakThis, RequestId, Completion = MoveTemp(Completion)]() mutable
		{
			TRACE_CPUPROFILER_EVENT_SCOPE(UIDemo_Backend_CompleteRequest);
			if (UBackendServiceSubsystem* StrongThis = WeakThis.Get())
			{
				StrongThis->ActiveRequests.Remove(RequestId);
				UE_LOG(LogUIDemoBackend, Verbose, TEXT("Completed backend request %d."), RequestId);
				Completion();
			}
		};

	FTimerHandle TimerHandle;
	World->GetTimerManager().SetTimer(
		TimerHandle, MoveTemp(TimerCallback), EffectiveDelay, false, EffectiveDelay);
	ActiveRequests.Add(RequestId, TimerHandle);
	UE_LOG(LogUIDemoBackend, Verbose, TEXT("Scheduled backend request %d (%.3fs)."),
		RequestId, EffectiveDelay);
	return RequestHandle;
}

float UBackendServiceSubsystem::GetDelayForScenario(
	const EBackendSimulationScenario Scenario) const
{
	return Scenario == EBackendSimulationScenario::Timeout
		? TimeoutDelaySeconds
		: SimulatedLatencySeconds;
}

FBackendServiceStatusResponse UBackendServiceSubsystem::BuildServiceStatusResponse(
	const EBackendSimulationScenario Scenario) const
{
	FBackendServiceStatusResponse Response;
	Response.Environment = TEXT("Mock");
	Response.ServiceVersion = TEXT("1.0.0");

	if (Scenario == EBackendSimulationScenario::Timeout)
	{
		Response.Error = UIDemoBackend::MakeScenarioError(
			EBackendEndpoint::ServiceStatus, Scenario, TEXT("service-status"));
		return Response;
	}

	const bool bUnavailable = Scenario == EBackendSimulationScenario::ServiceUnavailable;
	Response.bRequestSucceeded = true;
	Response.OverallState = bUnavailable
		? EBackendServiceState::Unavailable
		: EBackendServiceState::Operational;
	const EBackendServiceState State = Response.OverallState;
	Response.Services.Add(UIDemoBackend::MakeServiceStatus(TEXT("Identity"), State));
	Response.Services.Add(UIDemoBackend::MakeServiceStatus(TEXT("PlayerData"), State));
	Response.Services.Add(UIDemoBackend::MakeServiceStatus(TEXT("Configuration"), State));
	Response.Services.Add(UIDemoBackend::MakeServiceStatus(TEXT("Matchmaking"), State));
	return Response;
}

FRemoteConfigResponse UBackendServiceSubsystem::BuildRemoteConfigResponse(
	const EBackendSimulationScenario Scenario) const
{
	FRemoteConfigResponse Response;
	Response.Error = UIDemoBackend::MakeScenarioError(
		EBackendEndpoint::RemoteConfig, Scenario, TEXT("remote-config"));

	if (!Response.Error.HasError() && Scenario == EBackendSimulationScenario::ConfigRequestFailed)
	{
		Response.Error = FBackendError::Make(
			EBackendEndpoint::RemoteConfig, EBackendErrorCode::RequestFailed,
			TEXT("The simulated remote-config request failed."), true);
	}

	if (!Response.Error.HasError())
	{
		Response.bRequestSucceeded = true;
		Response.Config.SeasonId = TEXT("season_01");
		Response.Config.SeasonDisplayName = TEXT("Season One");
		Response.Config.bIsMatchmakingEnabled = true;
		Response.Config.bIsInventoryEnabled = true;
		Response.Config.bIsProgressionEnabled = true;
		Response.Config.ProfileCacheTtlSeconds = 120;
	}
	return Response;
}

FPlayerProfileResponse UBackendServiceSubsystem::BuildPlayerProfileResponse(
	const EBackendSimulationScenario Scenario) const
{
	FPlayerProfileResponse Response;
	Response.Error = UIDemoBackend::MakeScenarioError(
		EBackendEndpoint::PlayerProfile, Scenario, TEXT("player-profile"));

	if (!Response.Error.HasError() && Scenario == EBackendSimulationScenario::ProfileRequestFailed)
	{
		Response.Error = FBackendError::Make(
			EBackendEndpoint::PlayerProfile, EBackendErrorCode::RequestFailed,
			TEXT("The simulated player-profile request failed."), true);
	}

	if (!Response.Error.HasError())
	{
		Response.bRequestSucceeded = true;
		Response.Profile = MockPlayerProfile;
	}
	return Response;
}

FInventoryResponse UBackendServiceSubsystem::BuildInventoryResponse(
	const EBackendSimulationScenario Scenario) const
{
	FInventoryResponse Response;
	Response.Error = UIDemoBackend::MakeScenarioError(
		EBackendEndpoint::Inventory, Scenario, TEXT("inventory"));

	if (!Response.Error.HasError() && Scenario == EBackendSimulationScenario::InventoryRequestFailed)
	{
		Response.Error = FBackendError::Make(
			EBackendEndpoint::Inventory, EBackendErrorCode::RequestFailed,
			TEXT("The simulated inventory request failed."), true);
	}

	if (!Response.Error.HasError())
	{
		Response.bRequestSucceeded = true;
		if (Scenario != EBackendSimulationScenario::InventoryEmpty)
		{
			Response.Items = MockInventory;
		}
	}
	return Response;
}

FProgressionResponse UBackendServiceSubsystem::BuildProgressionResponse(
	const EBackendSimulationScenario Scenario) const
{
	FProgressionResponse Response;
	Response.Error = UIDemoBackend::MakeScenarioError(
		EBackendEndpoint::Progression, Scenario, TEXT("progression"));

	if (!Response.Error.HasError() && Scenario == EBackendSimulationScenario::ProgressionRequestFailed)
	{
		Response.Error = FBackendError::Make(
			EBackendEndpoint::Progression, EBackendErrorCode::RequestFailed,
			TEXT("The simulated progression request failed."), true);
	}

	if (!Response.Error.HasError())
	{
		Response.bRequestSucceeded = true;
		Response.Progression = MockProgression;
	}
	return Response;
}

FMatchmakingResponse UBackendServiceSubsystem::BuildMatchmakingResponse(
	const EBackendSimulationScenario Scenario,
	const FName PlaylistId) const
{
	FMatchmakingResponse Response;
	Response.PlaylistId = PlaylistId;
	Response.EstimatedWaitSeconds = 3;
	Response.Error = UIDemoBackend::MakeScenarioError(
		EBackendEndpoint::Matchmaking, Scenario, TEXT("matchmaking"));

	if (!Response.Error.HasError() && Scenario == EBackendSimulationScenario::MatchmakingFailed)
	{
		Response.Error = FBackendError::Make(
			EBackendEndpoint::Matchmaking, EBackendErrorCode::RequestFailed,
			TEXT("The simulated matchmaking request failed."), true);
	}

	if (!Response.Error.HasError())
	{
		Response.bRequestSucceeded = true;
		Response.TicketId = TEXT("ticket_demo_001");
		Response.SessionId = TEXT("session_demo_001");
	}
	return Response;
}

void UBackendServiceSubsystem::InitializeMockData()
{
	MockPlayerProfile.PlayerId = TEXT("player_demo_001");
	MockPlayerProfile.DisplayName = TEXT("DemoPlayer");
	MockPlayerProfile.PlayerLevel = 24;
	MockPlayerProfile.CurrentExperience = 675;
	MockPlayerProfile.ExperienceForNextLevel = 1000;
	MockPlayerProfile.SoftCurrency = 12500;
	MockPlayerProfile.PremiumCurrency = 850;
	MockPlayerProfile.SkillPoints = 3;

	MockInventory.Reset();
	MockInventory.Add(UIDemoBackend::MakeItem(
		TEXT("pulse_rifle"), LOCTEXT("PulseRifle", "Pulse Rifle"),
		LOCTEXT("PulseRifleDescription", "Reliable mid-range energy rifle."),
		TEXT("weapon_pulse_rifle"), EInventoryItemCategory::Weapon,
		EInventoryItemRarity::Epic, 820, true));
	MockInventory.Add(UIDemoBackend::MakeItem(
		TEXT("arc_cannon"), LOCTEXT("ArcCannon", "Arc Cannon"),
		LOCTEXT("ArcCannonDescription", "Heavy weapon that chains damage between targets."),
		TEXT("weapon_arc_cannon"), EInventoryItemCategory::Weapon,
		EInventoryItemRarity::Legendary, 910, false));
	MockInventory.Add(UIDemoBackend::MakeItem(
		TEXT("vanguard_plate"), LOCTEXT("VanguardPlate", "Vanguard Plate"),
		LOCTEXT("VanguardPlateDescription", "Reinforced armor for frontline deployments."),
		TEXT("armor_vanguard"), EInventoryItemCategory::Armor,
		EInventoryItemRarity::Rare, 740, true));
	MockInventory.Add(UIDemoBackend::MakeItem(
		TEXT("ember_shader"), LOCTEXT("EmberShader", "Ember Shader"),
		LOCTEXT("EmberShaderDescription", "Animated cosmetic shader with an ember finish."),
		TEXT("cosmetic_ember"), EInventoryItemCategory::Cosmetic,
		EInventoryItemRarity::Epic, 0, false));

	MockProgression.SkillPoints = MockPlayerProfile.SkillPoints;
	MockProgression.Nodes.Reset();
	MockProgression.Nodes.Add(UIDemoBackend::MakeSkill(
		TEXT("combat_training"), LOCTEXT("CombatTraining", "Combat Training"),
		LOCTEXT("CombatTrainingDescription", "Unlocks the core combat specialization."),
		0, TArray<FName>(), 0, 1, ESkillNodeState::Unlocked));
	MockProgression.Nodes.Add(UIDemoBackend::MakeSkill(
		TEXT("quick_reload"), LOCTEXT("QuickReload", "Quick Reload"),
		LOCTEXT("QuickReloadDescription", "Reduces reload time by 10%."),
		1, TArray<FName>{TEXT("combat_training")}, 1, 0, ESkillNodeState::Available));
	MockProgression.Nodes.Add(UIDemoBackend::MakeSkill(
		TEXT("shield_surge"), LOCTEXT("ShieldSurge", "Shield Surge"),
		LOCTEXT("ShieldSurgeDescription", "Restores shields after a successful dodge."),
		2, TArray<FName>{TEXT("combat_training")}, 1, 2, ESkillNodeState::Available));
	MockProgression.Nodes.Add(UIDemoBackend::MakeSkill(
		TEXT("overcharge"), LOCTEXT("Overcharge", "Overcharge"),
		LOCTEXT("OverchargeDescription", "Temporarily increases weapon output."),
		3, TArray<FName>{TEXT("quick_reload"), TEXT("shield_surge")}, 2, 1, ESkillNodeState::Locked));
}

void UBackendServiceSubsystem::RecalculateSkillAvailability()
{
	TRACE_CPUPROFILER_EVENT_SCOPE(UIDemo_Backend_RecalculateSkillAvailability);

	for (FSkillNodeData& Node : MockProgression.Nodes)
	{
		if (Node.State == ESkillNodeState::Unlocked)
		{
			continue;
		}

		const bool bAllPrerequisitesUnlocked = Node.PrerequisiteIds.IsEmpty() ||
			Algo::AllOf(Node.PrerequisiteIds, [this](const FName PrerequisiteId)
			{
				const FSkillNodeData* Prerequisite = MockProgression.Nodes.FindByPredicate(
					[PrerequisiteId](const FSkillNodeData& Candidate)
					{
						return Candidate.SkillId == PrerequisiteId;
					});
				return Prerequisite != nullptr && Prerequisite->State == ESkillNodeState::Unlocked;
			});

		Node.State = bAllPrerequisitesUnlocked
			? ESkillNodeState::Available
			: ESkillNodeState::Locked;
	}
}

bool UBackendServiceSubsystem::IsCacheValid(const double CachedAtSeconds) const
{
	return FPlatformTime::Seconds() - CachedAtSeconds <= UIDemoBackend::CacheTtlSeconds;
}

#undef LOCTEXT_NAMESPACE
