#pragma once

#include "CoreMinimal.h"
#include "Components/Widget.h"
#include "Types/SlateEnums.h"
#include "HangulMultiLineEditableText.generated.h"

class SHangulMultiLineEditableText;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FHangulMultiTextChanged, const FText&, Text);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FHangulMultiTextCommitted, const FText&, Text, ETextCommit::Type, CommitMethod);

/**
 * UMG (game UI / Blueprint) multi-line editable text (no border) with built-in Hangul composition.
 * Wraps the raw SHangulMultiLineEditableText. Korean only.
 */
UCLASS(meta = (DisplayName = "Hangul Multi-Line Editable Text"))
class HANGULINPUTSLATEUMG_API UHangulMultiLineEditableText : public UWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Content", meta = (MultiLine = true))
	FText Text;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Content")
	FText HintText;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FHangulMultiTextChanged OnTextChanged;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FHangulMultiTextCommitted OnTextCommitted;

	UFUNCTION(BlueprintCallable, Category = "Hangul")
	void SetText(FText InText);

	UFUNCTION(BlueprintPure, Category = "Hangul")
	FText GetText() const;

	UFUNCTION(BlueprintCallable, Category = "Hangul")
	void SetComposeMode(bool bInCompose);

	UFUNCTION(BlueprintCallable, Category = "Hangul")
	void ToggleComposeMode();

	UFUNCTION(BlueprintPure, Category = "Hangul")
	bool IsComposeMode() const;

	//~ UWidget
	virtual void SynchronizeProperties() override;
	virtual void ReleaseSlateResources(bool bReleaseChildren) override;
#if WITH_EDITOR
	virtual const FText GetPaletteCategory() override;
#endif

protected:
	virtual TSharedRef<SWidget> RebuildWidget() override;

	void HandleTextChanged(const FText& InText);
	void HandleTextCommitted(const FText& InText, ETextCommit::Type CommitType);

	TSharedPtr<SHangulMultiLineEditableText> MyWidget;
};
