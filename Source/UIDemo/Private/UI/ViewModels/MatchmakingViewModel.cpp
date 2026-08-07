#include "UI/ViewModels/MatchmakingViewModel.h"

#include "Engine/GameInstance.h"
#include "Engine/World.h"
#include "UI/ViewModels/ViewModelHelpers.h"

#define LOCTEXT_NAMESPACE "MatchmakingViewModel"

bool UMatchmakingViewModel::Initialize(UObject* WorldContextObject)
{
	CancelSearch();
	UWorld* World = IsValid(WorldContextObject) ? WorldContextObject->GetWorld() : nullptr;
	UGameInstance* GameInstance = World != nullptr ? World->GetGameInstance() : nullptr;
	BackendSubsystem = GameInstance != nullptr
		? GameInstance->GetSubsystem<UBackendServiceSubsystem>() : nullptr;
	SetState(IsValid(BackendSubsystem)
		? EMatchmakingViewState::Idle
		: EMatchmakingViewState::Error);
	if (!IsValid(BackendSubsystem))
	{
		SetError(LOCTEXT("InitializeError", "Matchmaking could not be initialized."));
		return false;
	}
	return true;
}

void UMatchmakingViewModel::StartSearch(const FName InPlaylistId)
{
	if (!IsValid(BackendSubsystem) || bIsSearching)
	{
		return;
	}
	PlaylistId = InPlaylistId.IsNone() ? FName(TEXT("standard")) : InPlaylistId;
	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(PlaylistId);
	SetError(FText::GetEmpty());
	SessionId.Reset();
	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(SessionId);
	EstimatedWaitSeconds = 3;
	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(EstimatedWaitSeconds);
	StatusText = LOCTEXT("Searching", "Searching for a match...");
	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(StatusText);
	SetState(EMatchmakingViewState::Searching);
	const int32 AttemptId = ++ActiveAttemptId;
	SearchRequest = BackendSubsystem->StartMatchmaking(
		PlaylistId,
		FOnMatchmakingCompleted::CreateUObject(
			this, &UMatchmakingViewModel::HandleMatchmakingResponse, AttemptId));
}

void UMatchmakingViewModel::Retry()
{
	if (State == EMatchmakingViewState::Error)
	{
		StartSearch(PlaylistId);
	}
}

void UMatchmakingViewModel::CancelSearch()
{
	const bool bWasSearching = bIsSearching;
	if (IsValid(BackendSubsystem))
	{
		BackendSubsystem->CancelRequest(SearchRequest);
	}
	SearchRequest.Reset();
	++ActiveAttemptId;
	if (bWasSearching)
	{
		StatusText = LOCTEXT("Cancelled", "Matchmaking cancelled.");
		UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(StatusText);
		SetState(EMatchmakingViewState::Cancelled);
	}
}

void UMatchmakingViewModel::BeginDestroy()
{
	if (IsValid(BackendSubsystem))
	{
		BackendSubsystem->CancelRequest(SearchRequest);
	}
	SearchRequest.Reset();
	++ActiveAttemptId;
	Super::BeginDestroy();
}

void UMatchmakingViewModel::HandleMatchmakingResponse(
	const FMatchmakingResponse& Response,
	const int32 AttemptId)
{
	if (AttemptId != ActiveAttemptId)
	{
		return;
	}
	SearchRequest.Reset();
	if (!Response.bRequestSucceeded)
	{
		SetError(UIDemo::UI::MakeUserFacingBackendError(Response.Error));
		StatusText = LOCTEXT("SearchFailed", "Matchmaking failed.");
		UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(StatusText);
		SetState(EMatchmakingViewState::Error);
		return;
	}
	SessionId = Response.SessionId;
	EstimatedWaitSeconds = Response.EstimatedWaitSeconds;
	StatusText = LOCTEXT("MatchFound", "Match found.");
	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(SessionId);
	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(EstimatedWaitSeconds);
	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(StatusText);
	SetState(EMatchmakingViewState::MatchFound);
}

void UMatchmakingViewModel::SetState(const EMatchmakingViewState NewState)
{
	if (State != NewState)
	{
		State = NewState;
		UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(State);
	}
	const bool bNewSearching = NewState == EMatchmakingViewState::Searching;
	if (bIsSearching != bNewSearching)
	{
		bIsSearching = bNewSearching;
		UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(bIsSearching);
	}
}

void UMatchmakingViewModel::SetError(const FText& NewError)
{
	if (!ErrorText.EqualTo(NewError))
	{
		ErrorText = NewError;
		UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(ErrorText);
	}
}

#undef LOCTEXT_NAMESPACE
