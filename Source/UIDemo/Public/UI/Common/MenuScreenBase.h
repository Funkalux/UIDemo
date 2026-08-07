#pragma once

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"
#include "Input/UIActionBindingHandle.h"
#include "MenuScreenBase.generated.h"

class UWidget;

/** CommonUI base for every navigable full-screen view and modal. */
UCLASS(Abstract, Blueprintable)
class UIDEMO_API UMenuScreenBase : public UCommonActivatableWidget
{
	GENERATED_BODY()

protected:
	virtual TOptional<FUIInputConfig> GetDesiredInputConfig() const override;
	virtual UWidget* NativeGetDesiredFocusTarget() const override;

	/** Optional named widget supplied by a Widget Blueprint. */
	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional), Category = "UIDemo|Navigation")
	TObjectPtr<UWidget> InitialFocusTarget;
};
