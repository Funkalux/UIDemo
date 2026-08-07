#include "UI/ViewModels/BootstrapViewModel.h"

#include "Backend/BackendServiceSubsystem.h"
#include "Engine/GameInstance.h"
#include "Engine/World.h"
#include "Frontend/FrontendSessionSubsystem.h"
#include "UI/ViewModels/ViewModelHelpers.h"

#define LOCTEXT_NAMESPACE "BootstrapViewModel"

DEFINE_LOG_CATEGORY_STATIC(LogUIDemoBootstrap, Log, All);

bool UBootstrapViewModel::Initialize(UObject* WorldContextObject)
{
	CancelActiveRequests();
	++ActiveAttemptId;

	BackendSubsystem = nullptr;
	SessionSubsystem = nullptr;

	if (!IsValid(WorldContextObject))
	{
		SetState(EBootstrapState::Error);
		SetStatusText(
			LOCTEXT(
				"InitializationFailedStatus",
				"Unable to initialize."));

		SetErrorText(
			LOCTEXT(
				"InvalidWorldContextError",
				"No valid world context was provided."));

		return false;
	}

	UWorld* World = WorldContextObject->GetWorld();
	UGameInstance* GameInstance =
		World != nullptr
		? World->GetGameInstance()
		: nullptr;

	if (GameInstance == nullptr)
	{
		SetState(EBootstrapState::Error);
		SetStatusText(
			LOCTEXT(
				"InitializationFailedStatus",
				"Unable to initialize."));

		SetErrorText(
			LOCTEXT(
				"MissingGameInstanceError",
				"The game instance is not available."));

		return false;
	}

	BackendSubsystem =
		GameInstance->GetSubsystem<UBackendServiceSubsystem>();
	SessionSubsystem =
		GameInstance->GetSubsystem<UFrontendSessionSubsystem>();

	if (!IsValid(BackendSubsystem) || !IsValid(SessionSubsystem))
	{
		SetState(EBootstrapState::Error);
		SetStatusText(
			LOCTEXT(
				"InitializationFailedStatus",
				"Unable to initialize."));

		SetErrorText(
			LOCTEXT(
				"MissingBackendSubsystemError",
				"The backend service is not available."));

		return false;
	}

	ResetLoadedData();

	SetErrorText(FText::GetEmpty());
	SetProgress(0.0f);
	SetStatusText(
		LOCTEXT(
			"ReadyToConnectStatus",
			"Ready to connect."));
	SetState(EBootstrapState::Idle);

	return true;
}

void UBootstrapViewModel::StartBootstrap()
{
	if (bIsLoading)
	{
		UE_LOG(
			LogUIDemoBootstrap,
			Verbose,
			TEXT("Ignoring StartBootstrap because an attempt is already active."));

		return;
	}

	if (!IsValid(BackendSubsystem))
	{
		SetState(EBootstrapState::Error);
		SetStatusText(
			LOCTEXT(
				"BackendUnavailableStatus",
				"Unable to connect."));

		SetErrorText(
			LOCTEXT(
				"BackendNotInitializedError",
				"The backend service has not been initialized."));

		return;
	}

	BeginBootstrapAttempt();
}

void UBootstrapViewModel::Retry()
{
	if (State != EBootstrapState::Error)
	{
		return;
	}

	StartBootstrap();
}

void UBootstrapViewModel::Cancel()
{
	CancelActiveRequests();
	++ActiveAttemptId;

	bPlayerProfileCompleted = false;
	bRemoteConfigCompleted = false;

	SetErrorText(FText::GetEmpty());
	SetProgress(0.0f);
	SetStatusText(
		LOCTEXT(
			"CancelledStatus",
			"Connection cancelled."));
	SetState(EBootstrapState::Idle);
}

void UBootstrapViewModel::BeginDestroy()
{
	CancelActiveRequests();
	++ActiveAttemptId;

	Super::BeginDestroy();
}

void UBootstrapViewModel::BeginBootstrapAttempt()
{
	CancelActiveRequests();

	++ActiveAttemptId;
	const int32 AttemptId = ActiveAttemptId;

	bPlayerProfileCompleted = false;
	bRemoteConfigCompleted = false;

	ResetLoadedData();

	SetErrorText(FText::GetEmpty());
	SetProgress(0.10f);
	SetStatusText(
		LOCTEXT(
			"CheckingServicesStatus",
			"Checking online services..."));
	SetState(EBootstrapState::CheckingServices);

	ServiceStatusRequest =
		BackendSubsystem->CheckServiceStatus(
			FOnBackendServiceStatusCompleted::CreateUObject(
				this,
				&UBootstrapViewModel::HandleServiceStatusResponse,
				AttemptId));
}

void UBootstrapViewModel::StartAccountRequests()
{
	const int32 AttemptId = ActiveAttemptId;

	SetProgress(0.35f);
	SetStatusText(
		LOCTEXT(
			"LoadingAccountStatus",
			"Loading account data..."));
	SetState(EBootstrapState::LoadingAccount);

	RemoteConfigRequest =
		BackendSubsystem->FetchRemoteConfig(
			FOnRemoteConfigCompleted::CreateUObject(
				this,
				&UBootstrapViewModel::HandleRemoteConfigResponse,
				AttemptId));

	PlayerProfileRequest =
		BackendSubsystem->FetchPlayerProfile(
			FOnPlayerProfileCompleted::CreateUObject(
				this,
				&UBootstrapViewModel::HandlePlayerProfileResponse,
				AttemptId));
}

void UBootstrapViewModel::HandleServiceStatusResponse(
	const FBackendServiceStatusResponse& Response,
	const int32 AttemptId)
{
	if (!IsCurrentAttempt(AttemptId))
	{
		return;
	}

	ServiceStatusRequest.Reset();

	if (!Response.bRequestSucceeded)
	{
		FailAttempt(Response.Error, AttemptId);
		return;
	}

	if (Response.OverallState !=
		EBackendServiceState::Operational)
	{
		const FBackendError Error = FBackendError::Make(
			EBackendEndpoint::ServiceStatus,
			EBackendErrorCode::ServiceUnavailable,
			TEXT("One or more required backend services are unavailable."),
			true);

		FailAttempt(Error, AttemptId);
		return;
	}

	StartAccountRequests();
}

void UBootstrapViewModel::HandleRemoteConfigResponse(
	const FRemoteConfigResponse& Response,
	const int32 AttemptId)
{
	if (!IsCurrentAttempt(AttemptId))
	{
		return;
	}

	RemoteConfigRequest.Reset();

	if (!Response.bRequestSucceeded)
	{
		FailAttempt(Response.Error, AttemptId);
		return;
	}

	RemoteConfig = Response.Config;
	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(RemoteConfig);

	bRemoteConfigCompleted = true;
	HandleAccountRequestCompleted(AttemptId);
}

void UBootstrapViewModel::HandlePlayerProfileResponse(
	const FPlayerProfileResponse& Response,
	const int32 AttemptId)
{
	if (!IsCurrentAttempt(AttemptId))
	{
		return;
	}

	PlayerProfileRequest.Reset();

	if (!Response.bRequestSucceeded)
	{
		FailAttempt(Response.Error, AttemptId);
		return;
	}

	PlayerProfile = Response.Profile;
	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(PlayerProfile);

	bPlayerProfileCompleted = true;
	HandleAccountRequestCompleted(AttemptId);
}

void UBootstrapViewModel::HandleAccountRequestCompleted(
	const int32 AttemptId)
{
	if (!IsCurrentAttempt(AttemptId))
	{
		return;
	}

	const int32 CompletedRequestCount =
		static_cast<int32>(bPlayerProfileCompleted) +
		static_cast<int32>(bRemoteConfigCompleted);

	const float AccountProgress =
		0.35f + (0.275f * CompletedRequestCount);

	SetProgress(AccountProgress);

	if (bPlayerProfileCompleted &&
		bRemoteConfigCompleted)
	{
		CompleteAttempt(AttemptId);
	}
}

void UBootstrapViewModel::CompleteAttempt(
	const int32 AttemptId)
{
	if (!IsCurrentAttempt(AttemptId))
	{
		return;
	}

	SetProgress(1.0f);
	SessionSubsystem->InitializeSession(PlayerProfile, RemoteConfig);
	SetErrorText(FText::GetEmpty());
	SetStatusText(
		LOCTEXT(
			"ConnectedStatus",
			"Connected."));
	SetState(EBootstrapState::Ready);

	// Invalidate any duplicate or delayed callbacks before notifying the view.
	++ActiveAttemptId;

	UE_LOG(
		LogUIDemoBootstrap,
		Log,
		TEXT("Bootstrap completed successfully."));

	OnBootstrapCompleted.Broadcast();
}

void UBootstrapViewModel::FailAttempt(
	const FBackendError& Error,
	const int32 AttemptId)
{
	if (!IsCurrentAttempt(AttemptId))
	{
		return;
	}

	UE_LOG(
		LogUIDemoBootstrap,
		Warning,
		TEXT("Bootstrap failed. Endpoint: %s. Error: %s. Message: %s"),
		*UEnum::GetValueAsString(Error.Endpoint),
		*UEnum::GetValueAsString(Error.ErrorCode),
		*Error.DebugMessage);

	CancelActiveRequests();

	// Ensure callbacks belonging to the failed attempt are ignored.
	++ActiveAttemptId;

	SetProgress(0.0f);
	SetStatusText(
		LOCTEXT(
			"ConnectionFailedStatus",
			"Connection failed."));
	SetErrorText(UIDemo::UI::MakeUserFacingBackendError(Error));
	SetState(EBootstrapState::Error);
}

void UBootstrapViewModel::CancelActiveRequests()
{
	if (IsValid(BackendSubsystem))
	{
		BackendSubsystem->CancelRequest(
			ServiceStatusRequest);

		BackendSubsystem->CancelRequest(
			RemoteConfigRequest);

		BackendSubsystem->CancelRequest(
			PlayerProfileRequest);
	}

	ServiceStatusRequest.Reset();
	RemoteConfigRequest.Reset();
	PlayerProfileRequest.Reset();
}

void UBootstrapViewModel::ResetLoadedData()
{
	PlayerProfile = FPlayerProfile();
	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(PlayerProfile);

	RemoteConfig = FRemoteConfig();
	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(RemoteConfig);
}

void UBootstrapViewModel::SetState(
	const EBootstrapState NewState)
{
	if (State != NewState)
	{
		State = NewState;
		UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(State);
	}

	const bool bNewIsLoading =
		NewState == EBootstrapState::CheckingServices ||
		NewState == EBootstrapState::LoadingAccount;

	if (bIsLoading != bNewIsLoading)
	{
		bIsLoading = bNewIsLoading;
		UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(bIsLoading);
	}

	const bool bNewCanRetry =
		NewState == EBootstrapState::Error;

	if (bCanRetry != bNewCanRetry)
	{
		bCanRetry = bNewCanRetry;
		UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(bCanRetry);
	}
}

void UBootstrapViewModel::SetStatusText(
	const FText& NewStatusText)
{
	if (StatusText.EqualTo(NewStatusText))
	{
		return;
	}

	StatusText = NewStatusText;
	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(StatusText);
}

void UBootstrapViewModel::SetErrorText(
	const FText& NewErrorText)
{
	if (ErrorText.EqualTo(NewErrorText))
	{
		return;
	}

	ErrorText = NewErrorText;
	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(ErrorText);
}

void UBootstrapViewModel::SetProgress(
	const float NewProgress)
{
	const float ClampedProgress =
		FMath::Clamp(NewProgress, 0.0f, 1.0f);

	if (FMath::IsNearlyEqual(Progress, ClampedProgress))
	{
		return;
	}

	Progress = ClampedProgress;
	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(Progress);
}

bool UBootstrapViewModel::IsCurrentAttempt(
	const int32 AttemptId) const
{
	return AttemptId == ActiveAttemptId;
}

#undef LOCTEXT_NAMESPACE
