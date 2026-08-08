#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"
#include "FrontendRootLayout.generated.h"

class UCommonActivatableWidget;
class UCommonActivatableWidgetStack;

/**
 * @brief Root UI layer that owns screen and modal navigation stacks.
 *
 * Screen and modal stacks keep navigation/back behavior out of
 * individual widgets and provide one predictable ownership point.
 */
UCLASS(Abstract, Blueprintable)
class UIDEMO_API UFrontendRootLayout : public UCommonUserWidget
{
	GENERATED_BODY()

public:
	/**
	 * @brief Pushes a full-screen activatable widget onto the screen layer.
	 * @param ScreenClass Widget class to instantiate and push.
	 * @return Created widget instance, or nullptr when the request is invalid.
	 */
	UFUNCTION(BlueprintCallable, Category = "UIDemo|Navigation")
	UCommonActivatableWidget* PushScreen(
		TSubclassOf<UCommonActivatableWidget> ScreenClass);

	/**
	 * @brief Pushes an activatable widget onto the modal layer.
	 * @param ModalClass Widget class to instantiate and push.
	 * @return Created widget instance, or nullptr when the request is invalid.
	 */
	UFUNCTION(BlueprintCallable, Category = "UIDemo|Navigation")
	UCommonActivatableWidget* PushModal(
		TSubclassOf<UCommonActivatableWidget> ModalClass);

protected:
	/** Primary stack containing full-screen frontend views. */
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "UIDemo|Navigation")
	TObjectPtr<UCommonActivatableWidgetStack> ScreenStack;

	/** Overlay stack containing modal dialogs above the active screen. */
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "UIDemo|Navigation")
	TObjectPtr<UCommonActivatableWidgetStack> ModalStack;
};
