> [English](EXAMPLES.en.md)

# HangulInputSlate — 예제 및 사용법

모든 위젯의 copy-paste 예제와 일반 레시피 모음. 한글 입력 전용 — 범위 및 설치는
[README](README.md)를 참조하세요.

- [모듈 설정](#모듈-설정)
- [Slate (C++)](#slate-c)
  - [멀티라인 텍스트 박스](#멀티라인-텍스트-박스-채팅-스타일)
  - [싱글라인 텍스트 박스](#싱글라인-텍스트-박스-검색이름-필드)
  - [Raw 위젯 (직접 테두리 구성)](#raw-위젯-직접-테두리-구성)
  - [한국어/영어 모드 토글](#한국어영어-모드-토글)
  - [텍스트 읽기 및 설정 (코드)](#텍스트-읽기-및-설정-코드)
  - [완성 미니 예제](#완성-미니-예제-입력--전송--모드-표시)
- [UMG (Blueprint)](#umg-blueprint)
- [UMG (C++)](#umg-c)
- [레시피](#레시피)
- [엔진 위젯에서 마이그레이션](#엔진-위젯에서-마이그레이션)
- [FAQ / 문제 해결](#faq--문제-해결)

---

## 모듈 설정

사용하는 모듈을 `*.Build.cs`에 추가합니다:

```csharp
// Slate 위젯 (SHangul*)
PrivateDependencyModuleNames.Add("HangulInputSlate");

// UMG 위젯 (UHangul*) — Blueprint/UMG 레이어를 사용할 경우에만
PrivateDependencyModuleNames.Add("HangulInputSlateUMG");
```

> 팁: 플러그인 유무에 관계없이 빌드되는 **선택적 의존성**으로 구성하려면 `*.Build.cs`에서
> 플러그인 존재를 감지한 뒤 매크로를 정의하세요.

---

## Slate (C++)

### 멀티라인 텍스트 박스 (채팅 스타일)

```cpp
#include "SHangulMultiLineEditableTextBox.h"

SNew(SHangulMultiLineEditableTextBox)
    .HintText(NSLOCTEXT("MyGame", "ChatHint", "메시지를 입력하세요... (Shift+Enter 줄바꿈)"))
    .ModiferKeyForNewLine(EModifierKey::Shift)          // Shift+Enter = 줄바꿈
    .OnTextChanged_Lambda([](const FText& NewText)
    {
        UE_LOG(LogTemp, Log, TEXT("현재: %s"), *NewText.ToString());
    })
    .OnTextCommitted_Lambda([this](const FText& Text, ETextCommit::Type CommitType)
    {
        if (CommitType == ETextCommit::OnEnter)
        {
            SendMessage(Text.ToString());               // 전송 로직
        }
    });
```

### 싱글라인 텍스트 박스 (검색/이름 필드)

```cpp
#include "SHangulEditableTextBox.h"

SNew(SHangulEditableTextBox)
    .HintText(NSLOCTEXT("MyGame", "SearchHint", "검색어..."))
    .OnTextChanged_Lambda([this](const FText& NewText)
    {
        ApplyFilter(NewText.ToString());                // 입력 중 필터 적용
    });
```

### Raw 위젯 (직접 테두리 구성)

`…Box` 위젯은 스타일이 적용된 테두리를 포함합니다. 직접 테두리를 구성하고
싶다면 Raw 위젯을 사용해 감싸세요:

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
    .AutoWrapText(true)                                  // OS IME가 분리되므로 줄바꿈 안전
];
```

> 싱글라인 Raw 위젯: `SHangulEditableText`.

### 한국어/영어 모드 토글

위젯은 **조합 모드** 상태에서 한글을 조합하고, 그 외에는 ASCII를 입력합니다. 사용자는
`Ctrl+Space` 또는 하드웨어 한/영 키로 전환합니다. 직접 버튼을 추가하려면:

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

영어 모드로 시작하기:

```cpp
SNew(SHangulEditableTextBox)
    .StartInComposeMode(false);   // ASCII 모드로 시작
```

모드 변경에 반응하기 (Raw 위젯은 델리게이트를 멤버로 노출):

```cpp
Editor->OnComposeModeChanged.BindLambda([]()
{
    // 표시 업데이트, 효과음 재생 등
});
```

### 텍스트 읽기 및 설정 (코드)

```cpp
Input->SetText(FText::FromString(TEXT("안녕하세요")));   // 조합 완료 후 텍스트 설정
FText Current = Input->GetText();
Input->InsertTextAtCursor(TEXT(" 추가"));               // 멀티라인 박스 전용
Input->FlushComposition();                              // 진행 중인 음절 즉시 확정
```

### 완성 미니 예제 (입력 + 전송 + 모드 표시)

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

1. **위젯 추가:** UMG 디자이너 팔레트(*Input* 카테고리)에서 **Hangul Editable Text Box**,
   **Hangul Multi-Line Editable Text Box**, 또는 테두리 없는 **Hangul Editable Text** /
   **Hangul Multi-Line Editable Text**를 드래그합니다.
2. **설정:** Details 패널에서 `Text`와 `Hint Text`를 설정합니다.
3. **이벤트 처리:** 위젯 이벤트에서 **On Text Changed** / **On Text Committed**를 바인딩합니다.
4. **Blueprint 함수 호출:** `Set Text`, `Get Text`, `Set Compose Mode`,
   `Toggle Compose Mode`, `Is Compose Mode`.

Enter로 채팅 전송하는 일반 그래프:

```
[On Text Committed] --(Commit Method == OnEnter)--> [Branch:true]
   --> [Send Chat (Text)]
   --> [MyHangulBox -> Set Text (empty)]
```

한/A 토글 버튼:

```
[Button OnClicked] --> [MyHangulBox -> Toggle Compose Mode]
[Button GetText]   <-- [Select(Is Compose Mode ? "한" : "A")]
```

---

## UMG (C++)

`meta = (BindWidget)`으로 디자이너에서 배치한 위젯 바인딩:

```cpp
#include "Blueprint/UserWidget.h"
#include "HangulMultiLineEditableTextBox.h"
#include "MyChatUserWidget.generated.h"

UCLASS()
class UMyChatUserWidget : public UUserWidget
{
    GENERATED_BODY()
protected:
    // 변수명은 UMG 디자이너에서의 위젯 이름과 일치해야 합니다.
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
        // Text.ToString() 전송 후:
        InputBox->SetText(FText::GetEmpty());
    }
}
```

런타임에 UserWidget 트리 안에서 생성:

```cpp
UHangulEditableTextBox* Box = WidgetTree->ConstructWidget<UHangulEditableTextBox>();
Box->SetComposeMode(true);
MyPanel->AddChild(Box);
```

---

## 레시피

| 목표 | 방법 |
|---|---|
| Enter로 채팅 전송 | `OnTextCommitted` + `CommitMethod == ETextCommit::OnEnter` 확인 |
| Shift+Enter 줄바꿈 멀티라인 | `.ModiferKeyForNewLine(EModifierKey::Shift)` (기본값) |
| 입력 중 필터 적용 | `OnTextChanged` |
| 영어 모드로 시작 | `.StartInComposeMode(false)` (Slate) / 디자이너에서 체크 해제 + `SetComposeMode(false)` |
| 한/A 토글 버튼 | `IsComposeMode()` 읽기, `ToggleComposeMode()` 호출 |
| 모드 변경 반응 | `OnComposeModeChanged` 바인딩 (Slate) / UMG 이벤트는 현재 미제공 — `IsComposeMode()` 폴링 |
| 입력 초기화 | `FlushComposition()` 후 `SetText(FText::GetEmpty())` |
| 포커스 강제 설정 | `FSlateApplication::Get().SetUserFocus(0, Widget, EFocusCause::SetDirectly)` |

---

## 엔진 위젯에서 마이그레이션

`SNew`/`SAssignNew` 호출 위치에서 찾아 바꾸기:

| 엔진 | HangulInputSlate |
|---|---|
| `SEditableText` | `SHangulEditableText` |
| `SEditableTextBox` | `SHangulEditableTextBox` |
| `SMultiLineEditableText` | `SHangulMultiLineEditableText` |
| `SMultiLineEditableTextBox` | `SHangulMultiLineEditableTextBox` |
| `UEditableText` (UMG) | `UHangulEditableText` |
| `UEditableTextBox` (UMG) | `UHangulEditableTextBox` |
| `UMultiLineEditableText` (UMG) | `UHangulMultiLineEditableText` |
| `UMultiLineEditableTextBox` (UMG) | `UHangulMultiLineEditableTextBox` |

일반 인수/메서드(HintText, AutoWrapText, IsReadOnly, ModiferKeyForNewLine, OnTextChanged,
OnTextCommitted, SetText/GetText/InsertTextAtCursor)는 원본과 동일합니다. **현재 미반영:**
`SetError`/유효성 검사 시각 효과, 일부 고급 스타일링 — 이 기능이 필요하면 Raw `SHangul…`
위젯 + 직접 만든 테두리를 사용하세요.

---

## FAQ / 문제 해결

**일본어 / 중국어가 조합되지 않습니다.** 의도된 동작입니다. 사전 변환 + 후보 선택 창이
필요하여 OS IME 분리 방식으로는 제공할 수 없습니다. 이 플러그인은 한국어 전용입니다.
README의 *지원 범위*를 참고하세요.

**Raw `SHangulMultiLineEditableText`에 배경이 없습니다.** Raw 위젯은 테두리가 없습니다
(엔진의 Raw `SMultiLineEditableText`와 동일). `…Box` 변형을 사용하거나, `SBorder`로
직접 감싸세요.

**한국어/영어를 어떻게 전환하나요?** `Ctrl+Space`, 하드웨어 한/영 키, 또는
`ToggleComposeMode()`를 호출하는 버튼으로 전환합니다.

**Mac/Linux에서 조합이 되지 않습니다.** OS IME 우회는 Windows 전용입니다
(`ImmAssociateContextEx`). 다른 플랫폼에서는 위젯이 일반 편집 텍스트로 동작합니다.

**긴 한글 줄이 깨졌었습니다 (UE-66315).** 이 플러그인이 OS IME를 분리하여 수정하는 바로
그 버그입니다. 여전히 발생한다면, 위젯을 실제로 교체했는지(플러그인 임포트만으로는 부족),
텍스트 박스에 키보드 포커스가 있는지 확인하세요.
