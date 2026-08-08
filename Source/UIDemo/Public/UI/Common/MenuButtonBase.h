#pragma once

#include "CoreMinimal.h"
#include "CommonButtonBase.h"
#include "MenuButtonBase.generated.h"

class URichTextBlock;

/**
 * @brief Native base class for reusable frontend menu buttons.
 *
 * Derived Widget Blueprints own layout, styling, and animation. This class
 * defines the reusable label API and enforces the required Rich Text widget
 * contract without introducing per-frame bindings.
 */
UCLASS(Abstract, Blueprintable)
class UIDEMO_API UMenuButtonBase : public UCommonButtonBase
{
	GENERATED_BODY()

public:
	/**
	 * @brief Updates the text displayed by the button.
	 *
	 * @param InLabelText Localized text to display.
	 */
	UFUNCTION(BlueprintCallable, Category = "UIDemo|Button")
	void SetLabelText(const FText& InLabelText);

	/**
	 * @brief Returns the text currently configured for the button.
	 *
	 * @return The localized button label.
	 */
	UFUNCTION(BlueprintPure, Category = "UIDemo|Button")
	FText GetLabelText() const { return LabelText; }

protected:
	/**
	 * @brief Synchronizes editable label data with the designer widget tree.
	 */
	virtual void NativePreConstruct() override;

	/** Localized label editable on each Widget Blueprint instance. */
	UPROPERTY(
		EditAnywhere,
		BlueprintReadOnly,
		Category = "UIDemo|Button",
		meta = (ExposeOnSpawn = "true"))
	FText LabelText;

	/** Required Rich Text widget supplied by the derived Widget Blueprint. */
	UPROPERTY(
		BlueprintReadOnly,
		meta = (BindWidget),
		Category = "UIDemo|Button")
	TObjectPtr<URichTextBlock> ButtonLabel;

private:
	/** Applies the stored label to the bound Rich Text widget when available. */
	void RefreshLabel();
};
