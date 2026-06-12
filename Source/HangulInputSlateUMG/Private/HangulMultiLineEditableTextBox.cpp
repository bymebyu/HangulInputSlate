#include "HangulMultiLineEditableTextBox.h"
#include "SHangulMultiLineEditableTextBox.h"
#include "Framework/SlateDelegates.h"

#define LOCTEXT_NAMESPACE "HangulInputSlateUMG"

TSharedRef<SWidget> UHangulMultiLineEditableTextBox::RebuildWidget()
{
	MyWidget = SNew(SHangulMultiLineEditableTextBox)
		.HintText(HintText)
		.OnTextChanged(FOnTextChanged::CreateUObject(this, &UHangulMultiLineEditableTextBox::HandleTextChanged))
		.OnTextCommitted(FOnTextCommitted::CreateUObject(this, &UHangulMultiLineEditableTextBox::HandleTextCommitted));
	return MyWidget.ToSharedRef();
}

void UHangulMultiLineEditableTextBox::SynchronizeProperties()
{
	Super::SynchronizeProperties();
	if (MyWidget.IsValid())
	{
		MyWidget->SetText(Text);
	}
}

void UHangulMultiLineEditableTextBox::ReleaseSlateResources(bool bReleaseChildren)
{
	Super::ReleaseSlateResources(bReleaseChildren);
	MyWidget.Reset();
}

void UHangulMultiLineEditableTextBox::SetText(FText InText)
{
	Text = InText;
	if (MyWidget.IsValid())
	{
		MyWidget->SetText(InText);
	}
}

FText UHangulMultiLineEditableTextBox::GetText() const
{
	return MyWidget.IsValid() ? MyWidget->GetText() : Text;
}

void UHangulMultiLineEditableTextBox::SetComposeMode(bool bInCompose)
{
	if (MyWidget.IsValid())
	{
		MyWidget->SetComposeMode(bInCompose);
	}
}

void UHangulMultiLineEditableTextBox::ToggleComposeMode()
{
	if (MyWidget.IsValid())
	{
		MyWidget->ToggleComposeMode();
	}
}

bool UHangulMultiLineEditableTextBox::IsComposeMode() const
{
	return MyWidget.IsValid() && MyWidget->IsComposeMode();
}

void UHangulMultiLineEditableTextBox::HandleTextChanged(const FText& InText)
{
	Text = InText;
	OnTextChanged.Broadcast(InText);
}

void UHangulMultiLineEditableTextBox::HandleTextCommitted(const FText& InText, ETextCommit::Type CommitType)
{
	OnTextCommitted.Broadcast(InText, CommitType);
}

#if WITH_EDITOR
const FText UHangulMultiLineEditableTextBox::GetPaletteCategory()
{
	return LOCTEXT("PaletteCategory", "Input");
}
#endif

#undef LOCTEXT_NAMESPACE
