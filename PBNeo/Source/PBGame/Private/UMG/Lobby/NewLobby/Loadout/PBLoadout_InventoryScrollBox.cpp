// Fill out your copyright notice in the Description page of Project Settings.

#include "PBGame.h"
#include "PBLoadout_InventoryScrollBox.h"
#include "PBLoadout_ItemVector2.h"
#include "UMG/PBUMGUtils.h"

PBGetterWidgetBlueprintVariable_Implementation(UScrollBox, UPBLoadout_InventoryScrollBox, ScrollBox);

UPBLoadout_InventoryScrollBox::UPBLoadout_InventoryScrollBox()
{
	
}

void UPBLoadout_InventoryScrollBox::NativeConstruct()
{
	Super::NativeConstruct();

	BindEvents();
}

void UPBLoadout_InventoryScrollBox::NativeDestruct()
{
	Super::NativeDestruct();
}

FReply UPBLoadout_InventoryScrollBox::NativeOnFocusReceived(const FGeometry& InGeometry, const FFocusEvent& InFocusEvent)
{
	FReply SuperReplay = Super::NativeOnFocusReceived(InGeometry, InFocusEvent);
	FEventReply E(true);

	// Set the initial focus target if exist
	if (GetBPV_ScrollBox()->GetChildrenCount() > 0)
	{
		auto TargetWidget = GetBPV_ScrollBox()->GetChildAt(0);
		return TransferUserFocusThroughReply(InFocusEvent, E, TargetWidget).NativeReply;

	}

	return SuperReplay;
}

void UPBLoadout_InventoryScrollBox::NotifyItemListReceived(const TArray<int32>& ItemList)
{
	BuildScrollBoxContents(ItemList);

	NavigateUserFocusToTheFirstWidget();

	BindEvents();
}

void UPBLoadout_InventoryScrollBox::BindEvents()
{
	auto ArrayOfItemVector2 = UPBUMGUtils::GetChildrenOfPanelAs<UPBLoadout_ItemVector2>(GetBPV_ScrollBox());
	for (auto ItemVec2 : ArrayOfItemVector2)
	{
		BindPBButtonEvents<UPBLoadout_InventoryScrollBox>(this, ItemVec2);

	}
}

void UPBLoadout_InventoryScrollBox::BuildScrollBoxContents(const TArray<int32>& ItemList)
{
	if (nullptr == GetWorld())
	{
		return;
	}

	GetBPV_ScrollBox()->ClearChildren();

	int32 const NumOfItem = ItemList.Num();
	int32 const NumOfItemVector2 = (NumOfItem / 2) + 1;

	// 1. Create the item vectors
	for (int32 i = 0; i < NumOfItemVector2; ++i)
	{
		auto ItemVector2 = CreateWidget<UPBLoadout_ItemVector2>(GetWorld(), ItemVector2Class);
		if (ItemVector2)
		{
			GetBPV_ScrollBox()->AddChild(ItemVector2);
		}
	}


	// 2. Fill out the item vector contents.
	auto Items = ItemList;
	auto Children = UPBUMGUtils::GetChildrenOfPanel(GetBPV_ScrollBox());
	for (auto Child : Children)
	{
		auto ItemVector2 = Cast<UPBLoadout_ItemVector2>(Child);
		if (ItemVector2)
		{
			int32 ItemId0 = Items.Num() > 0 ? Items.Pop() : 0;
			int32 ItemId1 = Items.Num() > 0 ? Items.Pop() : 0;
			
			ItemVector2->SetItemsId(ItemId0, ItemId1);

		}
	}
}

void UPBLoadout_InventoryScrollBox::NavigateUserFocusToTheFirstWidget()
{
	if (GetBPV_ScrollBox()->GetChildrenCount() > 0)
	{
		auto TargetWidget = GetBPV_ScrollBox()->GetChildAt(0);
		NavigateUserFocus(GetOwningLocalPlayer(), TargetWidget);

		UPBUMGUtils::ScrollWidgetIntoViewWithOffset(GetBPV_ScrollBox(), TargetWidget);
	}
}

void UPBLoadout_InventoryScrollBox::HandlePBButtonReleased(class UPBUserWidget* Widget)
{

}

void UPBLoadout_InventoryScrollBox::HandlePBButtonGotNaviInput(class UPBUserWidget* Widget, FKey PressedKey)
{
	auto Vec2HasFocus = Cast<UPBLoadout_ItemVector2>(Widget);
	if (nullptr == Vec2HasFocus)
	{
		return;
	}

	if (UPBUMGUtils::IsUp(PressedKey) || UPBUMGUtils::IsDown(PressedKey))
	{
		bool bInverseDir = UPBUMGUtils::IsUp(PressedKey);

		auto Vec2TargetFocus= Cast<UPBLoadout_ItemVector2>(UPBUMGUtils::GetNextWidgetInPanel(GetBPV_ScrollBox(), Widget, bInverseDir));
		if(Vec2TargetFocus)
		{
			UPBUMGUtils::NavigateToOtherPannelAtSameIndex(GetOwningLocalPlayer(), Vec2HasFocus->GetBPV_ItemHorizontalBox(), Vec2TargetFocus->GetBPV_ItemHorizontalBox());
			UPBUMGUtils::ScrollWidgetIntoViewWithOffset(GetBPV_ScrollBox(), Vec2TargetFocus);
			
		}
		return;
	}

	OnPBButtonGotNaviInput.Broadcast(this, PressedKey);
	
}

void UPBLoadout_InventoryScrollBox::HandlePBButtonFocused(class UPBUserWidget* Widget, bool bFocused)
{

}

void UPBLoadout_InventoryScrollBox::HandlePBButtonCanceled(class UPBUserWidget* Widget)
{
	OnPBButtonCanceled.Broadcast(this);
}

