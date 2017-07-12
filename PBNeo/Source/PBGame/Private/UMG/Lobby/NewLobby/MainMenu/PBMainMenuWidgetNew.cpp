// Fill out your copyright notice in the Description page of Project Settings.

#include "PBGame.h"
#include "PBMainMenuWidgetNew.h"
#include "PBMainMenu_Button.h"
#include "PBUMGUtils.h"
#include "CanvasPanel.h"
#include "PBLobby.h"
#include "NamedSlot.h"
#include "PBMainMenuPage.h"

PBGetterWidgetBlueprintVariable_Implementation(UCanvasPanel, UPBMainMenuWidgetNew, MainButtonCanvas);
PBGetterWidgetBlueprintVariable_Implementation(UCanvasPanel, UPBMainMenuWidgetNew, SubButtonCanvas);
PBGetterWidgetBlueprintVariable_Implementation(UCanvasPanel, UPBMainMenuWidgetNew, SubButtonSlots);

UPBMainMenuWidgetNew::UPBMainMenuWidgetNew()
{
	CurrDepth = -1;
}

void UPBMainMenuWidgetNew::NativeConstruct()
{
	Super::NativeConstruct();

	BindEvents();

	SetOwnerOfButtonsOfCanvas(GetBPV_MainButtonCanvas());
	SetOwnerOfButtonsOfCanvas(GetBPV_SubButtonCanvas());

	SetCurrDepth(0);
	
}

void UPBMainMenuWidgetNew::NativeDestruct()
{
	Super::NativeDestruct();
}

FReply UPBMainMenuWidgetNew::NativeOnFocusReceived(const FGeometry& InGeometry, const FFocusEvent& InFocusEvent)
{
	FReply SuperReply = Super::NativeOnFocusReceived(InGeometry, InFocusEvent);
	FEventReply E(true);

	if (GetLastFocusedMainButton())
	{
		return TransferUserFocusThroughReply(InFocusEvent, E, GetLastFocusedMainButton()).NativeReply;
	}
	else
	{
		return TransferUserFocusThroughReply(InFocusEvent, E, GetFirstButtonOfMenu()).NativeReply;
	}

	return SuperReply;
}

void UPBMainMenuWidgetNew::BindEvents()
{
	BindEventsOfButtonsOfCanvas(GetBPV_MainButtonCanvas());
	BindEventsOfButtonsOfCanvas(GetBPV_SubButtonCanvas());


}

void UPBMainMenuWidgetNew::SetOwnerOfButtonsOfCanvas(class UCanvasPanel* CanvasPanel)
{
	if (nullptr == CanvasPanel)
	{
		return;
	}

	auto Children = UPBUMGUtils::GetChildrenOfPanel(CanvasPanel);

	for (auto Child : Children)
	{
		auto Button = Cast<UPBMainMenu_Button>(Child);
		if (Button)
		{
			Button->SetOwningPBUserWidget(this);
		}
	}
}

void UPBMainMenuWidgetNew::BindEventsOfButtonsOfCanvas(class UCanvasPanel* CanvasPanel)
{

	if (nullptr == CanvasPanel)
	{
		return;
	}

	auto Children = UPBUMGUtils::GetChildrenOfPanel(CanvasPanel);

	for (auto Child : Children)
	{
		auto Button = Cast<UPBMainMenu_Button>(Child);
		if (Button)
		{

			Button->OnPBButtonReleased.AddUniqueDynamic(this, &UPBMainMenuWidgetNew::HandlePBButtonReleased);
			Button->OnPBButtonGotNaviInput.AddUniqueDynamic(this, &UPBMainMenuWidgetNew::HandlePBButtonGotNaviInput);
			Button->OnPBButtonFocused.AddUniqueDynamic(this, &UPBMainMenuWidgetNew::HandlePBButtonFocused);
			Button->OnPBButtonCanceled.AddUniqueDynamic(this, &UPBMainMenuWidgetNew::HandlePBButtonCanceled);
		}
	}

}


void UPBMainMenuWidgetNew::BroadcastDepthChanged(int32 NewDepth)
{
	auto Children = UPBUMGUtils::GetChildrenOfPanel(GetBPV_MainButtonCanvas());
	for (auto Child : Children)
	{
		auto Button = Cast<UPBMainMenu_Button>(Child);
		if (Button)
		{
			Button->NotifyDepthChanged(NewDepth);
		}
	}

	Children = UPBUMGUtils::GetChildrenOfPanel(GetBPV_SubButtonCanvas());
	for (auto Child : Children)
	{
		auto Button = Cast<UPBMainMenu_Button>(Child);
		if (Button)
		{
			Button->NotifyDepthChanged(NewDepth);
		}
	}
}

void UPBMainMenuWidgetNew::SetCurrDepth(int32 NewDepth)
{
	if (CurrDepth != NewDepth)
	{
		CurrDepth = NewDepth;
		BroadcastDepthChanged(NewDepth);
	}

}

UPBUserWidget* UPBMainMenuWidgetNew::GetFirstButtonOfMenu()
{
	if (GetBPV_MainButtonCanvas()->GetChildrenCount() > 0)
	{
		return Cast<UPBUserWidget>(GetBPV_MainButtonCanvas()->GetChildAt(0));
	}

	return nullptr;
}


void UPBMainMenuWidgetNew::HandlePBButtonReleased(UPBUserWidget* Widget)
{

	// 0. Is there any sub buttons?
	if (true == HasSubButtons(Widget) && true == IsTheMainButton(Widget))
	{

		// 0. show
		ShowSubButtons();

		// 1. Build
		BuildSubButtons(Widget);

		// 2. move
		ChangeLocationOfSubButtons(Widget);


		// 3. Move the focus to the sub button
		NavigateUserFocus(GetOwningLocalPlayer(), GetBPV_SubButtonCanvas()->GetChildAt(0));
	}
	else
	{
		/**
		* Here's the two case.
		* Case 1) The main button is released but there's no sub widget.
		* Case 2) The sub button is released. It means that there's no sub widget.
		*/


		// 1. hide
		HideSubButtons();

		// 2. change the page.
		RequestToPushPage(Widget);

	}

	
}

void UPBMainMenuWidgetNew::HandlePBButtonGotNaviInput(UPBUserWidget* Widget, FKey PressedKey)
{						
	if (true == UPBUMGUtils::IsUp(PressedKey))
	{
		if (IsTheMainButton(Widget))
		{
			// the widget is one of the main buttons
			UPBUMGUtils::NavigateToNextInPanel(GetOwningLocalPlayer(), GetBPV_MainButtonCanvas(), true);
		}
		else if (IsTheSubButton(Widget))
		{
			// the widget is one of the sub buttons
			UPBUMGUtils::NavigateToNextInPanel(GetOwningLocalPlayer(), GetBPV_SubButtonCanvas(), true);

		}

	}
	else if (true == UPBUMGUtils::IsDown(PressedKey))
	{
		if (IsTheMainButton(Widget))
		{
			// the widget is one of the main buttons
			UPBUMGUtils::NavigateToNextInPanel(GetOwningLocalPlayer(), GetBPV_MainButtonCanvas(), false);
		}
		else if (IsTheSubButton(Widget))
		{
			// the widget is one of the sub buttons
			UPBUMGUtils::NavigateToNextInPanel(GetOwningLocalPlayer(), GetBPV_SubButtonCanvas(), false);

		}
	}

	else if (true == UPBUMGUtils::IsRight(PressedKey))
	{

	}
	else if (true == UPBUMGUtils::IsLeft(PressedKey))
	{

	}

}

void UPBMainMenuWidgetNew::HandlePBButtonFocused(UPBUserWidget* Widget, bool bFocused)
{
	if (bFocused && Widget)
	{
		// memorize the last focused widget when the focused widget is main button.
		if (IsTheMainButton(Widget))
		{
			LastFocusedMainButton = Widget;
			HideSubButtons();

			SetCurrDepth(0);

		}
		else if (IsTheSubButton(Widget))
		{
			SetCurrDepth(1);
		}
	}
}

void UPBMainMenuWidgetNew::HandlePBButtonCanceled(UPBUserWidget* Widget)
{
	if (IsTheSubButton(Widget))
	{
		// Depth1 -> Depth0 (SubButtons -> MainButtons)
		NavigateUserFocus(GetOwningLocalPlayer(), LastFocusedMainButton);
		HideSubButtons();
	}
}

void UPBMainMenuWidgetNew::ChangeLocationOfSubButtons(class UPBUserWidget* FocusedButton)
{
	// Get Index of this widget in the panel.
	int32 Index = GetBPV_MainButtonCanvas()->GetChildIndex(FocusedButton);
	if (Index == -1)
	{
		return;
	}

	auto SlotsForCanvas = GetBPV_SubButtonSlots();
	if (SlotsForCanvas->GetChildrenCount() <= Index)
	{
		return;
	}

	auto TargetSlot = Cast<UNamedSlot>(SlotsForCanvas->GetChildAt(Index));
	if (TargetSlot)
	{
		TargetSlot->AddChild(GetBPV_SubButtonCanvas());
	}
}

bool UPBMainMenuWidgetNew::HasSubButtons(class UPBUserWidget* Widget)
{
	int32 Index = GetBPV_MainButtonCanvas()->GetChildIndex(Widget);
	if (true == SubButtonDataSetArray.IsValidIndex(Index))
	{
		auto DataSet = SubButtonDataSetArray[Index];
		if (DataSet.DataArray.Num() > 0)
		{
			return true;
		}
	}

	return false;
}

void UPBMainMenuWidgetNew::BuildSubButtons(class UPBUserWidget* Widget)
{
	int32 Index = GetBPV_MainButtonCanvas()->GetChildIndex(Widget);
	if (true == SubButtonDataSetArray.IsValidIndex(Index))
	{

		// Get the data to set the sub buttons.
		auto DataArray = SubButtonDataSetArray[Index].DataArray;

		auto SubButtons = UPBUMGUtils::GetChildrenOfPanel(GetBPV_SubButtonCanvas());

		// I assume that the SubButtons, which are made in the editor, are always bigger than data numbers.
		if (DataArray.Num() > SubButtons.Num())
		{
			UE_LOG(LogUI, Warning, TEXT("UPBMainMenuWidgetNew::BuildSubButtons: Overflow at the main menu subbuttons!"));
			return;
		}

		// Set the button data and visibility.
		for (int32 i = 0; i < SubButtons.Num(); ++i)
		{
			auto Btn = Cast<UPBMainMenu_Button>(SubButtons[i]);
			if (Btn)
			{
				if (true == DataArray.IsValidIndex(i) && DataArray[i].TargetPage != EPageType::None)
				{
					Btn->TitleText		= DataArray[i].ButtonText;
					Btn->TargetPage		= DataArray[i].TargetPage;
					Btn->TargetTabIndex = DataArray[i].TargetTabIndex;
					Btn->Show();
					Btn->UpdateButtonLook();

				}
				else
				{
					Btn->TitleText		= FText::FromString("None");
					Btn->TargetPage		= EPageType::None;
					Btn->TargetTabIndex = 0;
					Btn->Hide();
				}

			}
			
		}

	}
}

void UPBMainMenuWidgetNew::RequestToPushPage(class UPBUserWidget* Widget)
{
	auto Button = Cast<UPBMainMenu_Button>(Widget);
	if (Button)
	{
		if (EPageType::None != Button->TargetPage)
		{
			auto Page = GetOwningPage<UPBMainMenuPage>();
			Page->RequestToPushThePageWithTabIndex(Button->TargetPage, Button->TargetTabIndex);
		}
	}
}

bool UPBMainMenuWidgetNew::IsTheMainButton(class UPBUserWidget* Widget)
{
	if (UPBUMGUtils::IsTheWidgetInThePanel(Widget, GetBPV_MainButtonCanvas()))
	{
		auto Button = Cast<UPBMainMenu_Button>(Widget);
		if (Button)
		{
			return true;
		}
	}

	return false;
}

bool UPBMainMenuWidgetNew::IsTheSubButton(class UPBUserWidget* Widget)
{
	if (UPBUMGUtils::IsTheWidgetInThePanel(Widget, GetBPV_SubButtonCanvas()))
	{
		auto Button = Cast<UPBMainMenu_Button>(Widget);
		if (Button)
		{
			return true;
		}
	}

	return false;

}

void UPBMainMenuWidgetNew::ShowSubButtons()
{
	GetBPV_SubButtonCanvas()->SetVisibility(ESlateVisibility::SelfHitTestInvisible);

}

void UPBMainMenuWidgetNew::HideSubButtons()
{
	GetBPV_SubButtonCanvas()->SetVisibility(ESlateVisibility::Hidden);
}
