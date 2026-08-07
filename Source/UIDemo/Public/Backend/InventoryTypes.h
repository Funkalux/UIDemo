#pragma once

#include "CoreMinimal.h"
#include "Backend/BackendTypes.h"
#include "InventoryTypes.generated.h"

UENUM(BlueprintType)
enum class EInventoryItemCategory : uint8
{
	Weapon,
	Armor,
	Cosmetic
};

UENUM(BlueprintType)
enum class EInventoryItemRarity : uint8
{
	Common,
	Rare,
	Epic,
	Legendary
};

USTRUCT(BlueprintType)
struct UIDEMO_API FInventoryItem
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Inventory")
	FName ItemId = NAME_None;

	UPROPERTY(BlueprintReadOnly, Category = "Inventory")
	FText DisplayName;

	UPROPERTY(BlueprintReadOnly, Category = "Inventory")
	FText Description;

	UPROPERTY(BlueprintReadOnly, Category = "Inventory")
	FName IconId = NAME_None;

	UPROPERTY(BlueprintReadOnly, Category = "Inventory")
	EInventoryItemCategory Category = EInventoryItemCategory::Weapon;

	UPROPERTY(BlueprintReadOnly, Category = "Inventory")
	EInventoryItemRarity Rarity = EInventoryItemRarity::Common;

	UPROPERTY(BlueprintReadOnly, Category = "Inventory")
	int32 Power = 0;

	UPROPERTY(BlueprintReadOnly, Category = "Inventory")
	bool bEquipped = false;
};

USTRUCT(BlueprintType)
struct UIDEMO_API FInventoryResponse
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Inventory")
	bool bRequestSucceeded = false;

	UPROPERTY(BlueprintReadOnly, Category = "Inventory")
	bool bFromCache = false;

	UPROPERTY(BlueprintReadOnly, Category = "Inventory")
	TArray<FInventoryItem> Items;

	UPROPERTY(BlueprintReadOnly, Category = "Inventory")
	FBackendError Error;
};

USTRUCT(BlueprintType)
struct UIDEMO_API FEquipItemResponse
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Inventory")
	bool bRequestSucceeded = false;

	UPROPERTY(BlueprintReadOnly, Category = "Inventory")
	FName EquippedItemId = NAME_None;

	UPROPERTY(BlueprintReadOnly, Category = "Inventory")
	FBackendError Error;
};
