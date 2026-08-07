#pragma once

#include "CoreMinimal.h"

struct FBackendError;

namespace UIDemo::UI
{
	UIDEMO_API FText MakeUserFacingBackendError(const FBackendError& Error);
}
