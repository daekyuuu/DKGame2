// Fill out your copyright notice in the Description page of Project Settings.

#include "PBGame.h"
#include "PBCharSelect_RightWidget.h"


PBGetterWidgetBlueprintVariable_Implementation(UButton, UPBCharSelect_RightWidget, BackBtn);


void UPBCharSelect_RightWidget::NativeConstruct()
{
	Super::NativeConstruct();

	BindEvents();
}

void UPBCharSelect_RightWidget::NativeDestruct()
{
	Super::NativeDestruct();


}

void UPBCharSelect_RightWidget::BindEvents()
{
	GetBPV_BackBtn()->OnReleased.AddUniqueDynamic(this, &UPBCharSelect_RightWidget::HandleBackButtonReleased);

}

void UPBCharSelect_RightWidget::HandleBackButtonReleased()
{
	PopPage();
}
