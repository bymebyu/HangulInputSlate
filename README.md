> [English](README.en.md)

# HangulInputSlate

**Unreal Engine 5.7** Slate & UMG용 한글 입력 플러그인. 엔진의 IME 조합 결함(**UE-66315**)을
**OS IME를 분리하고 직접 한글을 조합**하는 방식으로 해결합니다.

네 가지 Slate 편집 텍스트 위젯의 drop-in 교체품과 UMG(게임 UI / Blueprint) 래퍼를 제공합니다.

> 📖 **예제 및 레시피:** **[EXAMPLES.md](EXAMPLES.md)** — 모든 위젯(Slate & UMG)의 copy-paste 샘플과
> 채팅 입력, 검색 필드, 한/A 토글, 엔진 위젯에서의 마이그레이션 등 일반 패턴 수록.

> **증상 (UE-66315):** 입력창에서 한글과 영어·공백을 번갈아 입력하다 보면 어느 순간 한글이
> 더 이상 입력되지 않고 멈추는 현상입니다. 입력창 바깥을 클릭했다가 돌아오면 잠깐 회복되지만
> 금방 다시 반복됩니다.
>
> **원인:** UE Slate는 영어·공백 같은 직접 텍스트 편집 시 Windows IME에 커서 위치 변경을
> 알리지 않습니다. IME가 조합 위치(앵커)를 잃어버리고, 이후 한글 조합이 잘못된 위치에서
> 시작되거나 완전히 깨집니다. 이 버그는 모든 Slate 편집 위젯이 공유하는 코드에 있어
> 에디터와 Windows 패키지 게임 모두에 영향을 미칩니다.
>
> **해결:** 이 플러그인은 OS IME를 완전히 분리하고, 키 입력에서 직접 **두벌식** 조합 오토마톤을
> 실행하여 문제를 우회합니다.

## 지원 범위

- ✅ **한국어만 지원** (두벌식). `IComposerEngine` + `MakeComposer` 언어 슬롯 아키텍처를 갖추고
  있으나 현재 한국어만 구현됩니다.
- ❌ **일본어 / 중국어 미지원.** 사전 변환 + 후보 선택 창이 필요하여 OS IME 분리 방식으로는
  제공 불가. (베트남어 Telex/VNI는 결정론적이라 추후 추가 가능하나 현재 미구현.)
- 🪟 **IME 우회는 Windows 전용.** Mac/Linux에서는 위젯이 컴파일되고 일반 편집 텍스트로 동작합니다
  (`ImmAssociateContextEx`가 Windows 전용).

## 설치

1. `HangulInputSlate/` 폴더를 프로젝트의 `Plugins/` 디렉토리에 복사합니다.
2. 플러그인을 활성화합니다 (편집 → 플러그인 → *Hangul Input (Slate)*), 또는 `.uproject` /
   의존 플러그인에서 `Enabled`로 설정합니다.
3. 리빌드합니다.

플러그인은 두 개의 **Runtime** 모듈을 제공합니다:

| 모듈 | 의존성 | 내용 |
|---|---|---|
| `HangulInputSlate` | Core, InputCore, Slate, SlateCore, ApplicationCore (UObject 없음) | 조합기, 컨트롤러, `SHangul*` Slate 위젯 |
| `HangulInputSlateUMG` | + CoreUObject, Engine, UMG | `UHangul*` UMG/Blueprint 위젯 |

Slate 위젯만 필요한 경우(예: 에디터 툴링) `HangulInputSlate`만 의존하면 됩니다.

## 사용법 — Slate (C++)

엔진 위젯을 `SHangul…` 등가품으로 교체합니다. 인수는 원본의 일반적으로 사용되는 부분집합입니다.

```cpp
#include "SHangulMultiLineEditableTextBox.h"

SNew(SHangulMultiLineEditableTextBox)
    .HintText(NSLOCTEXT("MyGame", "Hint", "메시지를 입력하세요..."))
    .OnTextChanged(this, &SMyWidget::HandleTextChanged)
    .OnTextCommitted(this, &SMyWidget::HandleTextCommitted);
```

| 엔진 위젯 | Drop-in 교체품 |
|---|---|
| `SEditableText` | `SHangulEditableText` |
| `SEditableTextBox` | `SHangulEditableTextBox` |
| `SMultiLineEditableText` | `SHangulMultiLineEditableText` |
| `SMultiLineEditableTextBox` | `SHangulMultiLineEditableTextBox` |

## 사용법 — UMG (Blueprint / 게임 UI)

팔레트의 *Input* 카테고리에 **Hangul Editable Text Box**, **Hangul Multi-Line Editable Text Box**,
테두리 없는 **Hangul Editable Text** / **Hangul Multi-Line Editable Text**가 추가됩니다.
`Text`, `HintText`, `OnTextChanged`, `OnTextCommitted`, `SetText` / `GetText` /
`SetComposeMode` / `ToggleComposeMode` / `IsComposeMode`를 노출합니다.

## 모드 전환 (한국어 / 영어)

- **Ctrl + Space** — 어떤 키보드 레이아웃에서도 토글.
- **한/영 키** — `VK_HANGUL`을 통한 토글 (Windows).
- **`ToggleComposeMode()`** — 직접 만든 UI 버튼에서 호출 (예: 한/A 토글 버튼).

## 지원 API 및 알려진 제한

엔진 원본 위젯과 동일한 방식으로 사용할 수 있는 파라미터·메서드:

`HintText`, `AutoWrapText`, `IsReadOnly`, `ModiferKeyForNewLine`,
`OnTextChanged`, `OnTextCommitted`, `SetText` / `GetText` / `InsertTextAtCursor`

**현재 미지원:**

- `SetError` 및 유효성 검사 시각 효과 (빨간 테두리 등)
- 포커스·에러 상태별 스타일 분리
- 일부 고급 스타일 파라미터

위 기능이 필요하다면 테두리 없는 `SHangul…` 위젯을 직접 `SBorder`나 커스텀 컨테이너로 감싸는
방식으로 더 세밀하게 제어할 수 있습니다.

**스타일:** `…Box` 위젯(테두리 포함)은 기본적으로 엔진의 `NormalEditableTextBox` 스타일을
사용합니다. `.Style(...)`을 전달하여 프로젝트 스타일을 적용할 수 있습니다.

## 요구 사항 및 주의 사항

- **UE 5.7.** Slate 위젯은 `SEditableText` / `SMultiLineEditableText`를 서브클래스로 하며
  내부 `FSlateEditableTextLayout`에 접근하여 조합을 구동합니다. 이는 **엔진 내부 API 의존성**
  이므로 다른 엔진 버전에서는 조정이 필요할 수 있습니다.
- 소스는 UTF-8이며, 엔진은 `/utf-8`로 빌드됩니다.

## 라이선스

MIT — 자세한 내용은 [LICENSE](LICENSE)를 참조하세요.

한글 자모 테이블은 오리지널 작업입니다 (libhangul 코드 미복사, 알고리즘만 참조),
LGPL 의무 없음.
