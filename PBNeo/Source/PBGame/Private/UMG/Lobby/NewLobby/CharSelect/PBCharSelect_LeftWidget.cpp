// Fill out your copyright notice in the Description page of Project Settings.

#include "PBGame.h"
#include "PBCharSelect_LeftWidget.h"





PBGetterWidgetBlueprintVariable_Implementation(UButton, UPBCharSelect_LeftWidget, BackBtn);


void UPBCharSelect_LeftWidget::NativeConstruct()
{
	Super::NativeConstruct();

	BindEvents();
}

void UPBCharSelect_LeftWidget::NativeDestruct()
{
	Super::NativeDestruct();


}

void UPBCharSelect_LeftWidget::BindEvents()
{
	GetBPV_BackBtn()->OnReleased.AddUniqueDynamic(this, &UPBCharSelect_LeftWidget::HandleBackButtonReleased);

}

void UPBCharSelect_LeftWidget::HandleBackButtonReleased()
{
	PopPage();
}
