#include "UI/ViewModels/InventoryItemViewModel.h"

void UInventoryItemViewModel::Initialize(const FInventoryItem& Item)
{
	ItemId = Item.ItemId;
	DisplayName = Item.DisplayName;
	Description = Item.Description;
	IconId = Item.IconId;
	Category = Item.Category;
	Rarity = Item.Rarity;
	Power = Item.Power;
	bEquipped = Item.bEquipped;
	bSelected = false;
}

void UInventoryItemViewModel::SetSelected(const bool bNewSelected)
{
	if (bSelected != bNewSelected)
	{
		bSelected = bNewSelected;
		UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(bSelected);
	}
}

void UInventoryItemViewModel::SetEquipped(const bool bNewEquipped)
{
	if (bEquipped != bNewEquipped)
	{
		bEquipped = bNewEquipped;
		UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(bEquipped);
	}
}
