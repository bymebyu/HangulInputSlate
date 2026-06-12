#include "HangulEditableTextBox.h"
#include "SHangulEditableTextBox.h"
#include "Framework/SlateDelegates.h"

#define LOCTEXT_NAMESPACE "HangulInputSlateUMG"

TSharedRef<SWidget> UHangulEditableTextBox::RebuildWidget()
{
	MyWidget = SNew(SHangulEditableTextBox)
		.HintText(HintText)
		.OnTextChanged(FOnTextChanged::CreateUObject(this, &UHangulEditableTextBox::HandleTextChanged))
		.OnTextCommitted(FOnTextCommitted::CreateUObject(this, &UHangulEditableTextBox::HandleTextCommitted));
	return MyWidget.ToSharedRef();
}

void UHangulEditableTextBox::SynchronizeProperties()
{
	Super::SynchronizeProperties();
	if (MyWidget.IsValid())
	{
		MyWidget->SetText(Text);
	}
}

void UHangulEditableTextBox::ReleaseSlateResources(bool bReleaseChildren)
{
	Super::ReleaseSlateResources(bReleaseChildren);
	MyWidget.Reset();
}

void UHangulEditableTextBox::SetText(FText InText)
{
	Text = InText;
	if (MyWidget.IsValid())
	{
		MyWidget->SetText(InText);
	}
}

FText UHangulEditableTextBox::GetText() const
{
	return MyWidget.IsValid() ? MyWidget->GetText() : Text;
}

void UHangulEditableTextBox::SetComposeMode(bool bInCompose)
{
	if (MyWidget.IsValid())
	{
		MyWidget->SetComposeMode(bInCompose);
	}
}

void UHangulEditableTextBox::ToggleComposeMode()
{
	if (MyWidget.IsValid())
	{
		MyWidget->ToggleComposeMode();
	}
}

bool UHangulEditableTextBox::IsComposeMode() const
{
	return MyWidget.IsValid() && MyWidget->IsComposeMode();
}

void UHangulEditableTextBox::HandleTextChanged(const FText& InText)
{
	Text = InText;
	OnTextChanged.Broadcast(InText);
}

void UHangulEditableTextBox::HandleTextCommitted(const FText& InText, ETextCommit::Type CommitType)
{
	OnTextCommitted.Broadcast(InText, CommitType);
}

#if WITH_EDITOR
const FText UHangulEditableTextBox::GetPaletteCategory()
{
	return LOCTEXT("PaletteCategory", "Input");
}
#endif

#undef LOCTEXT_NAMESPACE
