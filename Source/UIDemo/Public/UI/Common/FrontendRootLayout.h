#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"
#include "FrontendRootLayout.generated.h"

class UCommonActivatableWidget;
class UCommonActivatableWidgetStack;

/**
 * Root UI layer. Screen and modal stacks keep navigation/back behavior out of
 * individual widgets and provide one predictable ownership point.
 */
UCLASS(Abstract, Blueprintable)
class UIDEMO_API UFrontendRootLayout : public UCommonUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "UIDemo|Navigation")
	UCommonActivatableWidget* PushScreen(
		TSubclassOf<UCommonActivatableWidget> ScreenClass);

	UFUNCTION(BlueprintCallable, Category = "UIDemo|Navigation")
	UCommonActivatableWidget* PushModal(
		TSubclassOf<UCommonActivatableWidget> ModalClass);

protected:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "UIDemo|Navigation")
	TObjectPtr<UCommonActivatableWidgetStack> ScreenStack;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "UIDemo|Navigation")
	TObjectPtr<UCommonActivatableWidgetStack> ModalStack;
};
