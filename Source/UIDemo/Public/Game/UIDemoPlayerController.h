#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "UIDemoPlayerController.generated.h"

class UCommonActivatableWidget;
class UFrontendRootLayout;

UCLASS(Blueprintable)
class UIDEMO_API AUIDemoPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, Category = "UIDemo|UI")
	UFrontendRootLayout* GetRootLayout() const { return RootLayout; }

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, Category = "UIDemo|UI")
	TSubclassOf<UFrontendRootLayout> RootLayoutClass;

	UPROPERTY(EditDefaultsOnly, Category = "UIDemo|UI")
	TSubclassOf<UCommonActivatableWidget> InitialScreenClass;

private:
	UPROPERTY(Transient)
	TObjectPtr<UFrontendRootLayout> RootLayout;
};
