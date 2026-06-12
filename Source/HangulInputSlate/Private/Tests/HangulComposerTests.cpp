#include "CoreMinimal.h"
#include "Misc/AutomationTest.h"
#include "Composers/HangulComposer.h"
#include "HangulInputLanguage.h"
#include "IComposerEngine.h"

#if WITH_DEV_AUTOMATION_TESTS

// Expected Hangul uses raw UTF-8 literals, consistent with this codebase (UE compiles source as
// /utf-8; the existing HangulComposer.cpp likewise stores Hangul without a BOM and builds fine).

namespace
{
	// Mirror the widget's pre-edit model: replay typed keys through a composer and return the
	// resulting text. committed text accumulates; the trailing pre-edit is appended at the end.
	// A non-jamo key finalizes the current pre-edit, then inserts the literal character.
	FString Compose(const TCHAR* Keys)
	{
		FHangulComposer C;
		FString Text, Preedit, Committed, NewPreedit;
		for (const TCHAR* P = Keys; *P; ++P)
		{
			if (C.HandleChar(*P, Committed, NewPreedit))
			{
				Text += Committed;
				Preedit = NewPreedit;
			}
			else
			{
				Text += Preedit;
				C.Reset();
				Preedit.Reset();
				Text.AppendChar(*P);
			}
		}
		return Text + Preedit;
	}
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FHangulComposer_BasicSyllables,
	"HangulInputSlate.Composer.BasicSyllables",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

bool FHangulComposer_BasicSyllables::RunTest(const FString& Parameters)
{
	// 안녕하세요
	TestEqual(TEXT("annyeonghaseyo"), Compose(TEXT("dkssudgktpdy")),
		FString(TEXT("안녕하세요")));
	// 값 (cluster final ㅄ)
	TestEqual(TEXT("gabs cluster final"), Compose(TEXT("rkqt")), FString(TEXT("값")));
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FHangulComposer_LiaisonAndCompoundVowels,
	"HangulInputSlate.Composer.LiaisonAndCompoundVowels",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

bool FHangulComposer_LiaisonAndCompoundVowels::RunTest(const FString& Parameters)
{
	// 닭 + 모음 -> 연음 -> 달가 (cluster ㄺ splits: ㄹ stays, ㄱ migrates to next initial)
	TestEqual(TEXT("dalg liaison"), Compose(TEXT("ekfrk")), FString(TEXT("달가")));
	// 의 / 왜 / 위 (compound vowels ㅢ ㅙ ㅟ)
	TestEqual(TEXT("ui"), Compose(TEXT("dml")), FString(TEXT("의")));
	TestEqual(TEXT("wae"), Compose(TEXT("dho")), FString(TEXT("왜")));
	TestEqual(TEXT("wi"), Compose(TEXT("dnl")), FString(TEXT("위")));
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FHangulComposer_Backspace,
	"HangulInputSlate.Composer.Backspace",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

bool FHangulComposer_Backspace::RunTest(const FString& Parameters)
{
	FString Committed, Preedit;

	// 닭 (ㄷㅏㄺ) backspace -> 달 (cluster final ㄺ -> ㄹ)
	{
		FHangulComposer C;
		C.HandleChar('e', Committed, Preedit);
		C.HandleChar('k', Committed, Preedit);
		C.HandleChar('f', Committed, Preedit);
		C.HandleChar('r', Committed, Preedit); // 닭
		TestTrue(TEXT("backspace consumed (cluster)"), C.HandleBackspace(Preedit));
		TestEqual(TEXT("dak after cluster backspace"), Preedit, FString(TEXT("달")));
	}

	// 가 (ㄱㅏ) backspace -> ㄱ (vowel removed), backspace -> empty, backspace -> false
	{
		FHangulComposer C;
		C.HandleChar('r', Committed, Preedit);
		C.HandleChar('k', Committed, Preedit); // 가
		TestTrue(TEXT("backspace removes vowel"), C.HandleBackspace(Preedit));
		TestEqual(TEXT("lone initial g"), Preedit, FString(TEXT("ㄱ")));
		TestTrue(TEXT("backspace removes initial"), C.HandleBackspace(Preedit));
		TestEqual(TEXT("empty pre-edit"), Preedit, FString());
		TestFalse(TEXT("no more pre-edit -> normal backspace"), C.HandleBackspace(Preedit));
	}

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FHangulComposer_Factory,
	"HangulInputSlate.Composer.Factory",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

bool FHangulComposer_Factory::RunTest(const FString& Parameters)
{
	TUniquePtr<IComposerEngine> Engine = MakeComposer(EHangulInputLanguage::Korean);
	TestTrue(TEXT("Korean composer created"), Engine.IsValid());
	if (Engine.IsValid())
	{
		FString Committed, Preedit;
		TestTrue(TEXT("consumes jamo"), Engine->HandleChar('r', Committed, Preedit));
		TestEqual(TEXT("lone g via interface"), Preedit, FString(TEXT("ㄱ")));
	}
	return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS
