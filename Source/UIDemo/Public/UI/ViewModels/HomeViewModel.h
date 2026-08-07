#pragma once

#include "CoreMinimal.h"
#include "Backend/BackendTypes.h"
#include "MVVMViewModelBase.h"
#include "HomeViewModel.generated.h"

class UFrontendSessionSubsystem;

UCLASS(BlueprintType)
class UIDEMO_API UHomeViewModel final : public UMVVMViewModelBase
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "UIDemo|Home")
	bool Initialize(UObject* WorldContextObject);
	UFUNCTION(BlueprintCallable, Category = "UIDemo|Home")
	void Refresh();

protected:
	virtual void BeginDestroy() override;

private:
	UFUNCTION()
	void HandleSessionChanged();

	UPROPERTY(Transient)
	TObjectPtr<UFrontendSessionSubsystem> SessionSubsystem;
	UPROPERTY(BlueprintReadOnly, FieldNotify, Category = "UIDemo|Home", meta = (AllowPrivateAccess = "true"))
	FPlayerProfile PlayerProfile;
	UPROPERTY(BlueprintReadOnly, FieldNotify, Category = "UIDemo|Home", meta = (AllowPrivateAccess = "true"))
	FRemoteConfig RemoteConfig;
	UPROPERTY(BlueprintReadOnly, FieldNotify, Category = "UIDemo|Home", meta = (AllowPrivateAccess = "true"))
	float LevelProgress = 0.0f;
};
