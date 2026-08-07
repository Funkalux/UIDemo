#include "Game/UIDemoGameMode.h"

#include "Game/UIDemoPlayerController.h"

AUIDemoGameMode::AUIDemoGameMode()
{
	PlayerControllerClass = AUIDemoPlayerController::StaticClass();
	DefaultPawnClass = nullptr;
	HUDClass = nullptr;
}
