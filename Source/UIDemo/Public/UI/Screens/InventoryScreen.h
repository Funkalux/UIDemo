#pragma once

#include "CoreMinimal.h"
#include "Types/SlateEnums.h"
#include "UI/Common/MenuScreenBase.h"
#include "InventoryScreen.generated.h"

class UCommonListView;
class UInventoryViewModel;
class UMenuButtonBase;
class UWidget;

/**
 * @brief Native CommonUI screen responsible for inventory focus coordination.
 *
 * The screen synchronizes the virtualized list with the selection owned by the
 * inventory ViewModel, establishes deterministic controller navigation, and
 * keeps the equip action focusable while preventing unavailable activations.
 * Layout, styling, and data presentation remain in the derived Widget Blueprint.
 */
UCLASS(Abstract, Blueprintable)
class UIDEMO_API UInventoryScreen : public UMenuScreenBase
{
	GENERATED_BODY()

public:
	/**
	 * @brief Associates the screen with its MVVM-generated inventory ViewModel.
	 *
	 * Calling this function repeatedly with the same instance is safe. Passing a
	 * different instance removes all native listeners from the previous one.
	 *
	 * @param InViewModel Inventory ViewModel assigned to the Widget Blueprint.
	 */
	UFUNCTION(BlueprintCallable, Category = "UIDemo|Inventory")
	void SetInventoryViewModel(UInventoryViewModel* InViewModel);

protected:
	/** @brief Binds list events and establishes navigation after widget creation. */
	virtual void NativeConstruct() override;

	/** @brief Removes native listeners before the underlying widget tree is released. */
	virtual void NativeDestruct() override;

	/** @brief Restores the current list selection whenever the screen is activated. */
	virtual void NativeOnActivated() override;

	/**
	 * @brief Resolves the inventory list as the preferred focus target when populated.
	 * @return InventoryList when it contains items; otherwise the base-class target.
	 */
	virtual UWidget* NativeGetDesiredFocusTarget() const override;

	/** Virtualized inventory list supplied by the derived Widget Blueprint. */
	UPROPERTY(
		BlueprintReadOnly,
		meta = (BindWidget),
		Category = "UIDemo|Inventory")
	TObjectPtr<UCommonListView> InventoryList;

	/** Equip action supplied by the derived Widget Blueprint. */
	UPROPERTY(
		BlueprintReadOnly,
		meta = (BindWidget),
		Category = "UIDemo|Inventory")
	TObjectPtr<UMenuButtonBase> EquipButton;

private:
	/** Registers native listeners on the currently assigned ViewModel. */
	void BindViewModel();

	/** Removes every native listener registered by this screen. */
	void UnbindViewModel();

	/** Queues selection synchronization after MVVM has updated the list source. */
	void HandleItemsRebuilt();

	/** Applies equip-action availability without removing the button from focus. */
	void HandleCanEquipChanged(bool bCanEquip);

	/** Forwards a ListView selection change to the inventory ViewModel. */
	void HandleListSelectionChanged(UObject* Item);

	/** Schedules one selection pass for the next frame of the widget lifecycle. */
	void ScheduleSelectionSynchronization();

	/** Aligns ListView selection and CommonUI focus with the ViewModel selection. */
	void SynchronizeSelectionAndFocus();

	/** ViewModel instance currently observed by this native screen. */
	UPROPERTY(Transient)
	TObjectPtr<UInventoryViewModel> BoundInventoryViewModel;

	/** Prevents duplicate next-frame synchronization requests. */
	bool bSelectionSynchronizationPending = false;
};
