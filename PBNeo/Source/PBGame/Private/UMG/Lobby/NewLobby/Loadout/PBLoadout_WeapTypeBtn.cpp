// Fill out your copyright notice in the Description page of Project Settings.

#include "PBGame.h"
#include "PBLoadout_WeapTypeBtn.h"
#include "TextBlock.h"


PBGetterWidgetBlueprintVariable_Implementation(UTextBlock, UPBLoadout_WeapTypeBtn, Text0);
PBGetterWidgetBlueprintVariable_Implementation(UTextBlock, UPBLoadout_WeapTypeBtn, Text1);
PBGetterWidgetBlueprintVariable_Implementation(UTextBlock, UPBLoadout_WeapTypeBtn, Text2);
PBGetterWidgetBlueprintVariable_Implementation(UTextBlock, UPBLoadout_WeapTypeBtn, Text3);



UPBLoadout_WeapTypeBtn::UPBLoadout_WeapTypeBtn()
{
	MyDepth = 1;
}

void UPBLoadout_WeapTypeBtn::NativeConstruct()
{
	Super::NativeConstruct();

	GetBPV_Text0();
	GetBPV_Text1();
	GetBPV_Text2();
	GetBPV_Text3();

}

void UPBLoadout_WeapTypeBtn::SynchronizeProperties()
{

	SyncText();

	Super::SynchronizeProperties();

}

void UPBLoadout_WeapTypeBtn::SyncText()
{
	GetBPV_Text0()->SetText(ButtonText);
	GetBPV_Text1()->SetText(ButtonText);
	GetBPV_Text2()->SetText(ButtonText);
	GetBPV_Text3()->SetText(ButtonText);
}

void UPBLoadout_WeapTypeBtn::SetButtonText(FText Text)
{
	ButtonText = Text;
	SyncText();
}

void UPBLoadout_WeapTypeBtn::SetWeapTypeId(int32 NewId)
{
	WeapTypeId = NewId;
}

int32 UPBLoadout_WeapTypeBtn::GetWeapTypeId() const
{
	return WeapTypeId;
}

