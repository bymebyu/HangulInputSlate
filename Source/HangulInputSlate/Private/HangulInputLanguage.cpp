#include "HangulInputLanguage.h"
#include "IComposerEngine.h"
#include "Composers/HangulComposer.h"

TUniquePtr<IComposerEngine> MakeComposer(EHangulInputLanguage Language)
{
	switch (Language)
	{
	case EHangulInputLanguage::Korean:
		return MakeUnique<FHangulComposer>();
	default:
		return nullptr;
	}
}
