#include "SHangulMultiLineEditableTextBox.h"
#include "SHangulMultiLineEditableText.h"
#include "Widgets/Layout/SBorder.h"
#include "Styling/StyleDefaults.h"

void SHangulMultiLineEditableTextBox::Construct(const FArguments& InArgs)
{
	const FEditableTextBoxStyle* Style = InArgs._Style;
	const FSlateBrush* BackgroundBrush = Style ? &Style->BackgroundImageNormal : FStyleDefaults::GetNoBrush();
	const FMargin Padding = Style ? Style->Padding : FMargin(4.0f);

	ChildSlot
	[
		SNew(SBorder)
		.BorderImage(BackgroundBrush)
		.Padding(Padding)
		[
			SAssignNew(EditableText, SHangulMultiLineEditableText)
			.HintText(InArgs._HintText)
			.AutoWrapText(InArgs._AutoWrapText)
			.IsReadOnly(InArgs._IsReadOnly)
			.AllowMultiLine(true)
			.ModiferKeyForNewLine(InArgs._ModiferKeyForNewLine)
			.OnTextChanged(InArgs._OnTextChanged)
			.OnTextCommitted(InArgs._OnTextCommitted)
		]
	];

	EditableText->SetInputLanguage(InArgs._Language);
	EditableText->SetComposeMode(InArgs._StartInComposeMode);
	EditableText->OnComposeModeChanged = InArgs._OnComposeModeChanged;
}

void SHangulMultiLineEditableTextBox::SetText(const FText& InText)
{
	if (EditableText.IsValid())
	{
		EditableText->FlushComposition();
		EditableText->SetText(InText);
	}
}

FText SHangulMultiLineEditableTextBox::GetText() const
{
	return EditableText.IsValid() ? EditableText->GetText() : FText::GetEmpty();
}

void SHangulMultiLineEditableTextBox::InsertTextAtCursor(const FString& InText)
{
	if (EditableText.IsValid())
	{
		EditableText->FlushComposition();
		EditableText->InsertTextAtCursor(InText);
	}
}

bool SHangulMultiLineEditableTextBox::IsComposeMode() const
{
	return EditableText.IsValid() && EditableText->IsComposeMode();
}

void SHangulMultiLineEditableTextBox::SetComposeMode(bool bInCompose)
{
	if (EditableText.IsValid())
	{
		EditableText->SetComposeMode(bInCompose);
	}
}

void SHangulMultiLineEditableTextBox::ToggleComposeMode()
{
	if (EditableText.IsValid())
	{
		EditableText->ToggleComposeMode();
	}
}

void SHangulMultiLineEditableTextBox::FlushComposition()
{
	if (EditableText.IsValid())
	{
		EditableText->FlushComposition();
	}
}
