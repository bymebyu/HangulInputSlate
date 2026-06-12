#pragma once

#include "CoreMinimal.h"
#include "IComposerEngine.h"

/**
 * Standalone 두벌식 (2-set) Hangul input automaton, as an IComposerEngine.
 *
 * Bypasses the OS IME entirely: the widget detaches the Windows IME and feeds typed QWERTY
 * characters here; this composes Hangul syllables itself. This is the only way to work around
 * UE-66315 (Slate never reports direct, non-IME text edits to the Windows TSF IME, which
 * desyncs the Microsoft Korean IME's composition anchor and wedges composition) on an engine
 * we cannot patch.
 *
 * Model: at most ONE syllable is "in progress" (the pre-edit), represented by {Cho, Jung, Jong}.
 * Each typed jamo either modifies the current pre-edit or finalizes it and starts a new one.
 */
class HANGULINPUTSLATE_API FHangulComposer : public IComposerEngine
{
public:
	virtual bool HandleChar(TCHAR InChar, FString& OutCommitted, FString& OutPreedit) override;
	virtual bool HandleBackspace(FString& OutPreedit) override;
	virtual bool HasPreedit() const override { return Cho >= 0 || Jung >= 0; }
	virtual int32 PreeditLen() const override { return HasPreedit() ? 1 : 0; }
	virtual void Reset() override { Cho = -1; Jung = -1; Jong = 0; }

	/** Render the current {Cho, Jung, Jong} to a 0- or 1-character string. */
	FString Render() const;

private:
	int32 Cho = -1;   // initial index 0..18, or -1 (none)
	int32 Jung = -1;  // medial index 0..20, or -1 (none)
	int32 Jong = 0;   // final index 0..27 (0 = none)
};
