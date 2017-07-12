// Fill out your copyright notice in the Description page of Project Settings.

#include "PBGame.h"
#include "PBMainMenu_Button.h"
#include "PBLobby.h"
#include "PBMainMenuWidgetNew.h"


PBGetterWidgetBlueprintProperty_Implementation(UWidgetAnimation, UPBMainMenu_Button, SameDepth_Focused_Ani);
PBGetterWidgetBlueprintProperty_Implementation(UWidgetAnimation, UPBMainMenu_Button, SameDepth_Normal_Ani);
PBGetterWidgetBlueprintProperty_Implementation(UWidgetAnimation, UPBMainMenu_Button, OtherDepth_Focused_Ani);
PBGetterWidgetBlueprintProperty_Implementation(UWidgetAnimation, UPBMainMenu_Button, OtherDepth_Normal_Ani);

UPBMainMenu_Button::UPBMainMenu_Button()
{
	TargetPage = EPageType::None;
	TargetTabIndex = 0;
}

void UPBMainMenu_Button::NativeConstruct()
{
	Super::NativeConstruct();
}

void UPBMainMenu_Button::UpdateButtonLook()
{
	UWidgetAnimation* Anim = nullptr;

	if (bInSameDepth)
	{
		if (true == ContainUserFocus(GetOwningLocalPlayer(), this))
		{
			Anim = GetBPP_SameDepth_Focused_Ani();
		}
		else
		{
			Anim = GetBPP_SameDepth_Normal_Ani();
		}
	}
	else
	{
		if (AmILastFocusedButton())
		{
			Anim = GetBPP_OtherDepth_Focused_Ani();
		}
		else
		{
			Anim = GetBPP_OtherDepth_Normal_Ani();
		}

	}


	if (Anim)
	{
		AnimToPlay = Anim;
	}
}

bool UPBMainMenu_Button::AmILastFocusedButton()
{
	auto MainMenuWidget = Cast<UPBMainMenuWidgetNew>(OwningPBUserWidget);
	if (MainMenuWidget)
	{
		auto LastFocusedBtn = MainMenuWidget->GetLastFocusedMainButton();
		if (LastFocusedBtn)
		{
			return (this == LastFocusedBtn);
		}
	}

	return false;
}

