#pragma once

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"
#include "Input/UIActionBindingHandle.h"
#include "MenuScreenBase.generated.h"

class UWidget;

/**
 * @brief CommonUI base for every navigable full-screen view and modal.
 *
 * The base applies a menu-only input configuration and provides an optional
 * BindWidget contract for deterministic initial focus.
 */
UCLASS(Abstract, Blueprintable)
class UIDEMO_API UMenuScreenBase : public UCommonActivatableWidget
{
	GENERATED_BODY()

protected:
	/**
	 * @brief Returns the menu input configuration used while the screen is active.
	 * @return UI-only input configuration with no mouse capture.
	 */
	virtual TOptional<FUIInputConfig> GetDesiredInputConfig() const override;

	/**
	 * @brief Resolves the widget that should receive focus after activation.
	 * @return InitialFocusTarget when bound, otherwise the CommonUI fallback.
	 */
	virtual UWidget* NativeGetDesiredFocusTarget() const override;

	/** Optional focus target supplied by a derived Widget Blueprint. */
	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional), Category = "UIDemo|Navigation")
	TObjectPtr<UWidget> InitialFocusTarget;
};
