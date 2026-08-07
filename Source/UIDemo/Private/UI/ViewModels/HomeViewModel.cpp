#include "UI/ViewModels/HomeViewModel.h"

#include "Engine/GameInstance.h"
#include "Engine/World.h"
#include "Frontend/FrontendSessionSubsystem.h"

bool UHomeViewModel::Initialize(UObject* WorldContextObject)
{
	if (IsValid(SessionSubsystem))
	{
		SessionSubsystem->OnSessionChanged.RemoveDynamic(this, &UHomeViewModel::HandleSessionChanged);
	}

	UWorld* World = IsValid(WorldContextObject) ? WorldContextObject->GetWorld() : nullptr;
	UGameInstance* GameInstance = World != nullptr ? World->GetGameInstance() : nullptr;
	SessionSubsystem = GameInstance != nullptr
		? GameInstance->GetSubsystem<UFrontendSessionSubsystem>() : nullptr;
	if (!IsValid(SessionSubsystem))
	{
		return false;
	}

	SessionSubsystem->OnSessionChanged.AddUniqueDynamic(this, &UHomeViewModel::HandleSessionChanged);
	Refresh();
	return true;
}

void UHomeViewModel::Refresh()
{
	if (!IsValid(SessionSubsystem))
	{
		return;
	}
	PlayerProfile = SessionSubsystem->GetPlayerProfile();
	RemoteConfig = SessionSubsystem->GetRemoteConfig();
	LevelProgress = PlayerProfile.ExperienceForNextLevel > 0
		? static_cast<float>(PlayerProfile.CurrentExperience) /
			static_cast<float>(PlayerProfile.ExperienceForNextLevel)
		: 0.0f;
	LevelProgress = FMath::Clamp(LevelProgress, 0.0f, 1.0f);
	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(PlayerProfile);
	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(RemoteConfig);
	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(LevelProgress);
}

void UHomeViewModel::BeginDestroy()
{
	if (IsValid(SessionSubsystem))
	{
		SessionSubsystem->OnSessionChanged.RemoveDynamic(this, &UHomeViewModel::HandleSessionChanged);
	}
	Super::BeginDestroy();
}

void UHomeViewModel::HandleSessionChanged()
{
	Refresh();
}
