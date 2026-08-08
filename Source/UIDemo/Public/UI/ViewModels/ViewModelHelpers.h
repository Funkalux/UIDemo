#pragma once

#include "CoreMinimal.h"

struct FBackendError;

/** UI-specific helpers shared by native ViewModels. */
namespace UIDemo::UI
{
	/**
	 * @brief Converts a machine-readable backend error into localized UI copy.
	 * @param Error Backend error returned by the service facade.
	 * @return Localized message suitable for presentation to the player.
	 */
	UIDEMO_API FText MakeUserFacingBackendError(const FBackendError& Error);
}
