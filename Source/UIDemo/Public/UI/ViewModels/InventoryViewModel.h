#pragma once

#include "CoreMinimal.h"
#include "Backend/BackendServiceSubsystem.h"
#include "MVVMViewModelBase.h"
#include "UI/ViewModels/ViewModelTypes.h"
#include "InventoryViewModel.generated.h"

class UFrontendSessionSubsystem;
class UInventoryItemViewModel;

/** Broadcast after the inventory has rebuilt its item ViewModels. */
DECLARE_MULTICAST_DELEGATE(FOnInventoryItemsRebuilt);

/** Broadcast when the availability of the equip action changes. */
DECLARE_MULTICAST_DELEGATE_OneParam(
	FOnInventoryCanEquipChanged,
	bool /* bCanEquip */);

/**
 * @brief Screen-level presentation model for inventory browsing and equipment.
 *
 * The ViewModel owns asynchronous request state, item ViewModels, selection,
 * and equipment rules. The native screen coordinates UMG focus and navigation
 * without introducing widget references into this presentation model.
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

	/**
	 * @brief Returns the item currently selected by the inventory presentation.
	 * @return Selected item ViewModel, or nullptr when no item is selected.
	 */
	UInventoryItemViewModel* GetSelectedItem() const
	{
		return SelectedItem;
	}

	/**
	 * @brief Reports whether the selected item can currently be equipped.
	 * @return True when the equip action is available.
	 */
	bool CanEquipSelectedItem() const
	{
		return bCanEquip;
	}

	/**
	 * @brief Provides the native notification emitted after item reconstruction.
	 * @return Mutable multicast delegate used by the owning inventory screen.
	 */
	FOnInventoryItemsRebuilt& OnItemsRebuilt()
	{
		return ItemsRebuiltEvent;
	}

	/**
	 * @brief Provides the native notification for equip-action availability.
	 * @return Mutable multicast delegate used by the owning inventory screen.
	 */
	FOnInventoryCanEquipChanged& OnCanEquipChanged()
	{
		return CanEquipChangedEvent;
	}

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

	/** Native notification used to synchronize the virtualized list after rebuilds. */
	FOnInventoryItemsRebuilt ItemsRebuiltEvent;

	/** Native notification used to keep the CommonUI equip button focusable. */
	FOnInventoryCanEquipChanged CanEquipChangedEvent;
};
