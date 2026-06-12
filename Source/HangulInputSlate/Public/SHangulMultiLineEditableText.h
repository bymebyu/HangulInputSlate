#pragma once

#include "CoreMinimal.h"
#include "Widgets/Text/SMultiLineEditableText.h"
#include "HangulInputController.h"
#include "HangulInputLanguage.h"

/**
 * SMultiLineEditableText that composes Hangul itself, bypassing the OS IME (works around UE-66315).
 *
 * Reuses the parent's Slate arguments, so SNew(...).HintText(...).AutoWrapText(...) works unchanged.
 * Compose mode / language are configured after construction (SetComposeMode / SetInputLanguage), or
 * via the SHangulMultiLineEditableTextBox wrapper which exposes them as Slate arguments.
 */
class HANGULINPUTSLATE_API SHangulMultiLineEditableText : public SMultiLineEditableText
{
public:
	using FArguments = SMultiLineEditableText::FArguments;

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
