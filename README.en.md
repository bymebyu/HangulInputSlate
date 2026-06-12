> [한국어](README.md)

# HangulInputSlate

Korean (Hangul) text input for **Unreal Engine 5.7** Slate & UMG that works around the engine's
IME composition defect (**UE-66315**) by **detaching the OS IME and composing Hangul itself**.

Drop-in replacements for the four Slate editable-text widgets, plus UMG (game UI / Blueprint) wrappers.

> 📖 **Examples & recipes:** see **[EXAMPLES.en.md](EXAMPLES.en.md)** — copy-paste samples for every widget
> (Slate & UMG) plus common patterns (chat input, search field, 한/A toggle, migration from engine widgets).

> **Why:** UE Slate never notifies the Windows TSF IME of direct (non-IME) text edits, so the IME's
> composition anchor drifts and Hangul input wedges once you mix spaces/ASCII. The bug lives in code
> shared by every Slate editable-text widget, so it affects the editor *and* packaged games on Windows.
> The only deployable fix on a launcher-installed engine is to bypass the OS IME entirely and run a
> 두벌식 (2-set) composition automaton on the raw keystrokes.

## Scope

- ✅ **Korean only** (두벌식). The architecture has a language slot (`IComposerEngine` + `MakeComposer`),
  but only Korean is implemented.
- ❌ **Japanese / Chinese are NOT supported.** They require dictionary conversion + a candidate-selection
  window, which the OS-IME-detach approach cannot provide. (Vietnamese Telex/VNI is deterministic and
  *could* be added later, but is not implemented.)
- 🪟 **Windows only** for the actual IME bypass. On Mac/Linux the widgets compile and behave as plain
  editable text (no composition), since `ImmAssociateContextEx` is Windows-specific.

## Installation

1. Copy the `HangulInputSlate/` folder into your project's `Plugins/` directory.
2. Enable the plugin (Edit → Plugins → *Hangul Input (Slate)*), or ensure it is `Enabled` in your
   `.uproject` / depending plugin.
3. Rebuild.

The plugin ships two **Runtime** modules:

| Module | Depends on | Contents |
|---|---|---|
| `HangulInputSlate` | Core, InputCore, Slate, SlateCore, ApplicationCore (no UObject) | composer, controller, `SHangul*` Slate widgets |
| `HangulInputSlateUMG` | + CoreUObject, Engine, UMG | `UHangul*` UMG/Blueprint widgets |

If you only need the Slate widgets (e.g. editor tooling), depend on `HangulInputSlate` alone.

## Usage — Slate (C++)

Swap the engine widget for its `SHangul…` equivalent. Arguments are the commonly-used subset of the
originals.

```cpp
#include "SHangulMultiLineEditableTextBox.h"

SNew(SHangulMultiLineEditableTextBox)
    .HintText(NSLOCTEXT("MyGame", "Hint", "메시지를 입력하세요..."))
    .OnTextChanged(this, &SMyWidget::HandleTextChanged)
    .OnTextCommitted(this, &SMyWidget::HandleTextCommitted);
```

| Engine widget | Drop-in |
|---|---|
| `SEditableText` | `SHangulEditableText` |
| `SEditableTextBox` | `SHangulEditableTextBox` |
| `SMultiLineEditableText` | `SHangulMultiLineEditableText` |
| `SMultiLineEditableTextBox` | `SHangulMultiLineEditableTextBox` |

## Usage — UMG (Blueprint / game UI)

The palette gains **Hangul Editable Text Box**, **Hangul Multi-Line Editable Text Box**, and the
borderless **Hangul Editable Text** / **Hangul Multi-Line Editable Text** under the *Input* category.
They expose `Text`, `HintText`, `OnTextChanged`, `OnTextCommitted`, and `SetText` / `GetText` /
`SetComposeMode` / `ToggleComposeMode` / `IsComposeMode`.

## Mode switching (Korean / English)

- **Ctrl + Space** — toggle on any keyboard layout.
- **한/영 (Hangul) key** — toggled via `VK_HANGUL` (Windows).
- **`ToggleComposeMode()`** — call from your own UI button (e.g. a 한/A toggle).

## Supported API & known gaps

- Mirrors the *common* surface of the engine widgets (HintText, AutoWrapText, IsReadOnly,
  ModiferKeyForNewLine, OnTextChanged, OnTextCommitted, SetText/GetText/InsertTextAtCursor).
- **Not** mirrored in v1: `SetError`/validation visuals, full focus/error style states, and some
  advanced styling. Use the raw `SHangul…` widget + your own chrome if you need those.
- The `…Box` wrappers use `FCoreStyle`'s `NormalEditableTextBox` style by default; pass `.Style(...)`
  to match your project's look.

## Requirements & caveats

- **UE 5.7.** The Slate widgets subclass `SEditableText` / `SMultiLineEditableText` and access their
  internal `FSlateEditableTextLayout` to drive the composition. This is an **engine-internal API
  dependency** and may need adjustment on other engine versions.
- Source is UTF-8; the engine is built with `/utf-8`.

## License

MIT — see [LICENSE](LICENSE) for details.

The Hangul jamo tables are original work (no libhangul code copied; algorithm referenced only),
so there is no LGPL obligation.
