// Fill out your copyright notice in the Description page of Project Settings.

#include "PBGame.h"
#include "PBLobbyTabWidget.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "UMG/PBUMGUtils.h"
#include "UMG/Lobby/NewLobby/PBLobbyTabButton.h"

PBGetterWidgetBlueprintVariable_Implementation(UHorizontalBox, UPBLobbyTabWidget, TabButtonHorizontalBox);
PBGetterWidgetBlueprintVariable_Implementation(UCanvasPanel, UPBLobbyTabWidget, R1Canvas);


UPBLobbyTabWidget::UPBLobbyTabWidget()
{
	ActivatedButtonIndex = 0;
}

void UPBLobbyTabWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	BuildSubButtons();

	BindEvents();
}

void UPBLobbyTabWidget::NativeDestruct()
{
	Super::NativeDestruct();
}

void UPBLobbyTabWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	UpdateR1CanvasPosition();
}

FReply UPBLobbyTabWidget::NativeOnFocusReceived(const FGeometry& InGeometry, const FFocusEvent& InFocusEvent)
{
	FReply SuperReply = Super::NativeOnFocusReceived(InGeometry, InFocusEvent);

	// seungyoon.ji
	// TODO: 

	return SuperReply;
}

void UPBLobbyTabWidget::DesignTimeConstruct_Implementation()
{
	BuildSubButtons();

}

void UPBLobbyTabWidget::RotateToRight()
{
	Rotate(true);
}

void UPBLobbyTabWidget::RotateToLeft()
{
	Rotate(false);
}

void UPBLobbyTabWidget::JumpTo(int32 Index)
{
	if (GetBPV_TabButtonHorizontalBox()->GetChildrenCount() > Index
		&& Index >= 0)
	{
		JumpToInternal(Index);
	}

}

void UPBLobbyTabWidget::Rotate(bool bDirectionR)
{
	int32 Num = GetBPV_TabButtonHorizontalBox()->GetChildrenCount();

	// Update previous activated button
	ActivateTabButton(ActivatedButtonIndex, false);

	if (bDirectionR)
	{
		ActivatedButtonIndex = (ActivatedButtonIndex + 1) % Num;
	}
	else
	{
		--ActivatedButtonIndex;
		if (ActivatedButtonIndex < 0)
		{
			ActivatedButtonIndex = Num - 1;
		}
	}

	// Update newly activated button
	ActivateTabButton(ActivatedButtonIndex, true);
	
	OnPBLobbyTabChanged.Broadcast(ActivatedButtonIndex);
}

void UPBLobbyTabWidget::JumpToInternal(int32 Index)
{
	// Update previous activated button.
	ActivateTabButton(ActivatedButtonIndex, false);
	// Now the activated widget is changed.
	ActivatedButtonIndex = Index;
	// Update newly activated button.
	ActivateTabButton(ActivatedButtonIndex, true);

	OnPBLobbyTabChanged.Broadcast(Index);
}

void UPBLobbyTabWidget::BindEvents()
{
	auto Children = UPBUMGUtils::GetChildrenOfPanel(GetBPV_TabButtonHorizontalBox());
	for (auto Child : Children)
	{
		auto LobbyTabButton = Cast<UPBLobbyTabButton>(Child);
		if (LobbyTabButton)
		{
			LobbyTabButton->OnPBButtonFocused.AddUniqueDynamic(this, &UPBLobbyTabWidget::HandlePBButtonFocused);
			LobbyTabButton->OnPBButtonCanceled.AddUniqueDynamic(this, &UPBLobbyTabWidget::HandlePBButtonCanceled);
		}
	}
}

void UPBLobbyTabWidget::BuildSubButtons()
{
	GetBPV_TabButtonHorizontalBox()->ClearChildren();

	// Add Tab button to the horizontal box
	for (auto ButtonText : TabButtonTextArray)
	{
		auto TabButton = CreateWidget<UPBLobbyTabButton>(this->GetWorld(), TabButtonClass);
		if (TabButton)
		{
			TabButton->TitleText = ButtonText;
			GetBPV_TabButtonHorizontalBox()->AddChild(TabButton);
		}
	}

	// R1Canvas's position will be updated by HorizontalBox's Desire size.
	// And UE4 need one tick to update the desire size. Hence I update the position at next frame.
	if (GetWorld())
	{
		GetWorld()->GetTimerManager().SetTimerForNextTick(
			[=]()
		{
			UpdateR1CanvasPosition();
		}
		);
	}

}

void UPBLobbyTabWidget::UpdateR1CanvasPosition()
{
	auto CanvasSlotOfHorizontalBox = UWidgetLayoutLibrary::SlotAsCanvasSlot(GetBPV_TabButtonHorizontalBox());
	auto NewPositionOfR1Canvas = CanvasSlotOfHorizontalBox->GetPosition();
	NewPositionOfR1Canvas.X += GetBPV_TabButtonHorizontalBox()->GetDesiredSize().X;
	NewPositionOfR1Canvas.Y -= 16.0f; // Magic number for offset

	auto CanvasSlotOfR1Canvas = UWidgetLayoutLibrary::SlotAsCanvasSlot(GetBPV_R1Canvas());
	CanvasSlotOfR1Canvas->SetPosition(NewPositionOfR1Canvas);
}

void UPBLobbyTabWidget::ActivateTabButton(int32 ButtonIndex, bool bActive)
{

	auto Children = UPBUMGUtils::GetChildrenOfPanel(GetBPV_TabButtonHorizontalBox());
	if (false == Children.IsValidIndex(ButtonIndex))
	{
		return;
	}

	auto LobbyTabButton = Cast<UPBLobbyTabButton>(Children[ButtonIndex]);
	if (nullptr == LobbyTabButton)
	{
		return;
	}

	
	LobbyTabButton->Activate(bActive);



}

void UPBLobbyTabWidget::HandlePBButtonFocused(class UPBUserWidget* Widget, bool bFocused)
{

	/**
	* Actually, TabWidget don't need the focus handling because it will be handled by other widget and also It shouldn't be focused.
	* But for development environment, I allow the focus event as a tab rotation method.
	*/

	if (bFocused && Widget)
	{
		int32 Index = UPBUMGUtils::GetFocusWidgetIndexInThePanel(GetOwningLocalPlayer(), GetBPV_TabButtonHorizontalBox());
		if (Index != INDEX_NONE)
		{
			JumpToInternal(Index);
		}
	}
}

void UPBLobbyTabWidget::HandlePBButtonCanceled(class UPBUserWidget* Widget)
{
	OnPBButtonCanceled.Broadcast(Widget);
}
