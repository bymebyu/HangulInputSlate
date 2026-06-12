#include "HangulEditableText.h"
#include "SHangulEditableText.h"
#include "Framework/SlateDelegates.h"

#define LOCTEXT_NAMESPACE "HangulInputSlateUMG"

TSharedRef<SWidget> UHangulEditableText::RebuildWidget()
{
	MyWidget = SNew(SHangulEditableText)
		.HintText(HintText)
		.OnTextChanged(FOnTextChanged::CreateUObject(this, &UHangulEditableText::HandleTextChanged))
		.OnTextCommitted(FOnTextCommitted::CreateUObject(this, &UHangulEditableText::HandleTextCommitted));
	return MyWidget.ToSharedRef();
}

void UHangulEditableText::SynchronizeProperties()
{
	Super::SynchronizeProperties();
	if (MyWidget.IsValid())
	{
		MyWidget->SetText(Text);
	}
}

void UHangulEditableText::ReleaseSlateResources(bool bReleaseChildren)
{
	Super::ReleaseSlateResources(bReleaseChildren);
	MyWidget.Reset();
}

void UHangulEditableText::SetText(FText InText)
{
	Text = InText;
	if (MyWidget.IsValid())
	{
		MyWidget->SetText(InText);
	}
}

FText UHangulEditableText::GetText() const
{
	return MyWidget.IsValid() ? MyWidget->GetText() : Text;
}

void UHangulEditableText::SetComposeMode(bool bInCompose)
{
	if (MyWidget.IsValid())
	{
		MyWidget->SetComposeMode(bInCompose);
	}
}

void UHangulEditableText::ToggleComposeMode()
{
	if (MyWidget.IsValid())
	{
		MyWidget->ToggleComposeMode();
	}
}

bool UHangulEditableText::IsComposeMode() const
{
	return MyWidget.IsValid() && MyWidget->IsComposeMode();
}

void UHangulEditableText::HandleTextChanged(const FText& InText)
{
	Text = InText;
	OnTextChanged.Broadcast(InText);
}

void UHangulEditableText::HandleTextCommitted(const FText& InText, ETextCommit::Type CommitType)
{
	OnTextCommitted.Broadcast(InText, CommitType);
}

#if WITH_EDITOR
const FText UHangulEditableText::GetPaletteCategory()
{
	return LOCTEXT("PaletteCategory", "Input");
}
#endif

#undef LOCTEXT_NAMESPACE
