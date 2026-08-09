#include "UI/Screens/InventoryScreen.h"

#include "CommonListView.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "UI/Common/MenuButtonBase.h"
#include "UI/ViewModels/InventoryItemViewModel.h"
#include "UI/ViewModels/InventoryViewModel.h"

void UInventoryScreen::SetInventoryViewModel(
	UInventoryViewModel* InViewModel)
{
	if (BoundInventoryViewModel == InViewModel)
	{
		HandleCanEquipChanged(
			IsValid(BoundInventoryViewModel) &&
			BoundInventoryViewModel->CanEquipSelectedItem());
		ScheduleSelectionSynchronization();
		return;
	}

	UnbindViewModel();
	BoundInventoryViewModel = InViewModel;
	BindViewModel();

	HandleCanEquipChanged(
		IsValid(BoundInventoryViewModel) &&
		BoundInventoryViewModel->CanEquipSelectedItem());
	ScheduleSelectionSynchronization();
}

void UInventoryScreen::NativeConstruct()
{
	Super::NativeConstruct();

	checkf(
		IsValid(InventoryList),
		TEXT("InventoryScreen requires a CommonListView named InventoryList."));
	checkf(
		IsValid(EquipButton),
		TEXT("InventoryScreen requires a MenuButtonBase named EquipButton."));

	InventoryList->OnItemSelectionChanged().RemoveAll(this);
	InventoryList->OnItemSelectionChanged().AddUObject(
		this,
		&UInventoryScreen::HandleListSelectionChanged);

	InventoryList->SetNavigationRuleExplicit(
		EUINavigation::Right,
		EquipButton);

	EquipButton->SetNavigationRuleExplicit(
		EUINavigation::Left,
		InventoryList);

	HandleCanEquipChanged(
		IsValid(BoundInventoryViewModel) &&
		BoundInventoryViewModel->CanEquipSelectedItem());
}

void UInventoryScreen::NativeDestruct()
{
	bSelectionSynchronizationPending = false;

	if (IsValid(InventoryList))
	{
		InventoryList->OnItemSelectionChanged().RemoveAll(this);
	}
	UnbindViewModel();
	BoundInventoryViewModel = nullptr;

	Super::NativeDestruct();
}

void UInventoryScreen::NativeOnActivated()
{
	Super::NativeOnActivated();
	ScheduleSelectionSynchronization();
}

UWidget* UInventoryScreen::NativeGetDesiredFocusTarget() const
{
	return IsValid(InventoryList) && InventoryList->GetNumItems() > 0
		? InventoryList.Get()
		: Super::NativeGetDesiredFocusTarget();
}

void UInventoryScreen::BindViewModel()
{
	if (!IsValid(BoundInventoryViewModel))
	{
		return;
	}

	BoundInventoryViewModel->OnItemsRebuilt().AddUObject(
		this,
		&UInventoryScreen::HandleItemsRebuilt);
	BoundInventoryViewModel->OnCanEquipChanged().AddUObject(
		this,
		&UInventoryScreen::HandleCanEquipChanged);
}

void UInventoryScreen::UnbindViewModel()
{
	if (!IsValid(BoundInventoryViewModel))
	{
		return;
	}

	BoundInventoryViewModel->OnItemsRebuilt().RemoveAll(this);
	BoundInventoryViewModel->OnCanEquipChanged().RemoveAll(this);
}

void UInventoryScreen::HandleItemsRebuilt()
{
	ScheduleSelectionSynchronization();
}

void UInventoryScreen::HandleCanEquipChanged(const bool bCanEquip)
{
	if (IsValid(EquipButton))
	{
		EquipButton->SetIsLocked(!bCanEquip);
	}
}

void UInventoryScreen::HandleListSelectionChanged(UObject* Item)
{
	if (!IsValid(BoundInventoryViewModel) || !IsValid(Item))
	{
		return;
	}

	if (UInventoryItemViewModel* ItemViewModel =
		Cast<UInventoryItemViewModel>(Item))
	{
		BoundInventoryViewModel->SelectItem(ItemViewModel);
	}
}

void UInventoryScreen::ScheduleSelectionSynchronization()
{
	if (bSelectionSynchronizationPending || !IsValid(BoundInventoryViewModel))
	{
		return;
	}

	bSelectionSynchronizationPending = true;
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimerForNextTick(
			FTimerDelegate::CreateUObject(
				this,
				&UInventoryScreen::SynchronizeSelectionAndFocus));
		return;
	}

	SynchronizeSelectionAndFocus();
}

void UInventoryScreen::SynchronizeSelectionAndFocus()
{
	bSelectionSynchronizationPending = false;
	if (!IsActivated() ||
		!IsValid(BoundInventoryViewModel) ||
		!IsValid(InventoryList))
	{
		return;
	}

	UObject* ItemToSelect = BoundInventoryViewModel->GetSelectedItem();
	int32 ItemIndex = InventoryList->GetIndexForItem(ItemToSelect);
	if (ItemIndex == INDEX_NONE && InventoryList->GetNumItems() > 0)
	{
		ItemIndex = 0;
		ItemToSelect = InventoryList->GetItemAt(ItemIndex);
	}

	if (!IsValid(ItemToSelect) || ItemIndex == INDEX_NONE)
	{
		return;
	}

	InventoryList->SetSelectedItem(ItemToSelect);
	InventoryList->NavigateToIndex(ItemIndex);
	RequestRefreshFocus();
}
