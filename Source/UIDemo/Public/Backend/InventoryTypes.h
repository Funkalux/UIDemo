#pragma once

#include "CoreMinimal.h"
#include "Backend/BackendTypes.h"
#include "InventoryTypes.generated.h"

/** @brief High-level categories used to group inventory items. */
UENUM(BlueprintType)
enum class EInventoryItemCategory : uint8
{
	/** Equippable offensive item. */
	Weapon,
	/** Equippable defensive item. */
	Armor,
	/** Appearance-only item. */
	Cosmetic
};

/** @brief Presentation rarity assigned to an inventory item. */
UENUM(BlueprintType)
enum class EInventoryItemRarity : uint8
{
	/** Baseline item rarity. */
	Common,
	/** Uncommon item rarity. */
	Rare,
	/** High-value item rarity. */
	Epic,
	/** Highest item rarity represented by the demo. */
	Legendary
};

/** @brief Immutable inventory item payload returned by the backend facade. */
USTRUCT(BlueprintType)
struct UIDEMO_API FInventoryItem
{
	GENERATED_BODY()

	/** Stable identifier used by inventory mutation requests. */
	UPROPERTY(BlueprintReadOnly, Category = "Inventory")
	FName ItemId = NAME_None;

	/** Localized player-facing item name. */
	UPROPERTY(BlueprintReadOnly, Category = "Inventory")
	FText DisplayName;

	/** Localized player-facing item description. */
	UPROPERTY(BlueprintReadOnly, Category = "Inventory")
	FText Description;

	/** Presentation key used to resolve the item's icon asset. */
	UPROPERTY(BlueprintReadOnly, Category = "Inventory")
	FName IconId = NAME_None;

	/** Gameplay category used for grouping and equipment exclusivity. */
	UPROPERTY(BlueprintReadOnly, Category = "Inventory")
	EInventoryItemCategory Category = EInventoryItemCategory::Weapon;

	/** Rarity used by item presentation. */
	UPROPERTY(BlueprintReadOnly, Category = "Inventory")
	EInventoryItemRarity Rarity = EInventoryItemRarity::Common;

	/** Gameplay power value; zero is valid for cosmetic items. */
	UPROPERTY(BlueprintReadOnly, Category = "Inventory")
	int32 Power = 0;

	/** Whether this item is currently equipped in its category. */
	UPROPERTY(BlueprintReadOnly, Category = "Inventory")
	bool bEquipped = false;
};

/** @brief Result of an asynchronous inventory query. */
USTRUCT(BlueprintType)
struct UIDEMO_API FInventoryResponse
{
	GENERATED_BODY()

	/** Whether the inventory request completed successfully. */
	UPROPERTY(BlueprintReadOnly, Category = "Inventory")
	bool bRequestSucceeded = false;

	/** Whether the response was served by the local backend cache. */
	UPROPERTY(BlueprintReadOnly, Category = "Inventory")
	bool bFromCache = false;

	/** Inventory payload returned on success. */
	UPROPERTY(BlueprintReadOnly, Category = "Inventory")
	TArray<FInventoryItem> Items;

	/** Failure details returned when the request did not succeed. */
	UPROPERTY(BlueprintReadOnly, Category = "Inventory")
	FBackendError Error;
};

/** @brief Result of an asynchronous equip-item mutation. */
USTRUCT(BlueprintType)
struct UIDEMO_API FEquipItemResponse
{
	GENERATED_BODY()

	/** Whether the equipment mutation completed successfully. */
	UPROPERTY(BlueprintReadOnly, Category = "Inventory")
	bool bRequestSucceeded = false;

	/** Identifier of the item equipped on success. */
	UPROPERTY(BlueprintReadOnly, Category = "Inventory")
	FName EquippedItemId = NAME_None;

	/** Failure details returned when the mutation did not succeed. */
	UPROPERTY(BlueprintReadOnly, Category = "Inventory")
	FBackendError Error;
};
