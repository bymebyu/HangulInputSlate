#include "Composers/HangulComposer.h"

namespace
{
	// ── Jamo index reference ────────────────────────────────────────────────────────────────
	// 초성 Initial (19): ㄱㄲㄴㄷㄸㄹㅁㅂㅃㅅㅆㅇㅈㅉㅊㅋㅌㅍㅎ            index 0..18
	// 중성 Medial  (21): ㅏㅐㅑㅒㅓㅔㅕㅖㅗㅘㅙㅚㅛㅜㅝㅞㅟㅠㅡㅢㅣ        index 0..20
	// 종성 Final   (28): (none)ㄱㄲㄳㄴㄵㄶㄷㄹㄺㄻㄼㄽㄾㄿㅀㅁㅂㅄㅅㅆㅇㅈㅊㅋㅌㅍㅎ  index 0..27 (0 = none)

	// Initial index -> Hangul Compatibility Jamo codepoint (for rendering a lone initial consonant).
	const TCHAR InitialToCompat[19] = {
		0x3131, 0x3132, 0x3134, 0x3137, 0x3138, 0x3139, 0x3141, 0x3142, 0x3143, 0x3145,
		0x3146, 0x3147, 0x3148, 0x3149, 0x314A, 0x314B, 0x314C, 0x314D, 0x314E
	};

	struct FJamoKey
	{
		bool bValid = false;
		bool bVowel = false;
		int32 Initial = -1;  // initial index (consonants)
		int32 Final = 0;     // final index (consonants); 0 = cannot be a final
		int32 Medial = -1;   // medial index (vowels)
	};

	// Standard 두벌식 layout: the resolved character (Shift already applied) -> jamo.
	FJamoKey MapKey(TCHAR C)
	{
		switch (C)
		{
			// Consonants: { valid, vowel=false, Initial, Final, Medial=-1 }
			case 'r': return { true, false, 0,  1,  -1 }; // ㄱ
			case 'R': return { true, false, 1,  2,  -1 }; // ㄲ
			case 's': return { true, false, 2,  4,  -1 }; // ㄴ
			case 'e': return { true, false, 3,  7,  -1 }; // ㄷ
			case 'E': return { true, false, 4,  0,  -1 }; // ㄸ (no final form)
			case 'f': return { true, false, 5,  8,  -1 }; // ㄹ
			case 'a': return { true, false, 6,  16, -1 }; // ㅁ
			case 'q': return { true, false, 7,  17, -1 }; // ㅂ
			case 'Q': return { true, false, 8,  0,  -1 }; // ㅃ (no final form)
			case 't': return { true, false, 9,  19, -1 }; // ㅅ
			case 'T': return { true, false, 10, 20, -1 }; // ㅆ
			case 'd': return { true, false, 11, 21, -1 }; // ㅇ
			case 'w': return { true, false, 12, 22, -1 }; // ㅈ
			case 'W': return { true, false, 13, 0,  -1 }; // ㅉ (no final form)
			case 'c': return { true, false, 14, 23, -1 }; // ㅊ
			case 'z': return { true, false, 15, 24, -1 }; // ㅋ
			case 'x': return { true, false, 16, 25, -1 }; // ㅌ
			case 'v': return { true, false, 17, 26, -1 }; // ㅍ
			case 'g': return { true, false, 18, 27, -1 }; // ㅎ

			// Vowels: { valid, vowel=true, Initial=-1, Final=0, Medial }
			case 'k': return { true, true, -1, 0, 0  }; // ㅏ
			case 'o': return { true, true, -1, 0, 1  }; // ㅐ
			case 'i': return { true, true, -1, 0, 2  }; // ㅑ
			case 'O': return { true, true, -1, 0, 3  }; // ㅒ
			case 'j': return { true, true, -1, 0, 4  }; // ㅓ
			case 'p': return { true, true, -1, 0, 5  }; // ㅔ
			case 'u': return { true, true, -1, 0, 6  }; // ㅕ
			case 'P': return { true, true, -1, 0, 7  }; // ㅖ
			case 'h': return { true, true, -1, 0, 8  }; // ㅗ
			case 'y': return { true, true, -1, 0, 12 }; // ㅛ
			case 'n': return { true, true, -1, 0, 13 }; // ㅜ
			case 'b': return { true, true, -1, 0, 17 }; // ㅠ
			case 'm': return { true, true, -1, 0, 18 }; // ㅡ
			case 'l': return { true, true, -1, 0, 20 }; // ㅣ

			default:  return {};
		}
	}

	// Two medials combine into a compound vowel (ㅗ+ㅏ=ㅘ ...). Returns combined medial index or -1.
	int32 ComboVowel(int32 A, int32 B)
	{
		if (A == 8  && B == 0)  return 9;  // ㅗ+ㅏ=ㅘ
		if (A == 8  && B == 1)  return 10; // ㅗ+ㅐ=ㅙ
		if (A == 8  && B == 20) return 11; // ㅗ+ㅣ=ㅚ
		if (A == 13 && B == 4)  return 14; // ㅜ+ㅓ=ㅝ
		if (A == 13 && B == 5)  return 15; // ㅜ+ㅔ=ㅞ
		if (A == 13 && B == 20) return 16; // ㅜ+ㅣ=ㅟ
		if (A == 18 && B == 20) return 19; // ㅡ+ㅣ=ㅢ
		return -1;
	}

	// Existing final + new consonant's final form combine into a cluster final (ㄹ+ㄱ=ㄺ ...).
	int32 ComboFinal(int32 A, int32 B)
	{
		if (A == 1  && B == 19) return 3;  // ㄱ+ㅅ=ㄳ
		if (A == 4  && B == 22) return 5;  // ㄴ+ㅈ=ㄵ
		if (A == 4  && B == 27) return 6;  // ㄴ+ㅎ=ㄶ
		if (A == 8  && B == 1)  return 9;  // ㄹ+ㄱ=ㄺ
		if (A == 8  && B == 16) return 10; // ㄹ+ㅁ=ㄻ
		if (A == 8  && B == 17) return 11; // ㄹ+ㅂ=ㄼ
		if (A == 8  && B == 19) return 12; // ㄹ+ㅅ=ㄽ
		if (A == 8  && B == 25) return 13; // ㄹ+ㅌ=ㄾ
		if (A == 8  && B == 26) return 14; // ㄹ+ㅍ=ㄿ
		if (A == 8  && B == 27) return 15; // ㄹ+ㅎ=ㅀ
		if (A == 17 && B == 19) return 18; // ㅂ+ㅅ=ㅄ
		return -1;
	}

	// For backspace: a cluster final reverts to its first component; a single final reverts to none (0).
	int32 ClusterFinalBase(int32 J)
	{
		switch (J)
		{
			case 3:  return 1;  // ㄳ→ㄱ
			case 5:  return 4;  // ㄵ→ㄴ
			case 6:  return 4;  // ㄶ→ㄴ
			case 9:  return 8;  // ㄺ→ㄹ
			case 10: return 8;  // ㄻ→ㄹ
			case 11: return 8;  // ㄼ→ㄹ
			case 12: return 8;  // ㄽ→ㄹ
			case 13: return 8;  // ㄾ→ㄹ
			case 14: return 8;  // ㄿ→ㄹ
			case 15: return 8;  // ㅀ→ㄹ
			case 18: return 17; // ㅄ→ㅂ
			default: return 0;  // single final removed
		}
	}

	// 연음 for a cluster final: base stays on the current syllable, the second component migrates
	// to become the next syllable's initial. Returns false for single (non-cluster) finals.
	bool SplitFinalForLiaison(int32 J, int32& OutBaseFinal, int32& OutMovedInitial)
	{
		switch (J)
		{
			case 3:  OutBaseFinal = 1;  OutMovedInitial = 9;  return true; // ㄳ→ㄱ + ㅅ
			case 5:  OutBaseFinal = 4;  OutMovedInitial = 12; return true; // ㄵ→ㄴ + ㅈ
			case 6:  OutBaseFinal = 4;  OutMovedInitial = 18; return true; // ㄶ→ㄴ + ㅎ
			case 9:  OutBaseFinal = 8;  OutMovedInitial = 0;  return true; // ㄺ→ㄹ + ㄱ
			case 10: OutBaseFinal = 8;  OutMovedInitial = 6;  return true; // ㄻ→ㄹ + ㅁ
			case 11: OutBaseFinal = 8;  OutMovedInitial = 7;  return true; // ㄼ→ㄹ + ㅂ
			case 12: OutBaseFinal = 8;  OutMovedInitial = 9;  return true; // ㄽ→ㄹ + ㅅ
			case 13: OutBaseFinal = 8;  OutMovedInitial = 16; return true; // ㄾ→ㄹ + ㅌ
			case 14: OutBaseFinal = 8;  OutMovedInitial = 17; return true; // ㄿ→ㄹ + ㅍ
			case 15: OutBaseFinal = 8;  OutMovedInitial = 18; return true; // ㅀ→ㄹ + ㅎ
			case 18: OutBaseFinal = 17; OutMovedInitial = 9;  return true; // ㅄ→ㅂ + ㅅ
			default: return false;
		}
	}

	// 연음 for a single final: the final consonant becomes the next syllable's initial.
	int32 FinalToInitial(int32 J)
	{
		switch (J)
		{
			case 1:  return 0;  // ㄱ
			case 2:  return 1;  // ㄲ
			case 4:  return 2;  // ㄴ
			case 7:  return 3;  // ㄷ
			case 8:  return 5;  // ㄹ
			case 16: return 6;  // ㅁ
			case 17: return 7;  // ㅂ
			case 19: return 9;  // ㅅ
			case 20: return 10; // ㅆ
			case 21: return 11; // ㅇ
			case 22: return 12; // ㅈ
			case 23: return 14; // ㅊ
			case 24: return 15; // ㅋ
			case 25: return 16; // ㅌ
			case 26: return 17; // ㅍ
			case 27: return 18; // ㅎ
			default: return -1;
		}
	}

	// For backspace: a compound vowel reverts to its first component; otherwise -1 (remove the vowel).
	int32 CompoundVowelBase(int32 J)
	{
		switch (J)
		{
			case 9: case 10: case 11: return 8;  // ㅘㅙㅚ→ㅗ
			case 14: case 15: case 16: return 13; // ㅝㅞㅟ→ㅜ
			case 19: return 18;                   // ㅢ→ㅡ
			default: return -1;
		}
	}
}

FString FHangulComposer::Render() const
{
	FString Out;
	if (Cho >= 0 && Jung >= 0)
	{
		// Precomposed syllable: 0xAC00 + (initial*21 + medial)*28 + final
		Out.AppendChar(static_cast<TCHAR>(0xAC00 + (Cho * 21 + Jung) * 28 + Jong));
	}
	else if (Cho >= 0)
	{
		// Lone initial consonant (compatibility jamo).
		Out.AppendChar(InitialToCompat[Cho]);
	}
	else if (Jung >= 0)
	{
		// Lone vowel (compatibility jamo block is contiguous from ㅏ = 0x314F).
		Out.AppendChar(static_cast<TCHAR>(0x314F + Jung));
	}
	return Out;
}

bool FHangulComposer::HandleChar(TCHAR InChar, FString& OutCommitted, FString& OutPreedit)
{
	OutCommitted.Empty();
	OutPreedit.Empty();

	const FJamoKey J = MapKey(InChar);
	if (!J.bValid)
	{
		return false; // not a Hangul key
	}

	if (J.bVowel)
	{
		const int32 V = J.Medial;
		if (Jung < 0)
		{
			// empty, or initial-only -> attach the medial
			Jung = V;
		}
		else if (Jong == 0)
		{
			// initial + medial, no final -> try a compound vowel; otherwise finalize and start a new orphan vowel
			const int32 Combo = ComboVowel(Jung, V);
			if (Combo >= 0)
			{
				Jung = Combo;
			}
			else
			{
				OutCommitted = Render();
				Reset();
				Jung = V;
			}
		}
		else
		{
			// initial + medial + final -> 연음: the final migrates to the next syllable's initial
			int32 BaseFinal, MovedInitial;
			if (SplitFinalForLiaison(Jong, BaseFinal, MovedInitial))
			{
				Jong = BaseFinal;
				OutCommitted = Render();
				Reset();
				Cho = MovedInitial;
				Jung = V;
			}
			else
			{
				const int32 MovedInitial2 = FinalToInitial(Jong);
				Jong = 0;
				OutCommitted = Render();
				Reset();
				Cho = MovedInitial2;
				Jung = V;
			}
		}
	}
	else
	{
		// consonant
		if (Cho < 0 && Jung < 0)
		{
			// empty -> initial
			Cho = J.Initial;
		}
		else if (Jung < 0)
		{
			// initial-only -> two consonants in a row never combine in 두벌식: finalize, start new initial
			OutCommitted = Render();
			Reset();
			Cho = J.Initial;
		}
		else if (Jong == 0)
		{
			// initial + medial, no final -> set the final if this consonant has a final form
			if (J.Final > 0)
			{
				Jong = J.Final;
			}
			else
			{
				OutCommitted = Render();
				Reset();
				Cho = J.Initial;
			}
		}
		else
		{
			// initial + medial + final -> try a cluster final; otherwise finalize and start a new initial
			const int32 Combo = (J.Final > 0) ? ComboFinal(Jong, J.Final) : -1;
			if (Combo >= 0)
			{
				Jong = Combo;
			}
			else
			{
				OutCommitted = Render();
				Reset();
				Cho = J.Initial;
			}
		}
	}

	OutPreedit = Render();
	return true;
}

bool FHangulComposer::HandleBackspace(FString& OutPreedit)
{
	if (!HasPreedit())
	{
		return false; // nothing in progress -> caller does a normal backspace
	}

	if (Jong != 0)
	{
		Jong = ClusterFinalBase(Jong); // cluster -> first component; single -> 0 (removed)
	}
	else if (Jung >= 0)
	{
		const int32 Base = CompoundVowelBase(Jung);
		Jung = (Base >= 0) ? Base : -1; // compound -> first component; simple -> removed
	}
	else // Cho >= 0
	{
		Cho = -1;
	}

	OutPreedit = Render();
	return true;
}
