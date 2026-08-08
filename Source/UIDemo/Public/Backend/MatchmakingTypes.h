#pragma once

#include "CoreMinimal.h"
#include "Backend/BackendTypes.h"
#include "MatchmakingTypes.generated.h"

/** @brief Result of an asynchronous matchmaking request. */
USTRUCT(BlueprintType)
struct UIDEMO_API FMatchmakingResponse
{
	GENERATED_BODY()

	/** Whether matchmaking completed successfully. */
	UPROPERTY(BlueprintReadOnly, Category = "Matchmaking")
	bool bRequestSucceeded = false;

	/** Backend ticket identifier associated with the search. */
	UPROPERTY(BlueprintReadOnly, Category = "Matchmaking")
	FString TicketId;

	/** Session identifier returned when a match is found. */
	UPROPERTY(BlueprintReadOnly, Category = "Matchmaking")
	FString SessionId;

	/** Playlist used to create the matchmaking ticket. */
	UPROPERTY(BlueprintReadOnly, Category = "Matchmaking")
	FName PlaylistId = NAME_None;

	/** Backend-provided estimated wait duration in seconds. */
	UPROPERTY(BlueprintReadOnly, Category = "Matchmaking")
	int32 EstimatedWaitSeconds = 0;

	/** Failure details returned when matchmaking did not succeed. */
	UPROPERTY(BlueprintReadOnly, Category = "Matchmaking")
	FBackendError Error;
};
