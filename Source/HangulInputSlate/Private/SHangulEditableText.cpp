#include "SHangulEditableText.h"
#include "Widgets/Text/SlateEditableTextLayout.h"

// Verified (UE 5.7): SEditableText exposes protected `EditableTextLayout` (TUniquePtr<FSlateEditableTextLayout>)
// and public GetTextInputMethodContext(), same as SMultiLineEditableText.

FReply SHangulEditableText::OnKeyChar(const FGeometry& MyGeometry, const FCharacterEvent& InCharEvent)
{
	if (Controller.HandleKeyChar(InCharEvent, EditableTextLayout.Get()))
	{
		return FReply::Handled();
	}
	return SEditableText::OnKeyChar(MyGeometry, InCharEvent);
}

FReply SHangulEditableText::OnKeyDown(const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent)
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
	return SEditableText::OnKeyDown(MyGeometry, InKeyEvent);
}

FReply SHangulEditableText::OnFocusReceived(const FGeometry& MyGeometry, const FFocusEvent& InFocusEvent)
{
	const FReply Reply = SEditableText::OnFocusReceived(MyGeometry, InFocusEvent);
	if (EditableTextLayout.IsValid())
	{
		Controller.DetachOsIme(SharedThis(this), *EditableTextLayout);
	}
	return Reply;
}

void SHangulEditableText::OnFocusLost(const FFocusEvent& InFocusEvent)
{
	Controller.FlushComposition();
	Controller.RestoreOsIme(SharedThis(this));
	SEditableText::OnFocusLost(InFocusEvent);
}
