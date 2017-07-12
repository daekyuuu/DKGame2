// Fill out your copyright notice in the Description page of Project Settings.

#include "PBGame.h"
#include "PB3DWidgetRelay.h"
#include "WidgetInteractionComponent.h"

AWidgetInteractor::AWidgetInteractor()
{
	InteractionComp = CreateDefaultSubobject<UWidgetInteractionComponent>(TEXT("InteractionComp"));
}

UPB3DWidgetRelay::UPB3DWidgetRelay(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bIsFocusable = true;
	Visibility = ESlateVisibility::Visible;	

	InitWidgetInteractorSys();
}

void UPB3DWidgetRelay::NativeConstruct()
{
	Super::NativeConstruct();
	SpawnWidgetInteractor();
}

void UPB3DWidgetRelay::NativeDestruct()
{
	DestroyWidgetInteractor();
	Super::NativeDestruct();
}

UWidgetInteractionComponent* UPB3DWidgetRelay::GetWidgetIntereacter()
{
	if (WidgetInteractor)
	{
		return WidgetInteractor->InteractionComp;
	}

	return nullptr;
}

void UPB3DWidgetRelay::InitWidgetInteractorSys()
{
	MaxInteractionDistance = 10000.f;
}

void UPB3DWidgetRelay::SpawnWidgetInteractor()
{
	if (GetWorld())
	{
		WidgetInteractor = GetWorld()->SpawnActorDeferred<AWidgetInteractor>(
			AWidgetInteractor::StaticClass(), 
			FTransform::Identity, GetOwningPlayer(), nullptr, 
			ESpawnActorCollisionHandlingMethod::AlwaysSpawn);

		if (!WidgetInteractor)
		{
			return;
		}

		UWidgetInteractionComponent* InteractionComp = WidgetInteractor->InteractionComp;
		if (InteractionComp)
		{
			InteractionComp->InteractionSource = EWidgetInteractionSource::Mouse;
			InteractionComp->VirtualUserIndex = GetOwningLocalPlayer() ? GetOwningLocalPlayer()->GetControllerId() : 0;
			InteractionComp->InteractionDistance = MaxInteractionDistance;
		}

		UPBGameplayStatics::FinishSpawningActor(WidgetInteractor, FTransform::Identity);
	}
}

void UPB3DWidgetRelay::DestroyWidgetInteractor()
{
	if (WidgetInteractor)
	{
		WidgetInteractor->Destroy();
	}
}

FReply UPB3DWidgetRelay::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	FReply SuperReply = Super::NativeOnKeyDown(InGeometry, InKeyEvent);

	if (GetWidgetIntereacter())
	{
		GetWidgetIntereacter()->PressKey(InKeyEvent.GetKey());
		return FReply::Handled();
	}

	return SuperReply;
}

FReply UPB3DWidgetRelay::NativeOnKeyUp(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	FReply SuperReply = Super::NativeOnKeyUp(InGeometry, InKeyEvent);

	if (GetWidgetIntereacter())
	{
		GetWidgetIntereacter()->ReleaseKey(InKeyEvent.GetKey());
		return FReply::Handled();
	}

	return SuperReply;
}

FReply UPB3DWidgetRelay::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	FReply SuperReply = Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);

	if (GetWidgetIntereacter())
	{
		GetWidgetIntereacter()->PressPointerKey(InMouseEvent.GetEffectingButton());
		return FReply::Handled();
	}

	return SuperReply;
}

FReply UPB3DWidgetRelay::NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	FReply SuperReply = Super::NativeOnMouseButtonUp(InGeometry, InMouseEvent);
	
	if (GetWidgetIntereacter())
	{
		GetWidgetIntereacter()->ReleasePointerKey(InMouseEvent.GetEffectingButton());
		return FReply::Handled();
	}

	return SuperReply;
}

void UPB3DWidgetRelay::NativeOnFocusLost(const FFocusEvent& InFocusEvent)
{
	Super::NativeOnFocusLost(InFocusEvent);

	FSlateApplication& SlateApp = FSlateApplication::Get();

	if (GetWidgetIntereacter())
	{
		TSharedPtr<FSlateVirtualUser> VirtualUser = SlateApp.FindOrCreateVirtualUser(GetWidgetIntereacter()->VirtualUserIndex);
		if (VirtualUser.IsValid())
		{
			SlateApp.ClearUserFocus(VirtualUser->GetUserIndex(), EFocusCause::SetDirectly);
		}
	}
}

bool UPB3DWidgetRelay::ContainVirtualUserFocus(UWidget* Widget)
{
	if (Widget)
	{
		return ContainVirtualUserFocus(Widget->TakeWidget().Get());
	}

	return false;
}

bool UPB3DWidgetRelay::ContainVirtualUserFocus(SWidget& Widget)
{
	FSlateApplication& SlateApp = FSlateApplication::Get();
	
	if (GetWidgetIntereacter())
	{
		TSharedPtr<FSlateVirtualUser> VirtualUser = SlateApp.FindOrCreateVirtualUser(GetWidgetIntereacter()->VirtualUserIndex);
		return UPBUserWidget::ContainUserFocus(VirtualUser->GetUserIndex(), Widget);
	}

	return false;
}