#pragma once

#include "CoreMinimal.h"

class SWidget;

namespace HangulInputSlateWin
{
	/**
	 * Detach (bEnable=false) or restore (bEnable=true) the Windows IME for the native window
	 * backing this widget. Detaching stops the OS TSF IME from intercepting/composing keystrokes
	 * so they arrive as plain OnKeyChar and the composer can handle them — bypassing UE-66315.
	 * No-op on non-Windows platforms.
	 */
	void SetWidgetOsImeEnabled(const TSharedRef<const SWidget>& Widget, bool bEnable);
}
