#include "UI/ViewModels/InventoryViewModel.h"

#include "Engine/GameInstance.h"
#include "Engine/World.h"
#include "Frontend/FrontendSessionSubsystem.h"
#include "ProfilingDebugging/CpuProfilerTrace.h"
#include "UI/ViewModels/InventoryItemViewModel.h"
#include "UI/ViewModels/ViewModelHelpers.h"

bool UInventoryViewModel::Initialize(UObject* WorldContextObject)
{
	CancelRequests();
	++ActiveAttemptId;
	BackendSubsystem = nullptr;
	SessionSubsystem = nullptr;

	UWorld* World = IsValid(WorldContextObject) ? WorldContextObject->GetWorld() : nullptr;
	UGameInstance* GameInstance = World != nullptr ? World->GetGameInstance() : nullptr;
	if (GameInstance == nullptr)
	{
		SetError(NSLOCTEXT("InventoryViewModel", "NoGameInstance", "The inventory could not be initialized."));
		SetState(EAsyncViewState::Error);
		return false;
	}

	BackendSubsystem = GameInstance->GetSubsystem<UBackendServiceSubsystem>();
	SessionSubsystem = GameInstance->GetSubsystem<UFrontendSessionSubsystem>();
	if (!IsValid(BackendSubsystem) || !IsValid(SessionSubsystem))
	{
		SetError(NSLOCTEXT("InventoryViewModel", "MissingSubsystem", "Required services are unavailable."));
		SetState(EAsyncViewState::Error);
		return false;
	}

	SetError(FText::GetEmpty());
	SetState(EAsyncViewState::Idle);
	return true;
}

void UInventoryViewModel::LoadInventory(const bool bForceRefresh)
{
	if (!IsValid(BackendSubsystem) || bIsBusy)
	{
		return;
	}

	CancelRequests();
	const int32 AttemptId = ++ActiveAttemptId;
	SetError(FText::GetEmpty());
	SetState(EAsyncViewState::Loading);
	LoadRequest = BackendSubsystem->FetchInventory(
		bForceRefresh,
		FOnInventoryCompleted::CreateUObject(
			this, &UInventoryViewModel::HandleInventoryResponse, AttemptId));
}

void UInventoryViewModel::Retry()
{
	if (State == EAsyncViewState::Error)
	{
		LoadInventory(true);
	}
}

void UInventoryViewModel::SelectItem(UInventoryItemViewModel* Item)
{
	if (SelectedItem == Item)
	{
		return;
	}

	if (IsValid(SelectedItem))
	{
		SelectedItem->SetSelected(false);
	}
	SelectedItem = Item;
	if (IsValid(SelectedItem))
	{
		SelectedItem->SetSelected(true);
	}
	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(SelectedItem);
	UpdateCanEquip();
}

void UInventoryViewModel::EquipSelectedItem()
{
	if (!IsValid(BackendSubsystem) || !bCanEquip)
	{
		return;
	}

	const int32 AttemptId = ++ActiveAttemptId;
	SetError(FText::GetEmpty());
	bIsBusy = true;
	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(bIsBusy);
	UpdateCanEquip();
	EquipRequest = BackendSubsystem->EquipItem(
		SelectedItem->GetItemId(),
		FOnEquipItemCompleted::CreateUObject(
			this, &UInventoryViewModel::HandleEquipResponse, AttemptId));
}

void UInventoryViewModel::Cancel()
{
	CancelRequests();
	++ActiveAttemptId;
	SetState(Items.IsEmpty() ? EAsyncViewState::Idle : EAsyncViewState::Content);
}

void UInventoryViewModel::BeginDestroy()
{
	CancelRequests();
	++ActiveAttemptId;
	Super::BeginDestroy();
}

void UInventoryViewModel::HandleInventoryResponse(
	const FInventoryResponse& Response,
	const int32 AttemptId)
{
	if (AttemptId != ActiveAttemptId)
	{
		return;
	}

	LoadRequest.Reset();
	if (!Response.bRequestSucceeded)
	{
		SetError(UIDemo::UI::MakeUserFacingBackendError(Response.Error));
		SetState(EAsyncViewState::Error);
		return;
	}

	bFromCache = Response.bFromCache;
	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(bFromCache);
	RebuildItems(Response.Items);
	SetState(Items.IsEmpty() ? EAsyncViewState::Empty : EAsyncViewState::Content);
}

void UInventoryViewModel::HandleEquipResponse(
	const FEquipItemResponse& Response,
	const int32 AttemptId)
{
	if (AttemptId != ActiveAttemptId)
	{
		return;
	}

	EquipRequest.Reset();
	bIsBusy = false;
	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(bIsBusy);
	UpdateCanEquip();
	if (!Response.bRequestSucceeded)
	{
		SetError(UIDemo::UI::MakeUserFacingBackendError(Response.Error));
		return;
	}

	const EInventoryItemCategory EquippedCategory = SelectedItem->GetCategory();
	for (UInventoryItemViewModel* Item : Items)
	{
		if (IsValid(Item) && Item->GetCategory() == EquippedCategory)
		{
			Item->SetEquipped(Item->GetItemId() == Response.EquippedItemId);
		}
	}
	SessionSubsystem->SetEquippedItemId(Response.EquippedItemId);
	UpdateCanEquip();
}

void UInventoryViewModel::RebuildItems(const TArray<FInventoryItem>& SourceItems)
{
	TRACE_CPUPROFILER_EVENT_SCOPE(UIDemo_Inventory_RebuildItems);

	Items.Reset(SourceItems.Num());
	SelectedItem = nullptr;
	for (const FInventoryItem& SourceItem : SourceItems)
	{
		UInventoryItemViewModel* Item = NewObject<UInventoryItemViewModel>(this);
		Item->Initialize(SourceItem);
		Items.Add(Item);
	}
	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(Items);
	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(SelectedItem);
	if (!Items.IsEmpty())
	{
		SelectItem(Items[0]);
	}
	ItemsRebuiltEvent.Broadcast();
}

void UInventoryViewModel::SetState(const EAsyncViewState NewState)
{
	if (State != NewState)
	{
		State = NewState;
		UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(State);
	}
	const bool bNewBusy = NewState == EAsyncViewState::Loading;
	if (bIsBusy != bNewBusy)
	{
		bIsBusy = bNewBusy;
		UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(bIsBusy);
	}
	UpdateCanEquip();
}

void UInventoryViewModel::SetError(const FText& NewError)
{
	if (!ErrorText.EqualTo(NewError))
	{
		ErrorText = NewError;
		UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(ErrorText);
	}
}

void UInventoryViewModel::UpdateCanEquip()
{
	const bool bNewCanEquip = IsValid(SelectedItem) && !bIsBusy &&
		!SelectedItem->IsEquipped();
	if (bCanEquip != bNewCanEquip)
	{
		bCanEquip = bNewCanEquip;
		UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(bCanEquip);
		CanEquipChangedEvent.Broadcast(bCanEquip);
	}
}

void UInventoryViewModel::CancelRequests()
{
	if (IsValid(BackendSubsystem))
	{
		BackendSubsystem->CancelRequest(LoadRequest);
		BackendSubsystem->CancelRequest(EquipRequest);
	}
	LoadRequest.Reset();
	EquipRequest.Reset();
}
