// Fill out your copyright notice in the Description page of Project Settings.

#include "PBGame.h"
#include "PBOptionPage.h"
#include "PBPersistentUser.h"



UPBOptionPage::UPBOptionPage()
{

}

void UPBOptionPage::Init(class APlayerController* OwningPlayer, TSubclassOf<class UPBMainWidget> HUDWidgetClass)
{
	Super::Init(OwningPlayer, HUDWidgetClass);

	LoadPersistentValues();
	DoApplyChanges();
}

void UPBOptionPage::Enter()
{
	Super::Enter();


	OnEnter.Broadcast(GammaOpt, AimSensitivityOpt, bInvertYAxisOpt);
}

bool UPBOptionPage::SetAimSensitivity(int OptionIndex)
{
	AimSensitivityOpt = 0.1f + OptionIndex / 100.0f;
	return true;
}



bool UPBOptionPage::SetGamma(int OptionIndex)
{
	GammaOpt = 2.2f + 2.0f * ((OptionIndex - 10) / 20.0f);
	return true;
}


bool UPBOptionPage::SetInvertYAxis(bool Value)
{
	bInvertYAxisOpt = Value;
	return true;
}

void UPBOptionPage::LoadPersistentValues()
{
	if (GetPersistentUser())
	{
		AimSensitivityOpt = GetPersistentUser()->GetAimSensitivity();
		GammaOpt = GetPersistentUser()->GetGamma();
		bInvertYAxisOpt = GetPersistentUser()->GetInvertedYAxis();
	}
	else
	{
		AimSensitivityOpt = 1.0f;
		GammaOpt = 2.2f;
		bInvertYAxisOpt = false;
	}
}

void UPBOptionPage::DoApplyChanges()
{
	UPBPersistentUser* PersistentUser = GetPersistentUser();
	if (PersistentUser)
	{
		PersistentUser->SetGamma(GammaOpt);
		PersistentUser->SetAimSensitivity(AimSensitivityOpt);
		PersistentUser->SetInvertedYAxis(bInvertYAxisOpt);

		ensure(GEngine); GEngine->DisplayGamma = GammaOpt;
		PersistentUser->TellInputAboutKeybindings();

		PersistentUser->SaveIfDirty();
	}
}