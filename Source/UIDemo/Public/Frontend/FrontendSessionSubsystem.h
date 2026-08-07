#pragma once

#include "CoreMinimal.h"
#include "Backend/BackendTypes.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "FrontendSessionSubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnFrontendSessionChanged);

/**
 * Runtime store for account data shared by frontend screens.
 *
 * The bootstrap ViewModel writes the initial snapshot. Feature ViewModels read
 * and update it without depending on each other or on any widget instance.
 */
UCLASS()
class UIDEMO_API UFrontendSessionSubsystem final : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "UIDemo|Session")
	void InitializeSession(const FPlayerProfile& InProfile, const FRemoteConfig& InConfig);

	UFUNCTION(BlueprintCallable, Category = "UIDemo|Session")
	void ResetSession();

	UFUNCTION(BlueprintPure, Category = "UIDemo|Session")
	bool IsSessionReady() const { return bSessionReady; }

	UFUNCTION(BlueprintPure, Category = "UIDemo|Session")
	FPlayerProfile GetPlayerProfile() const { return PlayerProfile; }

	UFUNCTION(BlueprintPure, Category = "UIDemo|Session")
	FRemoteConfig GetRemoteConfig() const { return RemoteConfig; }

	UFUNCTION(BlueprintCallable, Category = "UIDemo|Session")
	void SetEquippedItemId(FName ItemId);

	UFUNCTION(BlueprintPure, Category = "UIDemo|Session")
	FName GetEquippedItemId() const { return EquippedItemId; }

	UFUNCTION(BlueprintCallable, Category = "UIDemo|Session")
	void SetSkillPoints(int32 NewSkillPoints);

	UPROPERTY(BlueprintAssignable, Category = "UIDemo|Session")
	FOnFrontendSessionChanged OnSessionChanged;

private:
	UPROPERTY(Transient)
	bool bSessionReady = false;

	UPROPERTY(Transient)
	FPlayerProfile PlayerProfile;

	UPROPERTY(Transient)
	FRemoteConfig RemoteConfig;

	UPROPERTY(Transient)
	FName EquippedItemId = NAME_None;
};
