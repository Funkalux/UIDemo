#pragma once

#include "CoreMinimal.h"
#include "Backend/BackendServiceSubsystem.h"
#include "MVVMViewModelBase.h"
#include "UI/ViewModels/ViewModelTypes.h"
#include "InventoryViewModel.generated.h"

class UFrontendSessionSubsystem;
class UInventoryItemViewModel;

/**
 * @brief Screen-level presentation model for inventory browsing and equipment.
 *
 * The ViewModel owns asynchronous request state and item ViewModels while the
 * Widget Blueprint owns list presentation, selection input, and navigation.
 */
UCLASS(BlueprintType)
class UIDEMO_API UInventoryViewModel final : public UMVVMViewModelBase
{
	GENERATED_BODY()

public:
	/**
	 * @brief Resolves backend and shared-session dependencies.
	 * @param WorldContextObject Object used to locate the owning game instance.
	 * @return True when every required subsystem was resolved successfully.
	 */
	UFUNCTION(BlueprintCallable, Category = "UIDemo|Inventory")
	bool Initialize(UObject* WorldContextObject);

	/**
	 * @brief Starts an asynchronous inventory query.
	 * @param bForceRefresh True to bypass a valid cached response.
	 */
	UFUNCTION(BlueprintCallable, Category = "UIDemo|Inventory")
	void LoadInventory(bool bForceRefresh = false);

	/** @brief Retries a failed inventory load while bypassing cache. */
	UFUNCTION(BlueprintCallable, Category = "UIDemo|Inventory")
	void Retry();

	/**
	 * @brief Changes the item selected by the inventory screen.
	 * @param Item Item ViewModel selected by the list, or nullptr to clear selection.
	 */
	UFUNCTION(BlueprintCallable, Category = "UIDemo|Inventory")
	void SelectItem(UInventoryItemViewModel* Item);

	/** @brief Starts an equipment mutation for the selected item when permitted. */
	UFUNCTION(BlueprintCallable, Category = "UIDemo|Inventory")
	void EquipSelectedItem();

	/** @brief Cancels pending work and returns to the best available stable state. */
	UFUNCTION(BlueprintCallable, Category = "UIDemo|Inventory")
	void Cancel();

protected:
	/** @brief Cancels pending requests before the ViewModel is destroyed. */
	virtual void BeginDestroy() override;

private:
	/**
	 * @brief Processes an inventory response that belongs to the active attempt.
	 * @param Response Response returned by the backend facade.
	 * @param AttemptId Attempt identifier captured when the request started.
	 */
	void HandleInventoryResponse(const FInventoryResponse& Response, int32 AttemptId);
	/**
	 * @brief Processes an equipment response that belongs to the active attempt.
	 * @param Response Response returned by the backend facade.
	 * @param AttemptId Attempt identifier captured when the request started.
	 */
	void HandleEquipResponse(const FEquipItemResponse& Response, int32 AttemptId);
	/**
	 * @brief Replaces item ViewModels from a backend inventory snapshot.
	 * @param SourceItems Items returned by a successful inventory request.
	 */
	void RebuildItems(const TArray<FInventoryItem>& SourceItems);
	/**
	 * @brief Updates the async screen state and derived busy state.
	 * @param NewState New presentation state.
	 */
	void SetState(EAsyncViewState NewState);
	/**
	 * @brief Updates localized error copy and broadcasts when it changes.
	 * @param NewError New player-facing error text.
	 */
	void SetError(const FText& NewError);
	/** @brief Recomputes whether the selected item can currently be equipped. */
	void UpdateCanEquip();
	/** @brief Cancels and clears load and equipment request handles. */
	void CancelRequests();

private:
	/** Backend facade used for inventory queries and mutations. */
	UPROPERTY(Transient)
	TObjectPtr<UBackendServiceSubsystem> BackendSubsystem;

	/** Shared session updated after a successful equipment mutation. */
	UPROPERTY(Transient)
	TObjectPtr<UFrontendSessionSubsystem> SessionSubsystem;

	/** Current asynchronous screen state. */
	UPROPERTY(BlueprintReadOnly, FieldNotify, Category = "UIDemo|Inventory", meta = (AllowPrivateAccess = "true"))
	EAsyncViewState State = EAsyncViewState::Idle;

	/** Item-level ViewModels consumed by the virtualized list. */
	UPROPERTY(BlueprintReadOnly, FieldNotify, Category = "UIDemo|Inventory", meta = (AllowPrivateAccess = "true"))
	TArray<TObjectPtr<UInventoryItemViewModel>> Items;

	/** Item currently selected by the inventory screen. */
	UPROPERTY(BlueprintReadOnly, FieldNotify, Category = "UIDemo|Inventory", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInventoryItemViewModel> SelectedItem;

	/** Localized load or mutation error displayed by the screen. */
	UPROPERTY(BlueprintReadOnly, FieldNotify, Category = "UIDemo|Inventory", meta = (AllowPrivateAccess = "true"))
	FText ErrorText;

	/** Whether a load or equipment mutation currently blocks input. */
	UPROPERTY(BlueprintReadOnly, FieldNotify, Category = "UIDemo|Inventory", meta = (AllowPrivateAccess = "true"))
	bool bIsBusy = false;

	/** Whether the latest successful load was served from cache. */
	UPROPERTY(BlueprintReadOnly, FieldNotify, Category = "UIDemo|Inventory", meta = (AllowPrivateAccess = "true"))
	bool bFromCache = false;

	/** Whether the selected item can currently be equipped. */
	UPROPERTY(BlueprintReadOnly, FieldNotify, Category = "UIDemo|Inventory", meta = (AllowPrivateAccess = "true"))
	bool bCanEquip = false;

	/** Handle for the active inventory query. */
	FBackendRequestHandle LoadRequest;
	/** Handle for the active equipment mutation. */
	FBackendRequestHandle EquipRequest;
	/** Identifier used to reject stale asynchronous callbacks. */
	int32 ActiveAttemptId = 0;
};
