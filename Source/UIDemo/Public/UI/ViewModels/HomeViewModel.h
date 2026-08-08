#pragma once

#include "CoreMinimal.h"
#include "Backend/BackendTypes.h"
#include "MVVMViewModelBase.h"
#include "HomeViewModel.generated.h"

class UFrontendSessionSubsystem;

/**
 * @brief Presentation model for the frontend home screen.
 *
 * The ViewModel mirrors shared session data and reacts to mutations performed
 * by other feature ViewModels without holding references to any widgets.
 */
UCLASS(BlueprintType)
class UIDEMO_API UHomeViewModel final : public UMVVMViewModelBase
{
	GENERATED_BODY()

public:
	/**
	 * @brief Resolves and subscribes to the shared frontend session.
	 * @param WorldContextObject Object used to locate the owning game instance.
	 * @return True when the session subsystem was resolved successfully.
	 */
	UFUNCTION(BlueprintCallable, Category = "UIDemo|Home")
	bool Initialize(UObject* WorldContextObject);

	/** @brief Copies the latest shared session values into FieldNotify properties. */
	UFUNCTION(BlueprintCallable, Category = "UIDemo|Home")
	void Refresh();

protected:
	/** @brief Removes the session event subscription before destruction. */
	virtual void BeginDestroy() override;

private:
	/** @brief Refreshes the presentation snapshot after a session mutation. */
	UFUNCTION()
	void HandleSessionChanged();

	/** Shared frontend session observed by this ViewModel. */
	UPROPERTY(Transient)
	TObjectPtr<UFrontendSessionSubsystem> SessionSubsystem;

	/** Player profile displayed by the home screen. */
	UPROPERTY(BlueprintReadOnly, FieldNotify, Category = "UIDemo|Home", meta = (AllowPrivateAccess = "true"))
	FPlayerProfile PlayerProfile;

	/** Remote feature configuration displayed by the home screen. */
	UPROPERTY(BlueprintReadOnly, FieldNotify, Category = "UIDemo|Home", meta = (AllowPrivateAccess = "true"))
	FRemoteConfig RemoteConfig;

	/** Normalized current-level experience in the range [0, 1]. */
	UPROPERTY(BlueprintReadOnly, FieldNotify, Category = "UIDemo|Home", meta = (AllowPrivateAccess = "true"))
	float LevelProgress = 0.0f;
};
