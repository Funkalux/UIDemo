#include "UI/ViewModels/SkillNodeViewModel.h"

void USkillNodeViewModel::Initialize(const FSkillNodeData& Node)
{
	SkillId = Node.SkillId;
	DisplayName = Node.DisplayName;
	Description = Node.Description;
	Cost = Node.Cost;
	PrerequisiteIds = Node.PrerequisiteIds;
	Row = Node.Row;
	Column = Node.Column;
	State = Node.State;
	bSelected = false;
}

void USkillNodeViewModel::ApplyState(const ESkillNodeState NewState)
{
	if (State != NewState)
	{
		State = NewState;
		UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(State);
	}
}

void USkillNodeViewModel::SetSelected(const bool bNewSelected)
{
	if (bSelected != bNewSelected)
	{
		bSelected = bNewSelected;
		UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(bSelected);
	}
}
