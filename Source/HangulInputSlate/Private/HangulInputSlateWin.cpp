#include "HangulInputSlateWin.h"

#if PLATFORM_WINDOWS
#include "Framework/Application/SlateApplication.h"
#include "Widgets/SWindow.h"
#include "GenericPlatform/GenericWindow.h"
#include "Windows/AllowWindowsPlatformTypes.h"
#include <windows.h>
#include <imm.h>
#include "Windows/HideWindowsPlatformTypes.h"
#endif

namespace HangulInputSlateWin
{
#if PLATFORM_WINDOWS
	static HWND GetWidgetHwnd(const TSharedRef<const SWidget>& Widget)
	{
		const TSharedPtr<SWindow> Window = FSlateApplication::Get().FindWidgetWindow(Widget);
		if (Window.IsValid() && Window->GetNativeWindow().IsValid())
		{
			return reinterpret_cast<HWND>(Window->GetNativeWindow()->GetOSWindowHandle());
		}
		return nullptr;
	}
#endif

	void SetWidgetOsImeEnabled(const TSharedRef<const SWidget>& Widget, bool bEnable)
	{
#if PLATFORM_WINDOWS
		if (HWND Hwnd = GetWidgetHwnd(Widget))
		{
			ImmAssociateContextEx(Hwnd, nullptr, bEnable ? IACE_DEFAULT : 0);
		}
#else
		(void)Widget;
		(void)bEnable;
#endif
	}
}
