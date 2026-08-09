#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "UIDemoPlayerController.generated.h"

class UCommonActivatableWidget;
class UFrontendRootLayout;

/**
 * @brief Local player controller responsible for creating the root UI layout.
 *
 * The controller owns the root widget for the lifetime of the local player and
 * pushes the configured initial screen onto its primary navigation stack.
 */
UCLASS(Blueprintable)
class UIDEMO_API AUIDemoPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	/**
	 * @brief Returns the root layout created for this local player.
	 * @return Root layout instance, or nullptr before BeginPlay completes.
	 */
	UFUNCTION(BlueprintPure, Category = "UIDemo|UI")
	UFrontendRootLayout* GetRootLayout() const
	{
		return RootLayout;
	}

	/**
	 * @brief Pushes the configured initial screen onto the primary screen stack.
	 *
	 * This function can be used to restart the frontend bootstrap flow without
	 * duplicating the initial-screen class reference in Widget Blueprints.
	 *
	 * @return Created initial-screen instance, or nullptr when unavailable.
	 */
	UFUNCTION(BlueprintCallable, Category = "UIDemo|UI")
	UCommonActivatableWidget* PushInitialScreen();

protected:
	/** @brief Creates the configured root layout for local controllers. */
	virtual void BeginPlay() override;

	/** Widget Blueprint class used as the root screen and modal layer. */
	UPROPERTY(EditDefaultsOnly, Category = "UIDemo|UI")
	TSubclassOf<UFrontendRootLayout> RootLayoutClass;

	/** Activatable screen pushed when the frontend bootstrap flow begins. */
	UPROPERTY(EditDefaultsOnly, Category = "UIDemo|UI")
	TSubclassOf<UCommonActivatableWidget> InitialScreenClass;

private:
	/** Root layout instance owned by this local player controller. */
	UPROPERTY(Transient)
	TObjectPtr<UFrontendRootLayout> RootLayout;
};