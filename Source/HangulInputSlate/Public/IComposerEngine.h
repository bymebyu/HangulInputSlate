#pragma once

#include "CoreMinimal.h"

/**
 * Language-neutral composition engine: turns typed QWERTY characters into composed
 * script (e.g. Hangul syllables), one character at a time.
 *
 * The widget glue (FHangulInputController) detaches the OS IME and feeds raw keystrokes
 * here; the engine reports what became FINAL (committed) and what is still IN PROGRESS
 * (the pre-edit, rendered as the trailing character of the text box). This keeps the
 * widget/controller language-agnostic — adding a new deterministic language is just a new
 * implementation of this interface (see MakeComposer in HangulInputLanguage.h).
 */
class IComposerEngine
{
public:
	virtual ~IComposerEngine() = default;

	/**
	 * Feed one typed character (already resolved for Shift, e.g. 'r' or 'R').
	 * @param OutCommitted  text that became FINAL this step (insert before the pre-edit slot; may be empty)
	 * @param OutPreedit    the new in-progress unit to occupy the pre-edit slot (may be empty)
	 * @return true if the char was consumed. If false, it is not a composition key — the caller should
	 *         flush (Reset) and let the character be inserted normally.
	 */
	virtual bool HandleChar(TCHAR InChar, FString& OutCommitted, FString& OutPreedit) = 0;

	/**
	 * Backspace within the in-progress unit (decomposes one step).
	 * @return true if it consumed a step (OutPreedit = new pre-edit, possibly empty);
	 *         false if there is no pre-edit (caller should perform a normal backspace).
	 */
	virtual bool HandleBackspace(FString& OutPreedit) = 0;

	/** Whether a unit is currently in progress. */
	virtual bool HasPreedit() const = 0;

	/** Number of characters the current pre-edit occupies in the text box (0 or 1). */
	virtual int32 PreeditLen() const = 0;

	/** Finalize/abandon the in-progress unit (it stays in the text as-is) and clear state. */
	virtual void Reset() = 0;
};
