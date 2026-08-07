#include "UI/Common/FrontendRootLayout.h"

#include "CommonActivatableWidget.h"
#include "Widgets/CommonActivatableWidgetContainer.h"

UCommonActivatableWidget* UFrontendRootLayout::PushScreen(
	const TSubclassOf<UCommonActivatableWidget> ScreenClass)
{
	return IsValid(ScreenStack) && ScreenClass
		? ScreenStack->AddWidget<UCommonActivatableWidget>(ScreenClass)
		: nullptr;
}

UCommonActivatableWidget* UFrontendRootLayout::PushModal(
	const TSubclassOf<UCommonActivatableWidget> ModalClass)
{
	return IsValid(ModalStack) && ModalClass
		? ModalStack->AddWidget<UCommonActivatableWidget>(ModalClass)
		: nullptr;
}
