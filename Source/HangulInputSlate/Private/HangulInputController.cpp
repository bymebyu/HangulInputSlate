#include "HangulInputController.h"
#include "IComposerEngine.h"
#include "HangulInputSlateWin.h"
#include "Widgets/Text/SlateEditableTextLayout.h"
#include "GenericPlatform/ITextInputMethodSystem.h"
#include "Framework/Application/SlateApplication.h"
#include "InputCoreTypes.h"
#include "Input/Events.h"

FHangulInputController::FHangulInputController(EHangulInputLanguage InLanguage)
	: Language(InLanguage)
{
	Composer = MakeComposer(InLanguage);
}

FHangulInputController::~FHangulInputController() = default;

void FHangulInputController::SetLanguage(EHangulInputLanguage InLanguage)
{
	FlushComposition();
	Language = InLanguage;
	Composer = MakeComposer(InLanguage);
}

void FHangulInputController::SetComposeMode(bool bInCompose)
{
	if (bComposeMode != bInCompose)
	{
		FlushComposition();
		bComposeMode = bInCompose;
	}
}

void FHangulInputController::FlushComposition()
{
	if (Composer.IsValid())
	{
		Composer->Reset();
	}
	PreeditLen = 0;
}

void FHangulInputController::DetachOsIme(const TSharedRef<SWidget>& Owner, FSlateEditableTextLayout& Layout)
{
#if PLATFORM_WINDOWS
	// Drop the Slate-side TSF context activation too, so the TIP cannot compose for this window.
	if (ITextInputMethodSystem* const System = FSlateApplication::Get().GetTextInputMethodSystem())
	{
		const TSharedRef<ITextInputMethodContext> Context = Layout.GetTextInputMethodContext();
		if (System->IsActiveContext(Context))
		{
			System->DeactivateContext(Context);
		}
	}
	HangulInputSlateWin::SetWidgetOsImeEnabled(Owner, false);
#else
	(void)Owner;
	(void)Layout;
#endif
}

void FHangulInputController::RestoreOsIme(const TSharedRef<SWidget>& Owner)
{
#if PLATFORM_WINDOWS
	HangulInputSlateWin::SetWidgetOsImeEnabled(Owner, true);
#else
	(void)Owner;
#endif
}

bool FHangulInputController::HandleKeyChar(const FCharacterEvent& CharEvent, FSlateEditableTextLayout* Layout)
{
	if (!bComposeMode || !Composer.IsValid() || Layout == nullptr)
	{
		return false; // widget calls its base OnKeyChar
	}

	const TSharedRef<ITextInputMethodContext> Context = Layout->GetTextInputMethodContext();
	FString Committed, Preedit;
	if (Composer->HandleChar(CharEvent.GetCharacter(), Committed, Preedit))
	{
		DeleteSelectionIfAny(*Context);
		RewritePreedit(*Context, Committed, Preedit);
		return true;
	}

	// Not a jamo (space, digit, punctuation): finalize the unit, then let the widget insert normally.
	FlushComposition();
	return false;
}

FHangulInputController::EKeyDownAction FHangulInputController::HandleKeyDown(const FKeyEvent& KeyEvent, FSlateEditableTextLayout* Layout)
{
#if PLATFORM_WINDOWS
	// Hardware 한/영 key. UE has no FKey for it, so match the raw virtual-key code VK_HANGUL (0x15).
	// With the OS IME detached, this WM_KEYDOWN reaches us as an unmapped key.
	if (KeyEvent.GetKeyCode() == 0x15)
	{
		ToggleComposeMode();
		return EKeyDownAction::ModeToggled;
	}
#endif

	// Ctrl+Space toggles compose mode (reliable on any keyboard layout).
	if (KeyEvent.GetKey() == EKeys::SpaceBar && KeyEvent.IsControlDown())
	{
		ToggleComposeMode();
		return EKeyDownAction::ModeToggled;
	}

	if (bComposeMode && Composer.IsValid() && Layout != nullptr)
	{
		const FKey Key = KeyEvent.GetKey();
		if (Key == EKeys::BackSpace)
		{
			FString Preedit;
			if (Composer->HandleBackspace(Preedit))
			{
				const TSharedRef<ITextInputMethodContext> Context = Layout->GetTextInputMethodContext();
				RewritePreedit(*Context, FString(), Preedit);
				return EKeyDownAction::Handled;
			}
			// no in-progress unit -> fall through to a normal backspace
		}
		else if (Key == EKeys::Enter || Key == EKeys::Left || Key == EKeys::Right
			|| Key == EKeys::Up || Key == EKeys::Down || Key == EKeys::Home
			|| Key == EKeys::End || Key == EKeys::Escape || Key == EKeys::Tab)
		{
			FlushComposition(); // caret is moving / committing — finalize first
		}
	}

	return EKeyDownAction::NotHandled;
}

void FHangulInputController::DeleteSelectionIfAny(ITextInputMethodContext& Context)
{
	uint32 Begin = 0, Length = 0;
	ITextInputMethodContext::ECaretPosition Caret = ITextInputMethodContext::ECaretPosition::Ending;
	Context.GetSelectionRange(Begin, Length, Caret);
	if (Length > 0)
	{
		Context.SetTextInRange(Begin, Length, FString());
		Context.SetSelectionRange(Begin, 0, ITextInputMethodContext::ECaretPosition::Ending);
		if (Composer.IsValid())
		{
			Composer->Reset();
		}
		PreeditLen = 0;
	}
}

void FHangulInputController::RewritePreedit(ITextInputMethodContext& Context, const FString& Committed, const FString& NewPreedit)
{
	uint32 Begin = 0, Length = 0;
	ITextInputMethodContext::ECaretPosition Caret = ITextInputMethodContext::ECaretPosition::Ending;
	Context.GetSelectionRange(Begin, Length, Caret);

	const uint32 OldLen = static_cast<uint32>(PreeditLen);
	const uint32 DelStart = (Begin >= OldLen) ? (Begin - OldLen) : 0u;
	const FString NewText = Committed + NewPreedit;
	Context.SetTextInRange(DelStart, OldLen, NewText);
	Context.SetSelectionRange(DelStart + static_cast<uint32>(NewText.Len()), 0, ITextInputMethodContext::ECaretPosition::Ending);
	PreeditLen = NewPreedit.Len();
}
