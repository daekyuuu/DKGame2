// Fill out your copyright notice in the Description page of Project Settings.

#include "PBGame.h"
#include "PBInGameMenuWidget.h"
#include "PBGameInstance.h"
#include "Player/PBLocalPlayer.h"
#include "UMG/InGame/PBHUDWidget.h"
#include "PBNetClientInterface.h"

UPBInGameMenuWidget::UPBInGameMenuWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UPBInGameMenuWidget::NativeConstruct()
{
	if (GetPersistentUser())
	{
		AimSensitivityOpt = GetPersistentUser()->GetAimSensitivity();
		GammaOpt = GetPersistentUser()->GetGamma();
		bInvertYAxisOpt = GetPersistentUser()->GetInvertedYAxis();
	}
	else
	{
		AimSensitivityOpt = 0.75f;
		GammaOpt = 2.2f;
		bInvertYAxisOpt = false;
	}

	ApplyChanges();

	Super::NativeConstruct();
}

void UPBInGameMenuWidget::Show(int32 ZOrder)
{
	Super::Show(ZOrder);

	APBPlayerController* const PCOwner = Cast<APBPlayerController>(GetOwningPlayer());
	if (PCOwner)
	{
		PCOwner->bShowMouseCursor = true;
		FInputModeUIOnly InputMode;
		PCOwner->SetInputMode(InputMode);

		//PCOwner->OnHideResultScoreboard();
	}
}

void UPBInGameMenuWidget::Hide()
{
	APBPlayerController* const PCOwner = Cast<APBPlayerController>(GetOwningPlayer());
	if (PCOwner)
	{
		//UPBHUDWidget* const PBHUD = PCOwner->GetPBHUDWidget();
		//if ((PBHUD != NULL) && (PBHUD->IsMatchOver() == true) && (PCOwner->IsPrimaryPlayer() == true))
		//{
		//	PCOwner->OnShowResultScoreboard();
		//}

		FInputModeGameOnly InputMode;
		PCOwner->SetInputMode(InputMode);
		PCOwner->bShowMouseCursor = false;
	}

	Super::Hide();
}

void UPBInGameMenuWidget::ToggleGameMenu()
{
	// Check for a valid user index. In-game menu needs a valid userindex for many OSS calls.
	// Could be invalid if the user signed out, in which case the 'please connect your control' ui should be up anyway.
	if (GetOwnerUserIndex() == -1)
	{
		UE_LOG(LogPB, Log, TEXT("Trying to toggle in-game menu for invalid userid"));
		return;
	}

	if (IsOpened())
	{
		Hide();
	}
	else
	{
		Show();
		UPBUserWidget::NavigateUserFocus(GetOwningLocalPlayer(), this);
	}
}

void UPBInGameMenuWidget::OnGameResume()
{
	Hide();
}

void UPBInGameMenuWidget::OnGotoMainMenu()
{
	ULocalPlayer* PlayerOwner = GetOwningLocalPlayer();
	if (PlayerOwner)
	{
		UPBGameInstance* const GameInstance = Cast<UPBGameInstance>(PlayerOwner->GetGameInstance());
		if (GameInstance)
		{
			GameInstance->LabelPlayerAsQuitter(PlayerOwner);
			GameInstance->ReturnToMainMenu(true);		

			UPBNetClientInterface* ClientInterface = UPBNetClientInterface::Get(this);
			ClientInterface->LeaveCurrentRoom();

		}
	}
}

void UPBInGameMenuWidget::OnGameQuit()
{
	ULocalPlayer* PlayerOwner = GetOwningLocalPlayer();
	if (PlayerOwner == nullptr)
		return;

	UPBGameInstance* const GI = Cast<UPBGameInstance>(PlayerOwner->GetGameInstance());
	if (GI)
	{
		GI->LabelPlayerAsQuitter(PlayerOwner);

		UPBNetClientInterface* ClientInterface = UPBNetClientInterface::Get(this);
		ClientInterface->LeaveCurrentRoom();
	}

	Hide();

	QuitGame();
}

void UPBInGameMenuWidget::QuitGame()
{
	APlayerController* const PCOwner = GetOwningPlayer();
	if (PCOwner)
	{
		PCOwner->ConsoleCommand("quit");
	}
}

int32 UPBInGameMenuWidget::GetOwnerUserIndex() const
{
	return GetOwningLocalPlayer() ? GetOwningLocalPlayer()->GetControllerId() : -1;
}

UPBPersistentUser* UPBInGameMenuWidget::GetPersistentUser() const
{
	auto PBLocalPlayer = Cast<UPBLocalPlayer>(GetOwningLocalPlayer());
	if (PBLocalPlayer)
	{
		return PBLocalPlayer->GetPersistentUser();
	}

	return nullptr;
}

TArray<FText> UPBInGameMenuWidget::GetAimSensitivityList()
{
	TArray<FText> SensitivityList;
	for (int32 i = 0; i < 101; i++)
	{
		SensitivityList.Add(FText::AsNumber(i));
	}
	return SensitivityList;
}

bool UPBInGameMenuWidget::SetAimSensitivity(int OptionIndex)
{
	AimSensitivityOpt = 0.1f + OptionIndex / 100.0f;
	return true;
}

int UPBInGameMenuWidget::GetAimSensitivityIndex()
{
	int32 SensivitityIndex = FMath::RoundToInt((AimSensitivityOpt - 0.1f) * 100.0f);
	return FMath::Clamp(SensivitityIndex, 0, 100);
}

TArray<FText> UPBInGameMenuWidget::GetGammaList()
{
	TArray<FText> GammaList;
	for (int32 i = -10; i < 11; i++)
	{
		GammaList.Add(FText::AsNumber(i));
	}
	return GammaList;
}

bool UPBInGameMenuWidget::SetGamma(int OptionIndex)
{
	GammaOpt = 2.2f + 2.0f * ((OptionIndex - 10) / 20.0f);
	return true;
}

int UPBInGameMenuWidget::GetGammaIndex()
{
	int32 GammaIndex = FMath::TruncToInt(((GammaOpt - 2.2f) / 2.0f) * 20.f) + 10;
	return FMath::Clamp(GammaIndex, 0, 20);
}

bool UPBInGameMenuWidget::SetInvertYAxis(bool Value)
{
	bInvertYAxisOpt = Value;
	return true;
}

bool UPBInGameMenuWidget::GetInvertYAxis()
{
	return bInvertYAxisOpt;
}

bool UPBInGameMenuWidget::ApplyChanges()
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

	return true;
}