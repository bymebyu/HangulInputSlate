#pragma once

#include "CoreMinimal.h"
#include "HangulInputLanguage.h"
#include "Templates/UniquePtr.h"

class IComposerEngine;
class SWidget;
class FSlateEditableTextLayout;
class ITextInputMethodContext;
struct FCharacterEvent;
struct FKeyEvent;

/**
 * Language-neutral glue between an editable-text widget and an IComposerEngine.
 *
 * Owns the composer + pre-edit state, detaches/restores the OS IME, and performs the live
 * pre-edit replace through the widget's ITextInputMethodContext. Shared by the single-line and
 * multi-line Hangul widgets.
 *
 * The owning widget forwards key/focus events here and passes its (protected)
 * FSlateEditableTextLayout, so the engine-internal access stays inside the widget.
 */
class HANGULINPUTSLATE_API FHangulInputController
{
public:
	explicit FHangulInputController(EHangulInputLanguage InLanguage = EHangulInputLanguage::Korean);
	~FHangulInputController();

	void SetLanguage(EHangulInputLanguage InLanguage);
	EHangulInputLanguage GetLanguage() const { return Language; }

	bool IsComposeMode() const { return bComposeMode; }
	void SetComposeMode(bool bInCompose);
	void ToggleComposeMode() { SetComposeMode(!bComposeMode); }

	/** Finalize the in-progress unit (it stays in the text) and clear composition state. */
	void FlushComposition();

	/** Windows: detach the OS IME for this widget's window so keystrokes arrive as plain OnKeyChar. */
	void DetachOsIme(const TSharedRef<SWidget>& Owner, FSlateEditableTextLayout& Layout);
	/** Windows: restore the OS IME (call on focus lost). */
	void RestoreOsIme(const TSharedRef<SWidget>& Owner);

	/** @return true if the char was consumed as a jamo (widget must NOT call its base OnKeyChar). */
	bool HandleKeyChar(const FCharacterEvent& CharEvent, FSlateEditableTextLayout* Layout);

	enum class EKeyDownAction : uint8 { NotHandled, Handled, ModeToggled };
	EKeyDownAction HandleKeyDown(const FKeyEvent& KeyEvent, FSlateEditableTextLayout* Layout);

private:
	void RewritePreedit(ITextInputMethodContext& Context, const FString& Committed, const FString& NewPreedit);
	void DeleteSelectionIfAny(ITextInputMethodContext& Context);

	TUniquePtr<IComposerEngine> Composer;
	int32 PreeditLen = 0;
	bool bComposeMode = true;
	EHangulInputLanguage Language = EHangulInputLanguage::Korean;
};
