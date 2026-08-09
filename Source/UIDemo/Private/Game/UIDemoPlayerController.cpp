#include "Game/UIDemoPlayerController.h"

#include "Blueprint/UserWidget.h"
#include "CommonActivatableWidget.h"
#include "UI/Common/FrontendRootLayout.h"

void AUIDemoPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (!IsLocalController() || !RootLayoutClass)
	{
		return;
	}

	RootLayout = CreateWidget<UFrontendRootLayout>(
		this,
		RootLayoutClass);

	if (!IsValid(RootLayout))
	{
		return;
	}

	RootLayout->AddToViewport(100);
	PushInitialScreen();
}

UCommonActivatableWidget*
AUIDemoPlayerController::PushInitialScreen()
{
	if (!IsValid(RootLayout) || !InitialScreenClass)
	{
		return nullptr;
	}

	return RootLayout->PushScreen(InitialScreenClass);
}