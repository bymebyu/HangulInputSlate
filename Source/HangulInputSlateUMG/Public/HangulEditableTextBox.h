#pragma once

#include "CoreMinimal.h"
#include "Components/Widget.h"
#include "Types/SlateEnums.h"
#include "HangulEditableTextBox.generated.h"

class SHangulEditableTextBox;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FHangulEditBoxTextChanged, const FText&, Text);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FHangulEditBoxTextCommitted, const FText&, Text, ETextCommit::Type, CommitMethod);

/**
 * UMG (game UI / Blueprint) single-line text box with built-in Hangul composition.
 * Wraps SHangulEditableTextBox. Korean only.
 */
UCLASS(meta = (DisplayName = "Hangul Editable Text Box"))
class HANGULINPUTSLATEUMG_API UHangulEditableTextBox : public UWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Content")
	FText Text;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Content")
	FText HintText;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FHangulEditBoxTextChanged OnTextChanged;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FHangulEditBoxTextCommitted OnTextCommitted;

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

	TSharedPtr<SHangulEditableTextBox> MyWidget;
};
