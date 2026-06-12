#include "SHangulMultiLineEditableText.h"
#include "Widgets/Text/SlateEditableTextLayout.h"

FReply SHangulMultiLineEditableText::OnKeyChar(const FGeometry& MyGeometry, const FCharacterEvent& InCharEvent)
{
	if (Controller.HandleKeyChar(InCharEvent, EditableTextLayout.Get()))
	{
		return FReply::Handled();
	}
	return SMultiLineEditableText::OnKeyChar(MyGeometry, InCharEvent);
}

FReply SHangulMultiLineEditableText::OnKeyDown(const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent)
{
	switch (Controller.HandleKeyDown(InKeyEvent, EditableTextLayout.Get()))
	{
	case FHangulInputController::EKeyDownAction::ModeToggled:
		OnComposeModeChanged.ExecuteIfBound();
		return FReply::Handled();
	case FHangulInputController::EKeyDownAction::Handled:
		return FReply::Handled();
	default:
		break;
	}
	return SMultiLineEditableText::OnKeyDown(MyGeometry, InKeyEvent);
}

FReply SHangulMultiLineEditableText::OnFocusReceived(const FGeometry& MyGeometry, const FFocusEvent& InFocusEvent)
{
	const FReply Reply = SMultiLineEditableText::OnFocusReceived(MyGeometry, InFocusEvent);
	if (EditableTextLayout.IsValid())
	{
		Controller.DetachOsIme(SharedThis(this), *EditableTextLayout);
	}
	return Reply;
}

void SHangulMultiLineEditableText::OnFocusLost(const FFocusEvent& InFocusEvent)
{
	Controller.FlushComposition();
	Controller.RestoreOsIme(SharedThis(this));
	SMultiLineEditableText::OnFocusLost(InFocusEvent);
}
