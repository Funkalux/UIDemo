#include "UI/Common/MenuScreenBase.h"

#include "Components/Widget.h"

TOptional<FUIInputConfig> UMenuScreenBase::GetDesiredInputConfig() const
{
	return FUIInputConfig(
		ECommonInputMode::Menu,
		EMouseCaptureMode::NoCapture,
		false);
}

UWidget* UMenuScreenBase::NativeGetDesiredFocusTarget() const
{
	return IsValid(InitialFocusTarget)
		? InitialFocusTarget.Get()
		: Super::NativeGetDesiredFocusTarget();
}
