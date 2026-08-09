#include "UI/ViewModels/BackendDebugViewModel.h"

#include "Backend/BackendServiceSubsystem.h"
#include "Engine/GameInstance.h"
#include "Engine/World.h"

bool UBackendDebugViewModel::Initialize(
	UObject* WorldContextObject)
{
	UWorld* World = IsValid(WorldContextObject)
		? WorldContextObject->GetWorld()
		: nullptr;

	UGameInstance* GameInstance = World != nullptr
		? World->GetGameInstance()
		: nullptr;

	BackendSubsystem = GameInstance != nullptr
		? GameInstance->GetSubsystem<UBackendServiceSubsystem>()
		: nullptr;

	Refresh();
	return IsValid(BackendSubsystem);
}

void UBackendDebugViewModel::ApplyScenario(
	const EBackendSimulationScenario NewScenario)
{
	if (IsValid(BackendSubsystem))
	{
		BackendSubsystem->SetSimulationScenario(NewScenario);
		Refresh();
	}
}

void UBackendDebugViewModel::ApplyLatency(
	const float NewLatencySeconds)
{
	if (IsValid(BackendSubsystem))
	{
		BackendSubsystem->SetSimulatedLatencySeconds(
			NewLatencySeconds);

		Refresh();
	}
}

void UBackendDebugViewModel::ResetSimulation()
{
	if (IsValid(BackendSubsystem))
	{
		BackendSubsystem->ResetSimulationSettings();
		Refresh();
	}
}

void UBackendDebugViewModel::ClearCache()
{
	if (IsValid(BackendSubsystem))
	{
		BackendSubsystem->ClearCachedData();
		Refresh();
	}
}

void UBackendDebugViewModel::Refresh()
{
	if (!IsValid(BackendSubsystem))
	{
		return;
	}

	const EBackendSimulationScenario NewScenario =
		BackendSubsystem->GetSimulationScenario();

	if (Scenario != NewScenario)
	{
		Scenario = NewScenario;
		UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(Scenario);
	}

	const float NewLatencySeconds =
		BackendSubsystem->GetSimulatedLatencySeconds();

	if (!FMath::IsNearlyEqual(
		LatencySeconds,
		NewLatencySeconds))
	{
		LatencySeconds = NewLatencySeconds;
		UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(
			LatencySeconds);
	}

	const int32 NewActiveRequestCount =
		BackendSubsystem->GetActiveRequestCount();

	if (ActiveRequestCount != NewActiveRequestCount)
	{
		ActiveRequestCount = NewActiveRequestCount;
		UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(
			ActiveRequestCount);
	}
}