#pragma once

#include "CoreMinimal.h"
#include "Backend/BackendTypes.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "FrontendSessionSubsystem.generated.h"

/** Broadcast when shared frontend session data changes. */
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnFrontendSessionChanged);

/**
 * @brief Runtime store for account data shared by frontend screens.
 *
 * The bootstrap ViewModel writes the initial snapshot. Feature ViewModels read
 * and update it without depending on each other or on any widget instance.
 */
UCLASS()
class UIDEMO_API UFrontendSessionSubsystem final : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	/**
	 * @brief Initializes the shared frontend session from bootstrap data.
	 * @param InProfile Player profile returned during bootstrap.
	 * @param InConfig Remote configuration returned during bootstrap.
	 */
	UFUNCTION(BlueprintCallable, Category = "UIDemo|Session")
	void InitializeSession(const FPlayerProfile& InProfile, const FRemoteConfig& InConfig);

	/** @brief Clears all shared frontend session data. */
	UFUNCTION(BlueprintCallable, Category = "UIDemo|Session")
	void ResetSession();

	/**
	 * @brief Reports whether bootstrap has initialized this session.
	 * @return True when the shared profile and configuration are ready.
	 */
	UFUNCTION(BlueprintPure, Category = "UIDemo|Session")
	bool IsSessionReady() const { return bSessionReady; }

	/**
	 * @brief Returns the current player-profile snapshot.
	 * @return Copy of the profile stored in the session.
	 */
	UFUNCTION(BlueprintPure, Category = "UIDemo|Session")
	FPlayerProfile GetPlayerProfile() const { return PlayerProfile; }

	/**
	 * @brief Returns the current remote-configuration snapshot.
	 * @return Copy of the configuration stored in the session.
	 */
	UFUNCTION(BlueprintPure, Category = "UIDemo|Session")
	FRemoteConfig GetRemoteConfig() const { return RemoteConfig; }

	/**
	 * @brief Updates the identifier of the item equipped by the frontend.
	 * @param ItemId Stable identifier of the equipped item.
	 */
	UFUNCTION(BlueprintCallable, Category = "UIDemo|Session")
	void SetEquippedItemId(FName ItemId);

	/**
	 * @brief Returns the identifier of the item equipped through the frontend.
	 * @return Equipped item identifier, or NAME_None when no item is tracked.
	 */
	UFUNCTION(BlueprintPure, Category = "UIDemo|Session")
	FName GetEquippedItemId() const { return EquippedItemId; }

	/**
	 * @brief Updates the skill-point balance shared with the home screen.
	 * @param NewSkillPoints New non-negative balance.
	 */
	UFUNCTION(BlueprintCallable, Category = "UIDemo|Session")
	void SetSkillPoints(int32 NewSkillPoints);

	/** Event emitted after any session field changes. */
	UPROPERTY(BlueprintAssignable, Category = "UIDemo|Session")
	FOnFrontendSessionChanged OnSessionChanged;

private:
	/** Whether bootstrap has produced a valid session snapshot. */
	UPROPERTY(Transient)
	bool bSessionReady = false;

	/** Player profile shared by feature ViewModels. */
	UPROPERTY(Transient)
	FPlayerProfile PlayerProfile;

	/** Remote configuration shared by feature ViewModels. */
	UPROPERTY(Transient)
	FRemoteConfig RemoteConfig;

	/** Item equipped during the current frontend session. */
	UPROPERTY(Transient)
	FName EquippedItemId = NAME_None;
};
