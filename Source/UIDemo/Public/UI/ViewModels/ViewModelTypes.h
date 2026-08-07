#pragma once

#include "CoreMinimal.h"
#include "ViewModelTypes.generated.h"

UENUM(BlueprintType)
enum class EAsyncViewState : uint8
{
	Idle,
	Loading,
	Content,
	Empty,
	Error
};

UENUM(BlueprintType)
enum class EMatchmakingViewState : uint8
{
	Idle,
	Searching,
	MatchFound,
	Cancelled,
	Error
};
