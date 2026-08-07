#include "UI/ViewModels/ProgressionViewModel.h"

#include "Engine/GameInstance.h"
#include "Engine/World.h"
#include "Frontend/FrontendSessionSubsystem.h"
#include "ProfilingDebugging/CpuProfilerTrace.h"
#include "UI/ViewModels/SkillNodeViewModel.h"
#include "UI/ViewModels/ViewModelHelpers.h"

bool UProgressionViewModel::Initialize(UObject* WorldContextObject)
{
	CancelRequests();
	++ActiveAttemptId;
	UWorld* World = IsValid(WorldContextObject) ? WorldContextObject->GetWorld() : nullptr;
	UGameInstance* GameInstance = World != nullptr ? World->GetGameInstance() : nullptr;
	BackendSubsystem = GameInstance != nullptr
		? GameInstance->GetSubsystem<UBackendServiceSubsystem>() : nullptr;
	SessionSubsystem = GameInstance != nullptr
		? GameInstance->GetSubsystem<UFrontendSessionSubsystem>() : nullptr;

	if (!IsValid(BackendSubsystem) || !IsValid(SessionSubsystem))
	{
		SetError(NSLOCTEXT("ProgressionViewModel", "MissingSubsystem", "Progression could not be initialized."));
		SetState(EAsyncViewState::Error);
		return false;
	}

	SetError(FText::GetEmpty());
	SetState(EAsyncViewState::Idle);
	return true;
}

void UProgressionViewModel::LoadProgression(const bool bForceRefresh)
{
	if (!IsValid(BackendSubsystem) || bIsBusy)
	{
		return;
	}
	CancelRequests();
	const int32 AttemptId = ++ActiveAttemptId;
	SetError(FText::GetEmpty());
	SetState(EAsyncViewState::Loading);
	LoadRequest = BackendSubsystem->FetchProgression(
		bForceRefresh,
		FOnProgressionCompleted::CreateUObject(
			this, &UProgressionViewModel::HandleProgressionResponse, AttemptId));
}

void UProgressionViewModel::Retry()
{
	if (State == EAsyncViewState::Error)
	{
		LoadProgression(true);
	}
}

void UProgressionViewModel::SelectNode(USkillNodeViewModel* Node)
{
	if (SelectedNode == Node)
	{
		return;
	}
	if (IsValid(SelectedNode))
	{
		SelectedNode->SetSelected(false);
	}
	SelectedNode = Node;
	if (IsValid(SelectedNode))
	{
		SelectedNode->SetSelected(true);
	}
	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(SelectedNode);
	UpdateCanUnlock();
}

void UProgressionViewModel::UnlockSelectedNode()
{
	if (!IsValid(BackendSubsystem) || !bCanUnlock)
	{
		return;
	}

	const int32 AttemptId = ++ActiveAttemptId;
	SetError(FText::GetEmpty());
	bIsBusy = true;
	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(bIsBusy);
	UpdateCanUnlock();
	UnlockRequest = BackendSubsystem->UnlockSkill(
		SelectedNode->GetSkillId(),
		FOnUnlockSkillCompleted::CreateUObject(
			this, &UProgressionViewModel::HandleUnlockResponse, AttemptId));
}

void UProgressionViewModel::Cancel()
{
	CancelRequests();
	++ActiveAttemptId;
	SetState(Nodes.IsEmpty() ? EAsyncViewState::Idle : EAsyncViewState::Content);
}

void UProgressionViewModel::BeginDestroy()
{
	CancelRequests();
	++ActiveAttemptId;
	Super::BeginDestroy();
}

void UProgressionViewModel::HandleProgressionResponse(
	const FProgressionResponse& Response,
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
	RebuildNodes(Response.Progression);
	SetState(Nodes.IsEmpty() ? EAsyncViewState::Empty : EAsyncViewState::Content);
}

void UProgressionViewModel::HandleUnlockResponse(
	const FUnlockSkillResponse& Response,
	const int32 AttemptId)
{
	if (AttemptId != ActiveAttemptId)
	{
		return;
	}
	UnlockRequest.Reset();
	bIsBusy = false;
	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(bIsBusy);
	UpdateCanUnlock();
	if (!Response.bRequestSucceeded)
	{
		SetError(UIDemo::UI::MakeUserFacingBackendError(Response.Error));
		return;
	}

	SkillPoints = Response.RemainingSkillPoints;
	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(SkillPoints);
	UpdateCanUnlock();
	SessionSubsystem->SetSkillPoints(SkillPoints);
	// Reload bypasses the invalidated cache and refreshes newly available nodes.
	LoadProgression(true);
}

void UProgressionViewModel::RebuildNodes(const FProgressionData& Data)
{
	TRACE_CPUPROFILER_EVENT_SCOPE(UIDemo_Progression_RebuildNodes);

	SkillPoints = Data.SkillPoints;
	Nodes.Reset(Data.Nodes.Num());
	SelectedNode = nullptr;
	for (const FSkillNodeData& SourceNode : Data.Nodes)
	{
		USkillNodeViewModel* Node = NewObject<USkillNodeViewModel>(this);
		Node->Initialize(SourceNode);
		Nodes.Add(Node);
	}
	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(SkillPoints);
	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(Nodes);
	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(SelectedNode);
	if (!Nodes.IsEmpty())
	{
		SelectNode(Nodes[0]);
	}
}

void UProgressionViewModel::SetState(const EAsyncViewState NewState)
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
	UpdateCanUnlock();
}

void UProgressionViewModel::SetError(const FText& NewError)
{
	if (!ErrorText.EqualTo(NewError))
	{
		ErrorText = NewError;
		UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(ErrorText);
	}
}

void UProgressionViewModel::UpdateCanUnlock()
{
	const bool bNewCanUnlock = IsValid(SelectedNode) && !bIsBusy &&
		SelectedNode->GetState() == ESkillNodeState::Available &&
		SkillPoints >= SelectedNode->GetCost();
	if (bCanUnlock != bNewCanUnlock)
	{
		bCanUnlock = bNewCanUnlock;
		UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(bCanUnlock);
	}
}

void UProgressionViewModel::CancelRequests()
{
	if (IsValid(BackendSubsystem))
	{
		BackendSubsystem->CancelRequest(LoadRequest);
		BackendSubsystem->CancelRequest(UnlockRequest);
	}
	LoadRequest.Reset();
	UnlockRequest.Reset();
}
