#include "UI/ViewModels/ViewModelHelpers.h"

#include "Backend/BackendTypes.h"

#define LOCTEXT_NAMESPACE "ViewModelHelpers"

FText UIDemo::UI::MakeUserFacingBackendError(const FBackendError& Error)
{
	switch (Error.ErrorCode)
	{
	case EBackendErrorCode::Timeout:
		return LOCTEXT("Timeout", "The request timed out. Please try again.");
	case EBackendErrorCode::ServiceUnavailable:
		return LOCTEXT("Unavailable", "Online services are currently unavailable.");
	case EBackendErrorCode::RequestFailed:
		return LOCTEXT("RequestFailed", "The request could not be completed.");
	case EBackendErrorCode::InvalidResponse:
		return LOCTEXT("InvalidResponse", "The service returned an invalid response.");
	case EBackendErrorCode::Cancelled:
		return LOCTEXT("Cancelled", "The request was cancelled.");
	case EBackendErrorCode::InternalError:
	case EBackendErrorCode::None:
	default:
		return LOCTEXT("Unexpected", "An unexpected error occurred.");
	}
}

#undef LOCTEXT_NAMESPACE
