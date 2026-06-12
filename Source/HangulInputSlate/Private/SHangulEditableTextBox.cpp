#include "SHangulEditableTextBox.h"
#include "SHangulEditableText.h"
#include "Widgets/Layout/SBorder.h"
#include "Styling/StyleDefaults.h"

void SHangulEditableTextBox::Construct(const FArguments& InArgs)
{
	const FEditableTextBoxStyle* Style = InArgs._Style;
	const FSlateBrush* BackgroundBrush = Style ? &Style->BackgroundImageNormal : FStyleDefaults::GetNoBrush();
	const FMargin Padding = Style ? Style->Padding : FMargin(4.0f, 2.0f);

	ChildSlot
	[
		SNew(SBorder)
		.BorderImage(BackgroundBrush)
		.Padding(Padding)
		.VAlign(VAlign_Center)
		[
			SAssignNew(EditableText, SHangulEditableText)
			.HintText(InArgs._HintText)
			.IsReadOnly(InArgs._IsReadOnly)
			.OnTextChanged(InArgs._OnTextChanged)
			.OnTextCommitted(InArgs._OnTextCommitted)
		]
	];

	EditableText->SetInputLanguage(InArgs._Language);
	EditableText->SetComposeMode(InArgs._StartInComposeMode);
	EditableText->OnComposeModeChanged = InArgs._OnComposeModeChanged;
}

void SHangulEditableTextBox::SetText(const FText& InText)
{
	if (EditableText.IsValid())
	{
		EditableText->FlushComposition();
		EditableText->SetText(InText);
	}
}

FText SHangulEditableTextBox::GetText() const
{
	return EditableText.IsValid() ? EditableText->GetText() : FText::GetEmpty();
}

bool SHangulEditableTextBox::IsComposeMode() const
{
	return EditableText.IsValid() && EditableText->IsComposeMode();
}

void SHangulEditableTextBox::SetComposeMode(bool bInCompose)
{
	if (EditableText.IsValid())
	{
		EditableText->SetComposeMode(bInCompose);
	}
}

void SHangulEditableTextBox::ToggleComposeMode()
{
	if (EditableText.IsValid())
	{
		EditableText->ToggleComposeMode();
	}
}

void SHangulEditableTextBox::FlushComposition()
{
	if (EditableText.IsValid())
	{
		EditableText->FlushComposition();
	}
}
