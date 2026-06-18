> [한국어](EXAMPLES.md)

# HangulInputSlate — Examples & Usage

Copy-paste examples for every widget, plus common recipes. Korean (Hangul) input only — see the
[README](README.en.md) for scope and installation.

- [Module setup](#module-setup)
- [Slate (C++)](#slate-c)
  - [Multi-line text box](#multi-line-text-box-chat-style)
  - [Single-line text box](#single-line-text-box-searchname-field)
  - [Raw widgets (your own border)](#raw-widgets-your-own-border)
  - [Korean/English mode toggle](#koreanenglish-mode-toggle)
  - [Reading & setting text programmatically](#reading--setting-text-programmatically)
  - [Complete mini example](#complete-mini-example-input--send--mode-indicator)
- [UMG (Blueprint)](#umg-blueprint)
- [UMG (C++)](#umg-c)
- [Recipes](#recipes)
- [Migration from engine widgets](#migration-from-engine-widgets)
- [FAQ / Troubleshooting](#faq--troubleshooting)

---

## Module setup

Add the module(s) you use to your `*.Build.cs`:

```csharp
// Slate widgets (SHangul*)
PrivateDependencyModuleNames.Add("HangulInputSlate");

// UMG widgets (UHangul*) — only if you use the Blueprint/UMG layer
PrivateDependencyModuleNames.Add("HangulInputSlateUMG");
```

> Tip: if you want the dependency to be **optional** (build with or without the plugin), detect the
> plugin in `*.Build.cs` and define a macro.

---

## Slate (C++)

### Multi-line text box (chat style)

```cpp
#include "SHangulMultiLineEditableTextBox.h"

SNew(SHangulMultiLineEditableTextBox)
    .HintText(NSLOCTEXT("MyGame", "ChatHint", "메시지를 입력하세요... (Shift+Enter 줄바꿈)"))
    .ModiferKeyForNewLine(EModifierKey::Shift)          // Shift+Enter = newline
    .OnTextChanged_Lambda([](const FText& NewText)
    {
        UE_LOG(LogTemp, Log, TEXT("Now: %s"), *NewText.ToString());
    })
    .OnTextCommitted_Lambda([this](const FText& Text, ETextCommit::Type CommitType)
    {
        if (CommitType == ETextCommit::OnEnter)
        {
            SendMessage(Text.ToString());               // your send logic
        }
    });
```

### Single-line text box (search/name field)

```cpp
#include "SHangulEditableTextBox.h"

SNew(SHangulEditableTextBox)
    .HintText(NSLOCTEXT("MyGame", "SearchHint", "검색어..."))
    .OnTextChanged_Lambda([this](const FText& NewText)
    {
        ApplyFilter(NewText.ToString());                // filter-as-you-type
    });
```

### Raw widgets (your own border)

The `…Box` widgets include a styled border. If you want to supply your own chrome, use the raw widget
and wrap it yourself:

```cpp
#include "SHangulMultiLineEditableText.h"
#include "Widgets/Layout/SBorder.h"

TSharedPtr<SHangulMultiLineEditableText> Editor;

SNew(SBorder)
.BorderImage(&FAppStyle::Get().GetWidgetStyle<FEditableTextBoxStyle>("NormalEditableTextBox").BackgroundImageNormal)
.Padding(FMargin(6.f, 4.f))
[
    SAssignNew(Editor, SHangulMultiLineEditableText)
    .HintText(NSLOCTEXT("MyGame", "Hint", "..."))
    .AutoWrapText(true)                                  // wrapping is safe — the OS IME is detached
];
```

> The raw widgets also exist for single line: `SHangulEditableText`.

### Korean/English mode toggle

The widget composes Hangul while in **compose mode** and types plain ASCII otherwise. Users switch with
`Ctrl+Space` or the hardware 한/영 key automatically. To add your own button:

```cpp
TSharedPtr<SHangulMultiLineEditableTextBox> Input;
// ... SAssignNew(Input, SHangulMultiLineEditableTextBox) ...

SNew(SButton)
.Text_Lambda([this]()
{
    return (Input.IsValid() && Input->IsComposeMode())
        ? FText::FromString(TEXT("한")) : FText::FromString(TEXT("A"));
})
.OnClicked_Lambda([this]()
{
    if (Input.IsValid()) { Input->ToggleComposeMode(); }
    return FReply::Handled();
});
```

Start in English instead of Korean:

```cpp
SNew(SHangulEditableTextBox)
    .StartInComposeMode(false);   // begins in plain-ASCII mode
```

React to mode changes (raw widget exposes the delegate as a member):

```cpp
Editor->OnComposeModeChanged.BindLambda([]()
{
    // update an indicator, play a sound, etc.
});
```

### Reading & setting text programmatically

```cpp
Input->SetText(FText::FromString(TEXT("안녕하세요")));   // finalizes composition, then sets
FText Current = Input->GetText();
Input->InsertTextAtCursor(TEXT(" 추가"));               // multi-line box only
Input->FlushComposition();                              // finalize the in-progress syllable
```

### Complete mini example (input + send + mode indicator)

```cpp
// SMyChatInput.h
#include "Widgets/SCompoundWidget.h"
class SHangulMultiLineEditableTextBox;

class SMyChatInput : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS(SMyChatInput) {}
        SLATE_EVENT(FSimpleDelegate, OnSend)
    SLATE_END_ARGS()
    void Construct(const FArguments& InArgs);
    FText GetText() const;
    void Clear();
private:
    TSharedPtr<SHangulMultiLineEditableTextBox> Input;
    FSimpleDelegate OnSend;
};
```

```cpp
// SMyChatInput.cpp
#include "SMyChatInput.h"
#include "SHangulMultiLineEditableTextBox.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/SBoxPanel.h"

void SMyChatInput::Construct(const FArguments& InArgs)
{
    OnSend = InArgs._OnSend;
    ChildSlot
    [
        SNew(SHorizontalBox)
        + SHorizontalBox::Slot().FillWidth(1.f)
        [
            SAssignNew(Input, SHangulMultiLineEditableTextBox)
            .HintText(NSLOCTEXT("MyGame", "Hint", "메시지..."))
            .OnTextCommitted_Lambda([this](const FText&, ETextCommit::Type Commit)
            {
                if (Commit == ETextCommit::OnEnter) { OnSend.ExecuteIfBound(); }
            })
        ]
        + SHorizontalBox::Slot().AutoWidth()
        [
            SNew(SButton)
            .Text_Lambda([this]() { return Input->IsComposeMode()
                ? FText::FromString(TEXT("한")) : FText::FromString(TEXT("A")); })
            .OnClicked_Lambda([this]() { Input->ToggleComposeMode(); return FReply::Handled(); })
        ]
        + SHorizontalBox::Slot().AutoWidth()
        [
            SNew(SButton).Text(NSLOCTEXT("MyGame", "Send", "전송"))
            .OnClicked_Lambda([this]() { OnSend.ExecuteIfBound(); return FReply::Handled(); })
        ]
    ];
}

FText SMyChatInput::GetText() const { return Input->GetText(); }
void SMyChatInput::Clear() { Input->FlushComposition(); Input->SetText(FText::GetEmpty()); }
```

---

## UMG (Blueprint)

1. **Add the widget**: in the UMG Designer palette (category *Input*) drag in **Hangul Editable Text Box**,
   **Hangul Multi-Line Editable Text Box**, or the borderless **Hangul Editable Text** /
   **Hangul Multi-Line Editable Text**.
2. **Set up**: in the Details panel set `Text` and `Hint Text`.
3. **Handle events**: in the widget's events, bind **On Text Changed** / **On Text Committed**.
4. **Call functions** from Blueprint graphs: `Set Text`, `Get Text`, `Set Compose Mode`,
   `Toggle Compose Mode`, `Is Compose Mode`.

A typical "send chat on Enter" graph:

```
[On Text Committed] --(Commit Method == OnEnter)--> [Branch:true]
   --> [Send Chat (Text)]
   --> [MyHangulBox -> Set Text (empty)]
```

A 한/A toggle button:

```
[Button OnClicked] --> [MyHangulBox -> Toggle Compose Mode]
[Button GetText]   <-- [Select(Is Compose Mode ? "한" : "A")]
```

---

## UMG (C++)

Bind a designer-placed widget with `meta = (BindWidget)`:

```cpp
#include "Blueprint/UserWidget.h"
#include "HangulMultiLineEditableTextBox.h"
#include "MyChatUserWidget.generated.h"

UCLASS()
class UMyChatUserWidget : public UUserWidget
{
    GENERATED_BODY()
protected:
    // The variable name must match the widget's name in the UMG Designer.
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UHangulMultiLineEditableTextBox> InputBox;

    virtual void NativeConstruct() override;

    UFUNCTION()
    void HandleCommitted(const FText& Text, ETextCommit::Type CommitMethod);
};
```

```cpp
#include "MyChatUserWidget.h"

void UMyChatUserWidget::NativeConstruct()
{
    Super::NativeConstruct();
    if (InputBox)
    {
        InputBox->OnTextCommitted.AddDynamic(this, &UMyChatUserWidget::HandleCommitted);
    }
}

void UMyChatUserWidget::HandleCommitted(const FText& Text, ETextCommit::Type CommitMethod)
{
    if (CommitMethod == ETextCommit::OnEnter)
    {
        // send Text.ToString(), then:
        InputBox->SetText(FText::GetEmpty());
    }
}
```

Create one at runtime inside a UserWidget tree:

```cpp
UHangulEditableTextBox* Box = WidgetTree->ConstructWidget<UHangulEditableTextBox>();
Box->SetComposeMode(true);
MyPanel->AddChild(Box);
```

---

## Recipes

| Goal | How |
|---|---|
| Send chat on Enter | `OnTextCommitted` + check `CommitMethod == ETextCommit::OnEnter` |
| Multi-line with Shift+Enter newline | `.ModiferKeyForNewLine(EModifierKey::Shift)` (default) |
| Filter-as-you-type search | `OnTextChanged` |
| Start in English | `.StartInComposeMode(false)` (Slate) / uncheck in Designer + `SetComposeMode(false)` |
| 한/A toggle button | read `IsComposeMode()`, call `ToggleComposeMode()` |
| React to mode change | bind `OnComposeModeChanged` (Slate) / `On Compose Mode Changed` is not currently exposed as a UMG event — poll `IsComposeMode()` |
| Clear input | `FlushComposition()` then `SetText(FText::GetEmpty())` |
| Give focus | `FSlateApplication::Get().SetUserFocus(0, Widget, EFocusCause::SetDirectly)` |

---

## Migration from engine widgets

Find-and-replace (within the `SNew`/`SAssignNew` call site):

| Engine | HangulInputSlate |
|---|---|
| `SEditableText` | `SHangulEditableText` |
| `SEditableTextBox` | `SHangulEditableTextBox` |
| `SMultiLineEditableText` | `SHangulMultiLineEditableText` |
| `SMultiLineEditableTextBox` | `SHangulMultiLineEditableTextBox` |
| `UEditableText` (UMG) | `UHangulEditableText` |
| `UEditableTextBox` (UMG) | `UHangulEditableTextBox` |
| `UMultiLineEditableText` (UMG) | `UHangulMultiLineEditableText` |
| `UMultiLineEditableTextBox` (UMG) | `UHangulMultiLineEditableTextBox` |

The common arguments/methods (HintText, AutoWrapText, IsReadOnly, ModiferKeyForNewLine, OnTextChanged,
OnTextCommitted, SetText/GetText/InsertTextAtCursor) match the originals. **Not** currently mirrored:
`SetError`/validation visuals and some advanced styling — use the raw `SHangul…` widget with your own
chrome if you need those.

---

## FAQ / Troubleshooting

**Japanese / Chinese don't compose.** By design — they need dictionary conversion + a candidate window,
which the OS-IME-detach approach can't provide. This plugin is Korean only. See the README's *Scope*.

**My raw `SHangulMultiLineEditableText` has no background.** The raw widgets have no border (like the
engine's raw `SMultiLineEditableText`). Use the `…Box` variant, or wrap it in your own `SBorder`.

**How do users switch Korean/English?** `Ctrl+Space`, the hardware 한/영 key, or a button calling
`ToggleComposeMode()`.

**Nothing composes on Mac/Linux.** The OS-IME bypass is Windows-only (`ImmAssociateContextEx`). On other
platforms the widgets behave as plain editable text.

**Long Korean lines used to break (UE-66315).** That's exactly the bug this plugin fixes by detaching the
OS IME. If you still see it, confirm you replaced the widget (not just imported the plugin) and that the
text box actually has keyboard focus when typing.
