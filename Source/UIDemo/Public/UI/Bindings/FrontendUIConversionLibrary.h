#pragma once

#include "CoreMinimal.h"
#include "Backend/BackendTypes.h"
#include "Backend/InventoryTypes.h"
#include "Components/SlateWrapperTypes.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "UI/ViewModels/ViewModelTypes.h"
#include "FrontendUIConversionLibrary.generated.h"

/**
 * @brief Stateless conversion functions used by frontend MVVM bindings.
 *
 * This library keeps presentation-specific mappings out of ViewModels while
 * exposing strongly typed conversion functions to Widget Blueprints.
 */
UCLASS()
class UIDEMO_API UFrontendUIConversionLibrary final
	: public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	/**
	 * @brief Maps a matchmaking state to its WidgetSwitcher index.
	 * @param State Matchmaking state exposed by the ViewModel.
	 * @return WidgetSwitcher index associated with the state.
	 */
	UFUNCTION(
		BlueprintPure,
		Category = "UIDemo|UI|Conversion",
		meta = (DisplayName = "Matchmaking State to Widget Index"))
	static int32 MatchmakingStateToWidgetIndex(
		EMatchmakingViewState State);

	/**
	 * @brief Maps a shared asynchronous state to its WidgetSwitcher index.
	 * @param State Asynchronous state exposed by a screen ViewModel.
	 * @return WidgetSwitcher index associated with the state.
	 */
	UFUNCTION(
		BlueprintPure,
		Category = "UIDemo|UI|Conversion",
		meta = (DisplayName = "Async State to Widget Index"))
	static int32 AsyncStateToWidgetIndex(
		EAsyncViewState State);

	/**
	 * @brief Converts an inventory category into localized display text.
	 * @param Category Inventory category to present.
	 * @return Localized player-facing category name.
	 */
	UFUNCTION(
		BlueprintPure,
		Category = "UIDemo|UI|Conversion",
		meta = (DisplayName = "Inventory Category to Text"))
	static FText InventoryCategoryToText(
		EInventoryItemCategory Category);

	/**
	 * @brief Converts an inventory rarity into localized display text.
	 * @param Rarity Inventory rarity to present.
	 * @return Localized player-facing rarity name.
	 */
	UFUNCTION(
		BlueprintPure,
		Category = "UIDemo|UI|Conversion",
		meta = (DisplayName = "Inventory Rarity to Text"))
	static FText InventoryRarityToText(
		EInventoryItemRarity Rarity);

	/**
	 * @brief Resolves visibility for an inventory action error banner.
	 * @param State Current asynchronous inventory state.
	 * @param ErrorText Current user-facing error message.
	 * @return Visible when Content contains an action error; otherwise Collapsed.
	 */
	UFUNCTION(
		BlueprintPure,
		Category = "UIDemo|UI|Conversion",
		meta = (DisplayName = "Inventory Action Error to Visibility"))
	static ESlateVisibility InventoryActionErrorToVisibility(
		EAsyncViewState State,
		const FText& ErrorText);

	/**
	 * @brief Resolves visibility for the inventory input blocker.
	 * @param State Current asynchronous inventory state.
	 * @param bIsBusy Whether an asynchronous operation is active.
	 * @return Visible during a Content-state mutation; otherwise Collapsed.
	 */
	UFUNCTION(
		BlueprintPure,
		Category = "UIDemo|UI|Conversion",
		meta = (DisplayName = "Inventory Busy to Visibility"))
	static ESlateVisibility InventoryBusyToVisibility(
		EAsyncViewState State,
		bool bIsBusy);

	/**
	 * @brief Converts a boolean into non-interactive presentation visibility.
	 * @param bVisible Whether the presentation widget should be displayed.
	 * @return HitTestInvisible when true; otherwise Collapsed.
	 */
	UFUNCTION(
		BlueprintPure,
		Category = "UIDemo|UI|Conversion",
		meta = (DisplayName = "Bool to Hit Test Invisible or Collapsed"))
	static ESlateVisibility BoolToHitTestInvisibleOrCollapsed(
		bool bVisible);

	/**
	 * @brief Maps a backend simulation scenario to its selector index.
	 *
	 * The mapping corresponds to the option order configured in
	 * WBP_BackendDebugModal.
	 *
	 * @param Scenario Backend simulation scenario exposed by the ViewModel.
	 * @return Zero-based selector index associated with the scenario.
	 */
	UFUNCTION(
		BlueprintPure,
		Category = "UIDemo|UI|Conversion",
		meta = (DisplayName = "Backend Scenario to Index"))
	static int32 BackendScenarioToIndex(
		EBackendSimulationScenario Scenario);

	/**
	 * @brief Maps a selector index to a backend simulation scenario.
	 *
	 * The mapping corresponds to the option order configured in
	 * WBP_BackendDebugModal.
	 *
	 * @param Index Zero-based index selected by the scenario selector.
	 * @return Backend simulation scenario associated with the index.
	 */
	UFUNCTION(
		BlueprintPure,
		Category = "UIDemo|UI|Conversion",
		meta = (DisplayName = "Index to Backend Scenario"))
	static EBackendSimulationScenario IndexToBackendScenario(
		int32 Index);
};