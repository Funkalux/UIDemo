#include "Frontend/FrontendSessionSubsystem.h"

void UFrontendSessionSubsystem::InitializeSession(
	const FPlayerProfile& InProfile,
	const FRemoteConfig& InConfig)
{
	PlayerProfile = InProfile;
	RemoteConfig = InConfig;
	EquippedItemId = NAME_None;
	bSessionReady = true;
	OnSessionChanged.Broadcast();
}

void UFrontendSessionSubsystem::ResetSession()
{
	bSessionReady = false;
	PlayerProfile = FPlayerProfile();
	RemoteConfig = FRemoteConfig();
	EquippedItemId = NAME_None;
	OnSessionChanged.Broadcast();
}

void UFrontendSessionSubsystem::SetEquippedItemId(const FName ItemId)
{
	if (EquippedItemId != ItemId)
	{
		EquippedItemId = ItemId;
		OnSessionChanged.Broadcast();
	}
}

void UFrontendSessionSubsystem::SetSkillPoints(const int32 NewSkillPoints)
{
	const int32 ClampedPoints = FMath::Max(0, NewSkillPoints);
	if (PlayerProfile.SkillPoints != ClampedPoints)
	{
		PlayerProfile.SkillPoints = ClampedPoints;
		OnSessionChanged.Broadcast();
	}
}
