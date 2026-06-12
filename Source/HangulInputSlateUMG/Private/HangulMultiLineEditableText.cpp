#include "HangulMultiLineEditableText.h"
#include "SHangulMultiLineEditableText.h"
#include "Framework/SlateDelegates.h"

#define LOCTEXT_NAMESPACE "HangulInputSlateUMG"

TSharedRef<SWidget> UHangulMultiLineEditableText::RebuildWidget()
{
	MyWidget = SNew(SHangulMultiLineEditableText)
		.HintText(HintText)
		.OnTextChanged(FOnTextChanged::CreateUObject(this, &UHangulMultiLineEditableText::HandleTextChanged))
		.OnTextCommitted(FOnTextCommitted::CreateUObject(this, &UHangulMultiLineEditableText::HandleTextCommitted));
	return MyWidget.ToSharedRef();
}

void UHangulMultiLineEditableText::SynchronizeProperties()
{
	Super::SynchronizeProperties();
	if (MyWidget.IsValid())
	{
		MyWidget->SetText(Text);
	}
}

void UHangulMultiLineEditableText::ReleaseSlateResources(bool bReleaseChildren)
{
	Super::ReleaseSlateResources(bReleaseChildren);
	MyWidget.Reset();
}

void UHangulMultiLineEditableText::SetText(FText InText)
{
	Text = InText;
	if (MyWidget.IsValid())
	{
		MyWidget->SetText(InText);
	}
}

FText UHangulMultiLineEditableText::GetText() const
{
	return MyWidget.IsValid() ? MyWidget->GetText() : Text;
}

void UHangulMultiLineEditableText::SetComposeMode(bool bInCompose)
{
	if (MyWidget.IsValid())
	{
		MyWidget->SetComposeMode(bInCompose);
	}
}

void UHangulMultiLineEditableText::ToggleComposeMode()
{
	if (MyWidget.IsValid())
	{
		MyWidget->ToggleComposeMode();
	}
}

bool UHangulMultiLineEditableText::IsComposeMode() const
{
	return MyWidget.IsValid() && MyWidget->IsComposeMode();
}

void UHangulMultiLineEditableText::HandleTextChanged(const FText& InText)
{
	Text = InText;
	OnTextChanged.Broadcast(InText);
}

void UHangulMultiLineEditableText::HandleTextCommitted(const FText& InText, ETextCommit::Type CommitType)
{
	OnTextCommitted.Broadcast(InText, CommitType);
}

#if WITH_EDITOR
const FText UHangulMultiLineEditableText::GetPaletteCategory()
{
	return LOCTEXT("PaletteCategory", "Input");
}
#endif

#undef LOCTEXT_NAMESPACE
