#pragma once

#include "CoreMinimal.h"
#include "Backend/BackendTypes.h"
#include "MatchmakingTypes.generated.h"

USTRUCT(BlueprintType)
struct UIDEMO_API FMatchmakingResponse
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Matchmaking")
	bool bRequestSucceeded = false;

	UPROPERTY(BlueprintReadOnly, Category = "Matchmaking")
	FString TicketId;

	UPROPERTY(BlueprintReadOnly, Category = "Matchmaking")
	FString SessionId;

	UPROPERTY(BlueprintReadOnly, Category = "Matchmaking")
	FName PlaylistId = NAME_None;

	UPROPERTY(BlueprintReadOnly, Category = "Matchmaking")
	int32 EstimatedWaitSeconds = 0;

	UPROPERTY(BlueprintReadOnly, Category = "Matchmaking")
	FBackendError Error;
};
