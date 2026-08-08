#pragma once

#include "CoreMinimal.h"
#include "Backend/InventoryTypes.h"
#include "MVVMViewModelBase.h"
#include "InventoryItemViewModel.generated.h"

/**
 * @brief Item-level presentation model used as a virtualized list item.
 *
 * Each instance owns immutable item presentation data plus mutable selection
 * and equipment flags that notify its active list-entry widget.
 */
UCLASS(BlueprintType)
class UIDEMO_API UInventoryItemViewModel final : public UMVVMViewModelBase
{
	GENERATED_BODY()

public:
	/**
	 * @brief Initializes the ViewModel from one backend item payload.
	 * @param Item Source item copied into presentation fields.
	 */
	void Initialize(const FInventoryItem& Item);

	/**
	 * @brief Updates the list-selection state and broadcasts when it changes.
	 * @param bNewSelected New selection state.
	 */
	void SetSelected(bool bNewSelected);

	/**
	 * @brief Updates the equipment state and broadcasts when it changes.
	 * @param bNewEquipped New equipment state.
	 */
	void SetEquipped(bool bNewEquipped);

	/** @return Stable identifier of the represented item. */
	FName GetItemId() const { return ItemId; }
	/** @return Gameplay category of the represented item. */
	EInventoryItemCategory GetCategory() const { return Category; }
	/** @return True when the represented item is currently equipped. */
	bool IsEquipped() const { return bEquipped; }

private:
	/** Stable identifier used by equipment requests. */
	UPROPERTY(BlueprintReadOnly, FieldNotify, Category = "UIDemo|Inventory", meta = (AllowPrivateAccess = "true"))
	FName ItemId = NAME_None;

	/** Localized player-facing item name. */
	UPROPERTY(BlueprintReadOnly, FieldNotify, Category = "UIDemo|Inventory", meta = (AllowPrivateAccess = "true"))
	FText DisplayName;

	/** Localized player-facing item description. */
	UPROPERTY(BlueprintReadOnly, FieldNotify, Category = "UIDemo|Inventory", meta = (AllowPrivateAccess = "true"))
	FText Description;

	/** Presentation key used by the widget to resolve an icon. */
	UPROPERTY(BlueprintReadOnly, FieldNotify, Category = "UIDemo|Inventory", meta = (AllowPrivateAccess = "true"))
	FName IconId = NAME_None;

	/** Gameplay category used for grouping and equipment exclusivity. */
	UPROPERTY(BlueprintReadOnly, FieldNotify, Category = "UIDemo|Inventory", meta = (AllowPrivateAccess = "true"))
	EInventoryItemCategory Category = EInventoryItemCategory::Weapon;

	/** Presentation rarity copied from the backend payload. */
	UPROPERTY(BlueprintReadOnly, FieldNotify, Category = "UIDemo|Inventory", meta = (AllowPrivateAccess = "true"))
	EInventoryItemRarity Rarity = EInventoryItemRarity::Common;

	/** Gameplay power value displayed by the entry and details panel. */
	UPROPERTY(BlueprintReadOnly, FieldNotify, Category = "UIDemo|Inventory", meta = (AllowPrivateAccess = "true"))
	int32 Power = 0;

	/** Whether the item is currently equipped in its category. */
	UPROPERTY(BlueprintReadOnly, FieldNotify, Category = "UIDemo|Inventory", meta = (AllowPrivateAccess = "true"))
	bool bEquipped = false;

	/** Whether the owning inventory screen currently selects this item. */
	UPROPERTY(BlueprintReadOnly, FieldNotify, Category = "UIDemo|Inventory", meta = (AllowPrivateAccess = "true"))
	bool bSelected = false;
};
