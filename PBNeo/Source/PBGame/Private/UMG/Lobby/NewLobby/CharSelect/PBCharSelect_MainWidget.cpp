// Fill out your copyright notice in the Description page of Project Settings.

#include "PBGame.h"
#include "PBCharSelect_MainWidget.h"



PBGetterWidgetBlueprintVariable_Implementation(UButton, UPBCharSelect_MainWidget, RedBtn);
PBGetterWidgetBlueprintVariable_Implementation(UButton, UPBCharSelect_MainWidget, BlueBtn);
PBGetterWidgetBlueprintVariable_Implementation(UButton, UPBCharSelect_MainWidget, BackBtn);

void UPBCharSelect_MainWidget::NativeConstruct()
{
	Super::NativeConstruct();

	BindEvents();
}

void UPBCharSelect_MainWidget::NativeDestruct()
{
	Super::NativeDestruct();
}

void UPBCharSelect_MainWidget::BindEvents()
{
	GetBPV_RedBtn()->OnReleased.AddUniqueDynamic(this, &UPBCharSelect_MainWidget::HandleRedButtonReleased);
	GetBPV_BlueBtn()->OnReleased.AddUniqueDynamic(this, &UPBCharSelect_MainWidget::HandleBlueButtonReleased);
	GetBPV_BackBtn()->OnReleased.AddUniqueDynamic(this, &UPBCharSelect_MainWidget::HandleBackButtonReleased);
}

void UPBCharSelect_MainWidget::HandleRedButtonReleased()
{
	print("Hello Red!");
	PushPage(EPageType::CharSelect_Left);
}

void UPBCharSelect_MainWidget::HandleBlueButtonReleased()
{
	print("Hello BLue!");
	PushPage(EPageType::CharSelect_Right);

}

void UPBCharSelect_MainWidget::HandleBackButtonReleased()
{
	PopPage();

}
