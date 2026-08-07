#pragma once

#include "CoreMinimal.h"
#include "Backend/BackendServiceSubsystem.h"
#include "MVVMViewModelBase.h"
#include "UI/ViewModels/ViewModelTypes.h"
#include "InventoryViewModel.generated.h"

class UFrontendSessionSubsystem;
class UInventoryItemViewModel;

UCLASS(BlueprintType)
class UIDEMO_API UInventoryViewModel final : public UMVVMViewModelBase
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "UIDemo|Inventory")
	bool Initialize(UObject* WorldContextObject);

	UFUNCTION(BlueprintCallable, Category = "UIDemo|Inventory")
	void LoadInventory(bool bForceRefresh = false);

	UFUNCTION(BlueprintCallable, Category = "UIDemo|Inventory")
	void Retry();

	UFUNCTION(BlueprintCallable, Category = "UIDemo|Inventory")
	void SelectItem(UInventoryItemViewModel* Item);

	UFUNCTION(BlueprintCallable, Category = "UIDemo|Inventory")
	void EquipSelectedItem();

	UFUNCTION(BlueprintCallable, Category = "UIDemo|Inventory")
	void Cancel();

protected:
	virtual void BeginDestroy() override;

private:
	void HandleInventoryResponse(const FInventoryResponse& Response, int32 AttemptId);
	void HandleEquipResponse(const FEquipItemResponse& Response, int32 AttemptId);
	void RebuildItems(const TArray<FInventoryItem>& SourceItems);
	void SetState(EAsyncViewState NewState);
	void SetError(const FText& NewError);
	void UpdateCanEquip();
	void CancelRequests();

private:
	UPROPERTY(Transient)
	TObjectPtr<UBackendServiceSubsystem> BackendSubsystem;

	UPROPERTY(Transient)
	TObjectPtr<UFrontendSessionSubsystem> SessionSubsystem;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Category = "UIDemo|Inventory", meta = (AllowPrivateAccess = "true"))
	EAsyncViewState State = EAsyncViewState::Idle;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Category = "UIDemo|Inventory", meta = (AllowPrivateAccess = "true"))
	TArray<TObjectPtr<UInventoryItemViewModel>> Items;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Category = "UIDemo|Inventory", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInventoryItemViewModel> SelectedItem;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Category = "UIDemo|Inventory", meta = (AllowPrivateAccess = "true"))
	FText ErrorText;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Category = "UIDemo|Inventory", meta = (AllowPrivateAccess = "true"))
	bool bIsBusy = false;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Category = "UIDemo|Inventory", meta = (AllowPrivateAccess = "true"))
	bool bFromCache = false;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Category = "UIDemo|Inventory", meta = (AllowPrivateAccess = "true"))
	bool bCanEquip = false;

	FBackendRequestHandle LoadRequest;
	FBackendRequestHandle EquipRequest;
	int32 ActiveAttemptId = 0;
};
