#pragma once

#include "CoreMinimal.h"
#include "Backend/InventoryTypes.h"
#include "MVVMViewModelBase.h"
#include "InventoryItemViewModel.generated.h"

UCLASS(BlueprintType)
class UIDEMO_API UInventoryItemViewModel final : public UMVVMViewModelBase
{
	GENERATED_BODY()

public:
	void Initialize(const FInventoryItem& Item);
	void SetSelected(bool bNewSelected);
	void SetEquipped(bool bNewEquipped);

	FName GetItemId() const { return ItemId; }
	EInventoryItemCategory GetCategory() const { return Category; }
	bool IsEquipped() const { return bEquipped; }

private:
	UPROPERTY(BlueprintReadOnly, FieldNotify, Category = "UIDemo|Inventory", meta = (AllowPrivateAccess = "true"))
	FName ItemId = NAME_None;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Category = "UIDemo|Inventory", meta = (AllowPrivateAccess = "true"))
	FText DisplayName;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Category = "UIDemo|Inventory", meta = (AllowPrivateAccess = "true"))
	FText Description;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Category = "UIDemo|Inventory", meta = (AllowPrivateAccess = "true"))
	FName IconId = NAME_None;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Category = "UIDemo|Inventory", meta = (AllowPrivateAccess = "true"))
	EInventoryItemCategory Category = EInventoryItemCategory::Weapon;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Category = "UIDemo|Inventory", meta = (AllowPrivateAccess = "true"))
	EInventoryItemRarity Rarity = EInventoryItemRarity::Common;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Category = "UIDemo|Inventory", meta = (AllowPrivateAccess = "true"))
	int32 Power = 0;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Category = "UIDemo|Inventory", meta = (AllowPrivateAccess = "true"))
	bool bEquipped = false;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Category = "UIDemo|Inventory", meta = (AllowPrivateAccess = "true"))
	bool bSelected = false;
};
