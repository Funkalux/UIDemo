#pragma once

#include "CoreMinimal.h"
#include "Backend/BackendTypes.h"
#include "MVVMViewModelBase.h"
#include "BackendDebugViewModel.generated.h"

class UBackendServiceSubsystem;

UCLASS(BlueprintType)
class UIDEMO_API UBackendDebugViewModel final : public UMVVMViewModelBase
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "UIDemo|Debug")
	bool Initialize(UObject* WorldContextObject);
	UFUNCTION(BlueprintCallable, Category = "UIDemo|Debug")
	void ApplyScenario(EBackendSimulationScenario NewScenario);
	UFUNCTION(BlueprintCallable, Category = "UIDemo|Debug")
	void ApplyLatency(float NewLatencySeconds);
	UFUNCTION(BlueprintCallable, Category = "UIDemo|Debug")
	void ResetSimulation();
	UFUNCTION(BlueprintCallable, Category = "UIDemo|Debug")
	void ClearCache();
	UFUNCTION(BlueprintCallable, Category = "UIDemo|Debug")
	void Refresh();

private:
	UPROPERTY(Transient)
	TObjectPtr<UBackendServiceSubsystem> BackendSubsystem;
	UPROPERTY(BlueprintReadOnly, FieldNotify, Category = "UIDemo|Debug", meta = (AllowPrivateAccess = "true"))
	EBackendSimulationScenario Scenario = EBackendSimulationScenario::Success;
	UPROPERTY(BlueprintReadOnly, FieldNotify, Category = "UIDemo|Debug", meta = (AllowPrivateAccess = "true"))
	float LatencySeconds = 0.75f;
	UPROPERTY(BlueprintReadOnly, FieldNotify, Category = "UIDemo|Debug", meta = (AllowPrivateAccess = "true"))
	int32 ActiveRequestCount = 0;
};
