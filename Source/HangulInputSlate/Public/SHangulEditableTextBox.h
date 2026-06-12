#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Styling/SlateTypes.h"
#include "Styling/CoreStyle.h"
#include "Framework/SlateDelegates.h"
#include "HangulInputLanguage.h"

class SHangulEditableText;

/**
 * Drop-in replacement for SEditableTextBox (single-line) with built-in Hangul composition.
 * Swap SNew(SEditableTextBox) -> SNew(SHangulEditableTextBox).
 *
 * Exposes the commonly-used subset of SEditableTextBox's surface (see plugin README for gaps).
 * Single-line composition relies on SHangulEditableText, whose engine access is unverified
 * (see SHangulEditableText.h).
 */
class HANGULINPUTSLATE_API SHangulEditableTextBox : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SHangulEditableTextBox)
		: _Style(&FCoreStyle::Get().GetWidgetStyle<FEditableTextBoxStyle>("NormalEditableTextBox"))
		, _HintText()
		, _IsReadOnly(false)
		, _Language(EHangulInputLanguage::Korean)
		, _StartInComposeMode(true)
	{}
		SLATE_STYLE_ARGUMENT(FEditableTextBoxStyle, Style)
		SLATE_ATTRIBUTE(FText, HintText)
		SLATE_ATTRIBUTE(bool, IsReadOnly)
		SLATE_ARGUMENT(EHangulInputLanguage, Language)
		SLATE_ARGUMENT(bool, StartInComposeMode)
		SLATE_EVENT(FOnTextChanged, OnTextChanged)
		SLATE_EVENT(FOnTextCommitted, OnTextCommitted)
		SLATE_EVENT(FSimpleDelegate, OnComposeModeChanged)
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);

	void SetText(const FText& InText);
	FText GetText() const;

	bool IsComposeMode() const;
	void SetComposeMode(bool bInCompose);
	void ToggleComposeMode();
	void FlushComposition();

	TSharedPtr<SHangulEditableText> GetEditableText() const { return EditableText; }

private:
	TSharedPtr<SHangulEditableText> EditableText;
};
