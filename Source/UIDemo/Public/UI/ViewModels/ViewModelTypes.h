#pragma once

#include "CoreMinimal.h"
#include "ViewModelTypes.generated.h"

/** @brief Shared lifecycle states for asynchronous data-backed screens. */
UENUM(BlueprintType)
enum class EAsyncViewState : uint8
{
	/** The screen has not started a request. */
	Idle,
	/** The screen is waiting for an asynchronous response. */
	Loading,
	/** The request succeeded and displayable data is available. */
	Content,
	/** The request succeeded but returned no displayable data. */
	Empty,
	/** The request failed and recovery UI should be shown. */
	Error
};

/** @brief Presentation states specific to the matchmaking flow. */
UENUM(BlueprintType)
enum class EMatchmakingViewState : uint8
{
	/** No matchmaking request has started. */
	Idle,
	/** A matchmaking ticket is waiting for completion. */
	Searching,
	/** Matchmaking returned a valid session. */
	MatchFound,
	/** The user cancelled the active search. */
	Cancelled,
	/** Matchmaking failed and retry UI should be shown. */
	Error
};
