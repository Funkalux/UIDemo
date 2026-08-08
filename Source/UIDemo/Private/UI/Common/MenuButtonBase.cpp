#include "UI/Common/MenuButtonBase.h"

#include "Components/RichTextBlock.h"

void UMenuButtonBase::SetLabelText(const FText& InLabelText)
{
	if (LabelText.EqualTo(InLabelText))
	{
		return;
	}

	LabelText = InLabelText;
	RefreshLabel();
}

void UMenuButtonBase::NativePreConstruct()
{
	Super::NativePreConstruct();
	RefreshLabel();
}

void UMenuButtonBase::RefreshLabel()
{
	if (IsValid(ButtonLabel))
	{
		ButtonLabel->SetText(LabelText);
	}
}
