#pragma once

#include "CoreMinimal.h"
#include "Templates/UniquePtr.h"

class IComposerEngine;

/**
 * Deterministic composition languages handled by the OS-IME-detach approach.
 *
 * v1 implements Korean only. Other deterministic languages (e.g. Vietnamese Telex/VNI)
 * are intentional placeholders — adding one is a new IComposerEngine implementation plus a
 * case in MakeComposer; the widgets/controller need no change. Conversion-based languages
 * (Japanese かな→漢字, Chinese 병음→漢字) are out of scope: they require a candidate window.
 */
enum class EHangulInputLanguage : uint8
{
	Korean,
	// Vietnamese,  // placeholder (Telex/VNI) — deterministic, separate state machine; not implemented
};

/** Create the composition engine for a language. Returns nullptr for unimplemented languages. */
HANGULINPUTSLATE_API TUniquePtr<IComposerEngine> MakeComposer(EHangulInputLanguage Language);
