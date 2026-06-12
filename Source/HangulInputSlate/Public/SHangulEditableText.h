#pragma once

#include "CoreMinimal.h"
#include "Widgets/Input/SEditableText.h"
#include "HangulInputController.h"
#include "HangulInputLanguage.h"

/**
 * Single-line SEditableText that composes Hangul itself, bypassing the OS IME (works around UE-66315).
 * Single-line variant of SHangulMultiLineEditableText.
 *
 * Verified against UE 5.7: SEditableText (Widgets/Input/SEditableText.h) exposes the same protected
 * `TUniquePtr<FSlateEditableTextLayout> EditableTextLayout` and public GetTextInputMethodContext()
 * as SMultiLineEditableText, so the composition access pattern matches the multi-line widget.
 */
class HANGULINPUTSLATE_API SHangulEditableText : public SEditableText
{
public:
	using FArguments = SEditableText::FArguments;

	bool IsComposeMode() const { return Controller.IsComposeMode(); }
	void SetComposeMode(bool bInCompose)
	{
		if (Controller.IsComposeMode() != bInCompose)
		{
			Controller.SetComposeMode(bInCompose);
			OnComposeModeChanged.ExecuteIfBound();
		}
	}
	void ToggleComposeMode() { SetComposeMode(!Controller.IsComposeMode()); }
	void FlushComposition() { Controller.FlushComposition(); }
	void SetInputLanguage(EHangulInputLanguage InLanguage) { Controller.SetLanguage(InLanguage); }

	/** Fired when compose mode toggles (한/영 key, Ctrl+Space, or ToggleComposeMode). */
	FSimpleDelegate OnComposeModeChanged;

	virtual FReply OnKeyChar(const FGeometry& MyGeometry, const FCharacterEvent& InCharEvent) override;
	virtual FReply OnKeyDown(const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent) override;
	virtual FReply OnFocusReceived(const FGeometry& MyGeometry, const FFocusEvent& InFocusEvent) override;
	virtual void OnFocusLost(const FFocusEvent& InFocusEvent) override;

private:
	FHangulInputController Controller;
};
