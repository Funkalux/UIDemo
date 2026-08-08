#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "UIDemoGameMode.generated.h"

/**
 * @brief Game mode used by the standalone frontend demonstration map.
 *
 * The native constructor configures the project-specific player controller and
 * disables gameplay framework classes that are unnecessary for a menu-only map.
 */
UCLASS(Blueprintable)
class UIDEMO_API AUIDemoGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	/** @brief Creates the frontend game mode and applies its native defaults. */
	AUIDemoGameMode();
};
