#include "UI/Bindings/FrontendUIConversionLibrary.h"

int32 UFrontendUIConversionLibrary::MatchmakingStateToWidgetIndex(
	const EMatchmakingViewState State)
{
	switch (State)
	{
	case EMatchmakingViewState::Idle:
		return 0;

	case EMatchmakingViewState::Searching:
		return 1;

	case EMatchmakingViewState::MatchFound:
		return 2;

	case EMatchmakingViewState::Cancelled:
		return 3;

	case EMatchmakingViewState::Error:
		return 4;

	default:
		ensureMsgf(
			false,
			TEXT("Unsupported matchmaking view state."));
		return 0;
	}
}

int32 UFrontendUIConversionLibrary::AsyncStateToWidgetIndex(
	const EAsyncViewState State)
{
	switch (State)
	{
	case EAsyncViewState::Idle:
		return 0;

	case EAsyncViewState::Loading:
		return 1;

	case EAsyncViewState::Content:
		return 2;

	case EAsyncViewState::Empty:
		return 3;

	case EAsyncViewState::Error:
		return 4;

	default:
		ensureMsgf(
			false,
			TEXT("Unsupported asynchronous view state."));
		return 0;
	}
}

FText UFrontendUIConversionLibrary::InventoryCategoryToText(
	const EInventoryItemCategory Category)
{
	switch (Category)
	{
	case EInventoryItemCategory::Weapon:
		return NSLOCTEXT(
			"FrontendUIConversionLibrary",
			"InventoryCategoryWeapon",
			"WEAPON");

	case EInventoryItemCategory::Armor:
		return NSLOCTEXT(
			"FrontendUIConversionLibrary",
			"InventoryCategoryArmor",
			"ARMOR");

	case EInventoryItemCategory::Cosmetic:
		return NSLOCTEXT(
			"FrontendUIConversionLibrary",
			"InventoryCategoryCosmetic",
			"COSMETIC");

	default:
		ensureMsgf(
			false,
			TEXT("Unsupported inventory item category."));
		return FText::GetEmpty();
	}
}

FText UFrontendUIConversionLibrary::InventoryRarityToText(
	const EInventoryItemRarity Rarity)
{
	switch (Rarity)
	{
	case EInventoryItemRarity::Common:
		return NSLOCTEXT(
			"FrontendUIConversionLibrary",
			"InventoryRarityCommon",
			"COMMON");

	case EInventoryItemRarity::Rare:
		return NSLOCTEXT(
			"FrontendUIConversionLibrary",
			"InventoryRarityRare",
			"RARE");

	case EInventoryItemRarity::Epic:
		return NSLOCTEXT(
			"FrontendUIConversionLibrary",
			"InventoryRarityEpic",
			"EPIC");

	case EInventoryItemRarity::Legendary:
		return NSLOCTEXT(
			"FrontendUIConversionLibrary",
			"InventoryRarityLegendary",
			"LEGENDARY");

	default:
		ensureMsgf(
			false,
			TEXT("Unsupported inventory item rarity."));
		return FText::GetEmpty();
	}
}

ESlateVisibility
UFrontendUIConversionLibrary::InventoryActionErrorToVisibility(
	const EAsyncViewState State,
	const FText& ErrorText)
{
	return State == EAsyncViewState::Content &&
		!ErrorText.IsEmpty()
		? ESlateVisibility::Visible
		: ESlateVisibility::Collapsed;
}

ESlateVisibility
UFrontendUIConversionLibrary::InventoryBusyToVisibility(
	const EAsyncViewState State,
	const bool bIsBusy)
{
	return State == EAsyncViewState::Content && bIsBusy
		? ESlateVisibility::Visible
		: ESlateVisibility::Collapsed;
}

ESlateVisibility
UFrontendUIConversionLibrary::BoolToHitTestInvisibleOrCollapsed(
	const bool bVisible)
{
	return bVisible
		? ESlateVisibility::HitTestInvisible
		: ESlateVisibility::Collapsed;
}

int32 UFrontendUIConversionLibrary::BackendScenarioToIndex(
	const EBackendSimulationScenario Scenario)
{
	switch (Scenario)
	{
	case EBackendSimulationScenario::Success:
		return 0;

	case EBackendSimulationScenario::ServiceUnavailable:
		return 1;

	case EBackendSimulationScenario::ProfileRequestFailed:
		return 2;

	case EBackendSimulationScenario::ConfigRequestFailed:
		return 3;

	case EBackendSimulationScenario::InventoryEmpty:
		return 4;

	case EBackendSimulationScenario::InventoryRequestFailed:
		return 5;

	case EBackendSimulationScenario::EquipItemFailed:
		return 6;

	case EBackendSimulationScenario::ProgressionRequestFailed:
		return 7;

	case EBackendSimulationScenario::UnlockSkillFailed:
		return 8;

	case EBackendSimulationScenario::InsufficientSkillPoints:
		return 9;

	case EBackendSimulationScenario::MatchmakingFailed:
		return 10;

	case EBackendSimulationScenario::Timeout:
		return 11;

	default:
		ensureMsgf(
			false,
			TEXT("Unsupported backend simulation scenario."));
		return 0;
	}
}

EBackendSimulationScenario
UFrontendUIConversionLibrary::IndexToBackendScenario(
	const int32 Index)
{
	switch (Index)
	{
	case 0:
		return EBackendSimulationScenario::Success;

	case 1:
		return EBackendSimulationScenario::ServiceUnavailable;

	case 2:
		return EBackendSimulationScenario::ProfileRequestFailed;

	case 3:
		return EBackendSimulationScenario::ConfigRequestFailed;

	case 4:
		return EBackendSimulationScenario::InventoryEmpty;

	case 5:
		return EBackendSimulationScenario::InventoryRequestFailed;

	case 6:
		return EBackendSimulationScenario::EquipItemFailed;

	case 7:
		return EBackendSimulationScenario::ProgressionRequestFailed;

	case 8:
		return EBackendSimulationScenario::UnlockSkillFailed;

	case 9:
		return EBackendSimulationScenario::InsufficientSkillPoints;

	case 10:
		return EBackendSimulationScenario::MatchmakingFailed;

	case 11:
		return EBackendSimulationScenario::Timeout;

	default:
		ensureMsgf(
			false,
			TEXT("Backend scenario index is outside the supported range."));
		return EBackendSimulationScenario::Success;
	}
}