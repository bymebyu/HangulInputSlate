#pragma once

#include "CoreMinimal.h"
#include "InputCoreTypes.h"
#include "Widgets/SCompoundWidget.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Styling/SlateTypes.h"
#include "Styling/CoreStyle.h"
#include "Framework/SlateDelegates.h"
#include "HangulInputLanguage.h"

class SHangulMultiLineEditableText;

/**
 * Drop-in replacement for SMultiLineEditableTextBox with built-in Hangul composition.
 * Swap SNew(SMultiLineEditableTextBox) -> SNew(SHangulMultiLineEditableTextBox).
 *
 * Exposes the commonly-used subset of SMultiLineEditableTextBox's surface. Not every argument is
 * mirrored (e.g. SetError / full styling states are out of scope for v1 — see plugin README).
 */
class HANGULINPUTSLATE_API SHangulMultiLineEditableTextBox : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SHangulMultiLineEditableTextBox)
		: _Style(&FCoreStyle::Get().GetWidgetStyle<FEditableTextBoxStyle>("NormalEditableTextBox"))
		, _HintText()
		, _AutoWrapText(false)
		, _IsReadOnly(false)
		, _ModiferKeyForNewLine(EModifierKey::Shift)
		, _Language(EHangulInputLanguage::Korean)
		, _StartInComposeMode(true)
	{}
		SLATE_STYLE_ARGUMENT(FEditableTextBoxStyle, Style)
		SLATE_ATTRIBUTE(FText, HintText)
		SLATE_ATTRIBUTE(bool, AutoWrapText)
		SLATE_ATTRIBUTE(bool, IsReadOnly)
		SLATE_ARGUMENT(EModifierKey::Type, ModiferKeyForNewLine)
		SLATE_ARGUMENT(EHangulInputLanguage, Language)
		SLATE_ARGUMENT(bool, StartInComposeMode)
		SLATE_EVENT(FOnTextChanged, OnTextChanged)
		SLATE_EVENT(FOnTextCommitted, OnTextCommitted)
		SLATE_EVENT(FSimpleDelegate, OnComposeModeChanged)
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);

	void SetText(const FText& InText);
	FText GetText() const;
	void InsertTextAtCursor(const FString& InText);

	bool IsComposeMode() const;
	void SetComposeMode(bool bInCompose);
	void ToggleComposeMode();
	void FlushComposition();

	/** Access the inner editable text widget (e.g. to set keyboard focus). */
	TSharedPtr<SHangulMultiLineEditableText> GetEditableText() const { return EditableText; }

private:
	TSharedPtr<SHangulMultiLineEditableText> EditableText;
};
