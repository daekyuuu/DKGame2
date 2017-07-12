// Fill out your copyright notice in the Description page of Project Settings.

#include "PBGame.h"
#include "PBSpectatorModeWidget.h"
#include "Blueprint/WidgetTree.h"
#include "Components/ScrollBox.h"
#include "Components/WidgetSwitcher.h"
#include "Components/Image.h"
#include "Components/TextWidgetTypes.h"
#include "Components/TextBlock.h"
#include "Components/ProgressBar.h"
#include "Components/CanvasPanel.h"
#include "Components/ContentWidget.h"
#include "Components/Button.h"
#include "Components/Border.h"

//----------------------- UPBSpectatorModeWidget : UPBSubWidget -----------------------
// Player List
PBGetterWidgetBlueprintVariable_Implementation(UScrollBox, UPBSpectatorModeWidget, PlayerListBox);
PBGetterWidgetBlueprintVariable_Implementation(UButton, UPBSpectatorModeWidget, Button_L1);
PBGetterWidgetBlueprintVariable_Implementation(UButton, UPBSpectatorModeWidget, Button_R1);
PBGetterWidgetBlueprintVariable_Implementation(UBorder, UPBSpectatorModeWidget, SpectatorDescriptionBorder);
PBGetterWidgetBlueprintVariable_Implementation(UTextBlock, UPBSpectatorModeWidget, SpectatorDescription);
PBGetterWidgetBlueprintVariable_Implementation(UBorder, UPBSpectatorModeWidget, ReasonWhyViewEnemyBorder);
PBGetterWidgetBlueprintVariable_Implementation(UTextBlock, UPBSpectatorModeWidget, ReasonWhyViewEnemy);

// Observable Player Info
// Name
PBGetterWidgetBlueprintVariable_Implementation(UTextBlock, UPBSpectatorModeWidget, Level_Text);
PBGetterWidgetBlueprintVariable_Implementation(UTextBlock, UPBSpectatorModeWidget, UserName_Text);
PBGetterWidgetBlueprintVariable_Implementation(UImage, UPBSpectatorModeWidget, Trophy_Image);
// Score
PBGetterWidgetBlueprintVariable_Implementation(UTextBlock, UPBSpectatorModeWidget, Kill_Value);
PBGetterWidgetBlueprintVariable_Implementation(UTextBlock, UPBSpectatorModeWidget, Assist_Value);
PBGetterWidgetBlueprintVariable_Implementation(UTextBlock, UPBSpectatorModeWidget, Death_Value);
// Weapon
PBGetterWidgetBlueprintVariable_Implementation(UImage, UPBSpectatorModeWidget, Weapon_Image);
PBGetterWidgetBlueprintVariable_Implementation(UTextBlock, UPBSpectatorModeWidget, Weapon_Text);
// System
PBGetterWidgetBlueprintVariable_Implementation(UTextBlock, UPBSpectatorModeWidget, ChangeLoadoutText);

UPBSpectatorModeWidget::UPBSpectatorModeWidget()
{
	bPreviousStateHasAnyAliveOwnTeamPlayer = false;
	RefreshRatioInSeconds = 1.0f;
	SelectedCardIndex = INDEX_NONE;
}

void UPBSpectatorModeWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// For Caching
	GetBPV_PlayerListBox()->SetVisibility(ESlateVisibility::Hidden);
	GetBPV_PlayerListBox()->AlwaysShowScrollbar = false;
	GetBPV_Button_L1()->SetVisibility(ESlateVisibility::Hidden);
	GetBPV_Button_R1()->SetVisibility(ESlateVisibility::Hidden);
	GetBPV_SpectatorDescriptionBorder()->SetVisibility(ESlateVisibility::Hidden);
	GetBPV_ReasonWhyViewEnemyBorder()->SetVisibility(ESlateVisibility::Hidden);
	GetBPV_ReasonWhyViewEnemy()->SetText(FText::FromString("Your team has been died everybody."));
	GetBPV_ChangeLoadoutText()->SetText(FText::FromString("Change Loadout"));

	BindWidgetEvent(OnSpawn, UPBSpectatorModeWidget::NotifySpawn);
	BindWidgetEvent(OnDeath, UPBSpectatorModeWidget::NotifyDeath);
	//BindWidgetEvent(OnBecomeFollower, UPBSpectatorModeWidget::NotifyBecomeFollower);
	BindWidgetEvent(OnUpdateFollowCamera, UPBSpectatorModeWidget::NotifyEnterFollowCamera);
}

void UPBSpectatorModeWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
}

void UPBSpectatorModeWidget::NativeDestruct()
{
	ClearRefreshRatioInSeconds();

	UnbindWidgetEvent(OnSpawn);
	UnbindWidgetEvent(OnDeath);
	//UnbindWidgetEvent(OnBecomeFollower);
	UnbindWidgetEvent(OnUpdateFollowCamera)

	Super::NativeDestruct();
}

void UPBSpectatorModeWidget::NotifySpawn(APBPlayerState* PlayerState)
{
	auto OP = GetOwningPlayer();
	if (nullptr == OP || false == OP->IsLocalPlayerController())
		return;

	bPreviousStateHasAnyAliveOwnTeamPlayer = true;

	RemoveCards();

	SetVisibility(ESlateVisibility::Hidden);

	GetBPV_PlayerListBox()->SetVisibility(ESlateVisibility::Hidden);
	GetBPV_PlayerListBox()->ClearChildren();
	GetBPV_Button_L1()->SetVisibility(ESlateVisibility::Hidden);
	GetBPV_Button_R1()->SetVisibility(ESlateVisibility::Hidden);
	GetBPV_SpectatorDescriptionBorder()->SetVisibility(ESlateVisibility::Hidden);
	GetBPV_ReasonWhyViewEnemyBorder()->SetVisibility(ESlateVisibility::Hidden);
}

void UPBSpectatorModeWidget::NotifyDeath(class APBPlayerState* KillerPlayerState, class APBPlayerState* KilledPlayerState, const UDamageType* KillerDamageType, const FTakeDamageInfo& TakeDamageInfo)
{
	//bPreviousStateHasAnyAliveOwnTeamPlayer = true;
	
	//RemoveCards();

	//auto OP = GetOwningPlayer();
	//if (nullptr == OP || false == OP->IsLocalPlayerController())
	//	return;

	//GetBPV_PlayerListBox()->ClearChildren();
}

void UPBSpectatorModeWidget::NotifyEnterFollowCamera(APBPlayerState *TargetPlayer, bool bTryToGetNextPlayer, bool bPlaySound)
{
	UWorld *World = GetWorld();
	if (nullptr == World)
		return;

	auto OP = GetOwningPlayer();
	if (nullptr == OP || false == OP->IsLocalPlayerController())
		return;

	if (nullptr != TargetPlayer)
	{
		if (bTryToGetNextPlayer)
		{
			if (SoundPreviousButton && bPlaySound)
			{
				UPBGameplayStatics::PlaySound2D(World, SoundPreviousButton);
			}
		}
		else
		{
			if (SoundNextButton && bPlaySound)
			{
				UPBGameplayStatics::PlaySound2D(World, SoundNextButton);
			}
		}

		FocusedPlayerState = TargetPlayer;
	}

	SetRefreshRatioInSeconds(RefreshRatioInSeconds);
	UpdatePlayerStates();
	FocusCard(/*TargetPlayer*/);
	UpdateFocusedPlayerPanel(/*TargetPlayer*/);
}

void UPBSpectatorModeWidget::AddCards(EPBTeamType TeamType)
{
	auto OP = GetOwningPlayer();
	if (nullptr == OP || false == OP->IsLocalPlayerController())
		return;

	auto PS = OP->PlayerState;
	auto GS = Cast<APBGameState>(UGameplayStatics::GetGameState(GetWorld()));
	for (auto Itr : GS->PlayerArray)
	{
		auto CurPS = Cast<APBPlayerState>(Itr);
		if (CurPS->GetTeamType() != TeamType || CurPS == PS)
			continue;

		AddCard(CurPS, false);
	}
}

void UPBSpectatorModeWidget::AddCard(class APBPlayerState *PlayerState, bool bOnlyCreateWidget)
{
	auto OP = GetOwningPlayer();
	if (nullptr == OP || false == OP->IsLocalPlayerController())
		return;

	//for (auto CurCard : Cards)
	//{
	//	if (CurCard.PlayerState == PlayerState)
	//		return;
	//}

	auto Card = CreateWidget<UPBSpectatorInfoCard>(GetOwningPlayer(), SpectatorInfoCardClass);
	GetBPV_PlayerListBox()->AddChild(Card);
	Card->SetVisibility(ESlateVisibility::Visible);
	if (false == bOnlyCreateWidget)
	{
		Card->UpdateCard(PlayerState, false);
	}
	FPBSpectatorCard Data = { PlayerState, Card };
	Cards.Add(Data);
}

void UPBSpectatorModeWidget::RemoveCards()
{
	auto OP = GetOwningPlayer();
	if (nullptr == OP || false == OP->IsLocalPlayerController())
		return;

	for (auto CurCard : Cards)
	{
		//GetBPV_PlayerListBox()->RemoveChild(CurCard.Card);
		CurCard.Card->SetVisibility(ESlateVisibility::Hidden);
		CurCard.Card->RemoveFromParent();
		CurCard.Card->ConditionalBeginDestroy();
		CurCard.Card = nullptr;
		//CurCard.PlayerState = nullptr;
	}

	Cards.Reset();

	ClearRefreshRatioInSeconds();
}

void UPBSpectatorModeWidget::FocusCard(/*class APBPlayerState *PlayerState*/)
{
	auto OP = GetOwningPlayer();
	if (nullptr == OP || false == OP->IsLocalPlayerController())
		return;


	if (false == FocusedPlayerState.IsValid())// ensureMsgf(FocusedPlayerState.IsValid(), TEXT("UPBSpectatorModeWidget::FocusCard has no FocusedPlayerState")))
		return;

	auto InPlayerState = FocusedPlayerState.Get();

	//for (auto CurCard : Cards)
	for (int32 i = 0; i < Cards.Num(); ++i)
	{
		auto CurCard = Cards[i];
		APBPlayerState *CurPS = CurCard.PlayerState;
		if (CurPS->UniqueId == InPlayerState->UniqueId)
		{
			SelectedCardIndex = i;
			CurCard.Card->SetActiveSwitcherItemByStatus(EPBSpectatorInfoCardStatus::Selected);
			CurCard.Card->UpdateCard(CurCard.PlayerState, true);
		}
		else
		{
			auto OC = CurPS->GetOwnerCharacter();
			if (OC)
			{
				bool bIsAlive = OC->IsAlive();
				CurCard.Card->SetActiveSwitcherItemByStatus(bIsAlive ? EPBSpectatorInfoCardStatus::Normal : EPBSpectatorInfoCardStatus::Death);
				CurCard.Card->UpdateCard(CurCard.PlayerState, false);
			}
		}
	}
}

void UPBSpectatorModeWidget::UpdatePlayerStates()
{
	auto OP = GetOwningPlayer();
	if (nullptr == OP || false == OP->IsLocalPlayerController())
		return;

	auto OC = GetOwningCharacter();
	if (nullptr == OC)
		return;

	if (false == OC->IsSpectator() && OC->IsAlive())
	{
		if (ESlateVisibility::Visible == GetVisibility())
		{
			SetVisibility(ESlateVisibility::Hidden);

			GetBPV_PlayerListBox()->SetVisibility(ESlateVisibility::Hidden);
			GetBPV_Button_L1()->SetVisibility(ESlateVisibility::Hidden);
			GetBPV_Button_R1()->SetVisibility(ESlateVisibility::Hidden);
			GetBPV_SpectatorDescriptionBorder()->SetVisibility(ESlateVisibility::Hidden);
			GetBPV_ReasonWhyViewEnemyBorder()->SetVisibility(ESlateVisibility::Hidden);
		}

		return; 
	}

	auto GS = Cast<APBGameState>(UGameplayStatics::GetGameState(GetWorld()));
	if (nullptr == GS)
		return;

	auto PS = Cast<APBPlayerState>(OP->PlayerState);
	if (nullptr == PS)
		return;

	EPBTeamType TeamType = PS->GetTeamType();
	EPBTeamType EnemyTeamType = (EPBTeamType)(1 - ((int32)(TeamType) % 2));
	TArray<APBPlayerState *> PlayerStates;
	PlayerStates = GS->GetTeamPlayers((int32)TeamType);

	bool bHasAnyAlivePlayer = GS->HasAnyAlivePlayer(TeamType);
	if (false == bHasAnyAlivePlayer)
	{
		PlayerStates = GS->GetTeamPlayers((int32)EnemyTeamType);
	}

	bool bHasAnyAliveEnemyPlayer = GS->HasAnyAlivePlayer(EnemyTeamType);
	if (bPreviousStateHasAnyAliveOwnTeamPlayer != bHasAnyAlivePlayer)
	{
		bPreviousStateHasAnyAliveOwnTeamPlayer = bHasAnyAlivePlayer;

		if (false == bHasAnyAlivePlayer)
		{
			//@todo: 같은 팀이 모두 죽어서 적 플레이어를 보여주도록 할 경우.
			RemoveCards();
			GetBPV_PlayerListBox()->ClearChildren();
			//PlayerStates.Reset();

			// 적도 생존자가 없다.
			if (false == bHasAnyAliveEnemyPlayer)
			{
				SetVisibility(ESlateVisibility::Hidden);

				GetBPV_PlayerListBox()->SetVisibility(ESlateVisibility::Hidden);
				GetBPV_PlayerListBox()->ClearChildren();
				GetBPV_Button_L1()->SetVisibility(ESlateVisibility::Hidden);
				GetBPV_Button_R1()->SetVisibility(ESlateVisibility::Hidden);
				GetBPV_SpectatorDescriptionBorder()->SetVisibility(ESlateVisibility::Hidden);
				GetBPV_ReasonWhyViewEnemyBorder()->SetVisibility(ESlateVisibility::Hidden);

				return;
			}
		}
	}
	
	if (ESlateVisibility::Visible != GetVisibility())
	{
		SetVisibility(ESlateVisibility::Visible);

		GetBPV_PlayerListBox()->SetVisibility(ESlateVisibility::Visible);
		GetBPV_Button_L1()->SetVisibility(ESlateVisibility::Visible);
		GetBPV_Button_R1()->SetVisibility(ESlateVisibility::Visible);
		GetBPV_SpectatorDescriptionBorder()->SetVisibility(ESlateVisibility::Visible);
		GetBPV_ReasonWhyViewEnemyBorder()->SetVisibility(ESlateVisibility::Visible);
	}

	// Clean Invalid PlayerState
	Cards.RemoveAll([&](const FPBSpectatorCard &Data)
	{
		bool bHaveToRemove = true;
		for (auto CurPS : PlayerStates)
		{
			auto CurOC = CurPS->GetOwnerCharacter();
			//@todo: 도중에 난입한 경우(Spectator)의 처리를 추가해야함.
			if (CurOC && CurOC->IsSpectator())
			{
				continue;
			}

			if (CurPS->UniqueId == Data.PlayerState->UniqueId)
			{
				bHaveToRemove = false;
				break;
			}
		}

		if (true == bHaveToRemove)
		{
			GetBPV_PlayerListBox()->RemoveChild(Data.Card);
		}

		return bHaveToRemove;
	});

	// Update PlayerState
	for (auto CurPS : PlayerStates)
	{
		//if (CurPS == PS)
		//	continue;

		bool bHaveToAdd = true;
		for (auto CurCard : Cards)
		{
			auto PSOfCurCard = CurCard.PlayerState;
			//auto OCOfCurCard = PSOfCurCard->GetOwnerCharacter();
			if (PSOfCurCard->UniqueId == CurPS->UniqueId)// || (nullptr == OCOfCurCard || false == OCOfCurCard->IsSpectator()))
			{
				bHaveToAdd = false;
				break;
			}
		}

		if (bHaveToAdd)
		{
			EPBTeamType CurPSTeamType = CurPS->GetTeamType();
			if (bHasAnyAlivePlayer)
			{
				if (CurPSTeamType == TeamType)
				{
					AddCard(CurPS, true);
				}
			}
			else
			{
				if (CurPSTeamType != TeamType)
				{
					AddCard(CurPS, true);
				}
			}
		}
	}

	//for (auto CurCard : Cards)
	for (int32 i = 0; i < Cards.Num(); ++i)
	{
		auto CurCard = Cards[i];
		CurCard.Card->UpdateCard(CurCard.PlayerState, i == SelectedCardIndex);
	}

	UpdateFocusedPlayerPanel();
}

void UPBSpectatorModeWidget::SetRefreshRatioInSeconds(float InRefreshRatioInSeconds)
{
	auto OP = GetOwningPlayer();
	if (nullptr == OP || false == OP->IsLocalPlayerController())
		return;

	if (RefreshRatioInSeconds != InRefreshRatioInSeconds || false == RefreshRatioTimerHandle.IsValid())
	{
		if (GetWorld())
		{
			auto &TM = GetWorld()->GetTimerManager();
			TM.ClearTimer(RefreshRatioTimerHandle);
			TM.SetTimer(RefreshRatioTimerHandle, this, &UPBSpectatorModeWidget::UpdatePlayerStates, InRefreshRatioInSeconds, true);
		}

		RefreshRatioInSeconds = InRefreshRatioInSeconds;
	}
}

void UPBSpectatorModeWidget::ClearRefreshRatioInSeconds()
{
	auto OP = GetOwningPlayer();
	if (nullptr == OP || false == OP->IsLocalPlayerController())
		return;

	auto &TM = GetWorld()->GetTimerManager();
	TM.ClearTimer(RefreshRatioTimerHandle);
}

void UPBSpectatorModeWidget::UpdateFocusedPlayerPanel(/*APBPlayerState *InPlayerState*/)
{
	auto OP = GetOwningPlayer();
	if (nullptr == OP || false == OP->IsLocalPlayerController())
		return;

	if (false == ensureMsgf(FocusedPlayerState.IsValid(), TEXT("UPBSpectatorModeWidget::UpdateFocusedPlayerPanel has no FocusedPlayerState")))
		return;

	auto InPlayerState = FocusedPlayerState.Get();

	if (GetVisibility() != ESlateVisibility::Visible)
		return;

	if (GetOwningPlayer() == nullptr || false == GetOwningPlayer()->IsLocalPlayerController())
		return;

	auto OC = GetOwningCharacter();
	if (nullptr == OC)
		return;
	
	auto InOC = InPlayerState->GetOwnerCharacter();
	auto OwnPS = Cast<APBPlayerState>(GetOwningPlayer()->PlayerState);
	
	auto WS = Cast<APBWorldSettings>(GetWorld()->GetWorldSettings());
	FColor TeamColor = WS->GetTeamColor(InOC->GetTeamType());
	
	// describe a target player you're looking
	GetBPV_SpectatorDescriptionBorder()->SetVisibility(ESlateVisibility::Visible);
	GetBPV_SpectatorDescription()->SetText(FText::FromString(FString("CAM BY ") + InPlayerState->GetShortPlayerName()));
	GetBPV_SpectatorDescription()->SetColorAndOpacity(FLinearColor(TeamColor));
	
	// remind reason why you're looking the enemy
	EPBTeamType MyTeam = OwnPS->GetTeamType();
	EPBTeamType ParamTeam = InPlayerState->GetTeamType();
	if (MyTeam == ParamTeam)
	{
		GetBPV_ReasonWhyViewEnemyBorder()->SetVisibility(ESlateVisibility::Hidden);
	}
	else
	{
		GetBPV_ReasonWhyViewEnemyBorder()->SetVisibility(ESlateVisibility::Visible);

		TeamColor = WS->GetTeamColor(OC->GetTeamType());
		GetBPV_ReasonWhyViewEnemy()->SetColorAndOpacity(FLinearColor(TeamColor));
	}

	// target player information
	// Name
	GetBPV_Level_Text()->SetText(FText::FromString("1"));
	GetBPV_UserName_Text()->SetText(FText::FromString(InPlayerState->GetShortPlayerName()));
	//GetBPV_Trophy_Image();

	// Score
	GetBPV_Kill_Value()->SetText(FText::FromString(FString::FromInt(InPlayerState->GetKills())));
	GetBPV_Assist_Value()->SetText(FText::FromString(FString::FromInt(0)));
	GetBPV_Death_Value()->SetText(FText::FromString(FString::FromInt(InPlayerState->GetDeaths())));

	// Weapon
	//GetBPV_Weapon_Image();
	//@todo: Memo는 임시 필드명
	if (InOC)
	{
		APBWeapon *Weapon = InOC->GetCurrentWeapon();
		if (false == ensureMsgf(nullptr != Weapon, TEXT("it could'not reference a Weapon.")))
			return;

		FPBWeaponTableData *WeaponTableData = Weapon->GetTableData();
		if (WeaponTableData)
		{
			GetBPV_Weapon_Text()->SetText(FText::FromString(WeaponTableData->Name));

			//WeaponImage
			//@note: 현재 IconResInfo 리소스 없음.
			//FString IconPath = "/Game/UMG/Images/NewInGame/Icon/IconResInfo_Char/";
			//IconPath += WeaponTableData->IconResInfo + "." + WeaponTableData->IconResInfo;
			//UTexture2D *WeaponImage = Cast<UTexture2D>(StaticLoadObject(UTexture2D::StaticClass(), NULL, *IconPath));
			//@note: IconResInfo 리소스가 있을 때 까지 IconResShop을 사용함.
			FString IconPath = "/Game/UMG/Images/NewInGame/Icon/IconResShop/";
			IconPath += WeaponTableData->IconResShop+ "." + WeaponTableData->IconResShop;
			UTexture2D *WeaponImage = Cast<UTexture2D>(StaticLoadObject(UTexture2D::StaticClass(), nullptr, *IconPath));
			if (WeaponImage)
			{
				GetBPV_Weapon_Image()->SetIsEnabled(true);
				GetBPV_Weapon_Image()->SetBrushFromTexture(WeaponImage, true);
			}
			else
			{
				GetBPV_Weapon_Image()->SetIsEnabled(false);
			}
		}
	}

	FocusCard();
}

//----------------------- UPBSpectatorInfoCard : UPBUserWidget -----------------------
// Switcher
PBGetterWidgetBlueprintVariable_Implementation(UWidgetSwitcher, UPBSpectatorInfoCard, Switcher);

// Normal
PBGetterWidgetBlueprintVariable_Implementation(UCanvasPanel, UPBSpectatorInfoCard, Item_Normal);
PBGetterWidgetBlueprintVariable_Implementation(UImage, UPBSpectatorInfoCard, Normal_Avatar);
PBGetterWidgetBlueprintVariable_Implementation(UTextBlock, UPBSpectatorInfoCard, Normal_UserName);
PBGetterWidgetBlueprintVariable_Implementation(UProgressBar, UPBSpectatorInfoCard, Normal_HealthBar);
PBGetterWidgetBlueprintVariable_Implementation(UTextBlock, UPBSpectatorInfoCard, Normal_LevelLabel);

// Selected
PBGetterWidgetBlueprintVariable_Implementation(UCanvasPanel, UPBSpectatorInfoCard, Item_Selected);
PBGetterWidgetBlueprintVariable_Implementation(UImage, UPBSpectatorInfoCard, Selected_Avatar);
PBGetterWidgetBlueprintVariable_Implementation(UTextBlock, UPBSpectatorInfoCard, Selected_UserName);
PBGetterWidgetBlueprintVariable_Implementation(UProgressBar, UPBSpectatorInfoCard, Selected_HealthBar);
PBGetterWidgetBlueprintVariable_Implementation(UTextBlock, UPBSpectatorInfoCard, Selected_LevelLabel);

// Death
PBGetterWidgetBlueprintVariable_Implementation(UCanvasPanel, UPBSpectatorInfoCard, Item_Death);
PBGetterWidgetBlueprintVariable_Implementation(UImage, UPBSpectatorInfoCard, Death_Avatar);
PBGetterWidgetBlueprintVariable_Implementation(UTextBlock, UPBSpectatorInfoCard, Death_UserName);
PBGetterWidgetBlueprintVariable_Implementation(UProgressBar, UPBSpectatorInfoCard, Death_HealthBar);
PBGetterWidgetBlueprintVariable_Implementation(UTextBlock, UPBSpectatorInfoCard, Death_LevelLabel);

void UPBSpectatorInfoCard::NativeConstruct()
{
	Super::NativeConstruct();

	// For Caching
	{
		GetBPV_Switcher();
		{
			GetBPV_Item_Normal();
			GetBPV_Item_Selected();
			GetBPV_Item_Death();
		}
		SetActiveSwitcherItemByStatus(Data.CurrentCharacterStatus);

		GetBPV_Normal_Avatar();
		GetBPV_Normal_UserName()->SetText(Data.UserName);
		GetBPV_Normal_HealthBar()->SetPercent(Data.HealthPercentage);
		GetBPV_Normal_LevelLabel()->SetText(FText::FromString(FString::FromInt(Data.Level)));

		GetBPV_Selected_Avatar();
		GetBPV_Selected_UserName()->SetText(Data.UserName);
		GetBPV_Selected_HealthBar()->SetPercent(Data.HealthPercentage);
		GetBPV_Selected_LevelLabel()->SetText(FText::FromString(FString::FromInt(Data.Level)));

		GetBPV_Death_Avatar();
		GetBPV_Death_UserName()->SetText(Data.UserName);
		GetBPV_Death_HealthBar()->SetPercent(Data.HealthPercentage);
		GetBPV_Death_LevelLabel()->SetText(FText::FromString(FString::FromInt(Data.Level)));
	}
}

void UPBSpectatorInfoCard::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
}

void UPBSpectatorInfoCard::NativeDestruct()
{
	Super::NativeDestruct();
}

void UPBSpectatorInfoCard::NotifyDeath(class APBPlayerState* KillerPlayerState, class APBPlayerState* VictimPlayerState, const UDamageType* KillerDamageType, const FTakeDamageInfo& TakeDamageInfo)
{
}

bool UPBSpectatorInfoCard::IsSelected() const
{
	return Data.CurrentCharacterStatus == EPBSpectatorInfoCardStatus::Selected;
}

void UPBSpectatorInfoCard::SetActiveSwitcherItemByStatus(EPBSpectatorInfoCardStatus CardStatus)
{	
	auto Switcher = GetBPV_Switcher();
	int32 ActiveWidgetIndex = Switcher->GetActiveWidgetIndex();
	auto ActivatedWidget = Cast<UCanvasPanel>(Switcher->GetWidgetAtIndex(ActiveWidgetIndex));

	for (int32 i = 0; i < Switcher->GetNumWidgets(); ++i)
	{
		auto CurWidget = Switcher->GetWidgetAtIndex(i);
		if (nullptr == CurWidget || ActivatedWidget == CurWidget)
			continue;

		if ((CardStatus == EPBSpectatorInfoCardStatus::Death && CurWidget == GetBPV_Item_Death())
			|| (CardStatus == EPBSpectatorInfoCardStatus::Normal && CurWidget == GetBPV_Item_Normal())
			|| (CardStatus == EPBSpectatorInfoCardStatus::Selected && CurWidget == GetBPV_Item_Selected()))
		{
			Data.CurrentCharacterStatus = CardStatus;
			Switcher->SetActiveWidgetIndex(i);
			return;
		}
	}
}

void UPBSpectatorInfoCard::UpdateCard(APBPlayerState *InPlayerState, bool bSelectedCard)
{
	//@todo: 테이블에서 적합한 데이터를 찾아 적용해야함.
	auto OC = InPlayerState->GetOwnerCharacter();
	if (nullptr == OC)
		return;

	EPBSpectatorInfoCardStatus CurrentCharacterStatus = bSelectedCard ? EPBSpectatorInfoCardStatus::Selected : (EPBSpectatorInfoCardStatus)(OC->IsAlive());
	if (CurrentCharacterStatus != Data.CurrentCharacterStatus)
	{
		SetActiveSwitcherItemByStatus(CurrentCharacterStatus);
		Data.CurrentCharacterStatus = CurrentCharacterStatus;
	}

	Data.UserName = FText::FromString(InPlayerState->GetShortPlayerName());
	Data.HealthPercentage = OC->GetHealthPercentage();
	Data.Level = 1;
	
	
	auto WS = Cast<APBWorldSettings>(GetWorld()->GetWorldSettings());
	FColor TeamColor = WS->GetTeamColor(OC->GetTeamType());
	if (GetIsEnabled())
	{
		UTexture2D *AvatarImage = nullptr;
		auto CharacterTable = OC->GetTableData();
		if (CharacterTable)
		{
			FString IconPath = "/Game/UMG/Images/NewInGame/Icon/IconCharShop/";
			IconPath += CharacterTable->IconResShop + "." + CharacterTable->IconResShop;
			AvatarImage = Cast<UTexture2D>(StaticLoadObject(UTexture2D::StaticClass(), nullptr, *IconPath));
		}

		if (AvatarImage)
		{
			GetBPV_Normal_Avatar()->SetIsEnabled(true);
			GetBPV_Normal_Avatar()->SetBrushFromTexture(AvatarImage, true);
		}
		else
		{
			GetBPV_Normal_Avatar()->SetIsEnabled(false);
		}
		
		GetBPV_Normal_UserName()->SetText(Data.UserName);
		GetBPV_Normal_UserName()->SetColorAndOpacity(FLinearColor(TeamColor));
		GetBPV_Normal_HealthBar()->SetPercent(Data.HealthPercentage);
		//GetBPV_Normal_HealthBar()->SetFillColorAndOpacity(FLinearColor(TeamColor));
		GetBPV_Normal_LevelLabel()->SetText(FText::FromString(FString::FromInt(Data.Level)));

		if (AvatarImage)
		{
			GetBPV_Selected_Avatar()->SetIsEnabled(true);
			GetBPV_Selected_Avatar()->SetBrushFromTexture(AvatarImage, true);
		}
		else
		{
			GetBPV_Selected_Avatar()->SetIsEnabled(false);
		}

		GetBPV_Selected_UserName()->SetText(Data.UserName);
		GetBPV_Selected_UserName()->SetColorAndOpacity(FLinearColor(TeamColor));
		GetBPV_Selected_HealthBar()->SetPercent(Data.HealthPercentage);
		//GetBPV_Selected_HealthBar()->SetFillColorAndOpacity(FLinearColor(TeamColor));
		GetBPV_Selected_LevelLabel()->SetText(FText::FromString(FString::FromInt(Data.Level)));

		if (AvatarImage)
		{
			GetBPV_Death_Avatar()->SetIsEnabled(true);
			GetBPV_Death_Avatar()->SetBrushFromTexture(AvatarImage, true);
		}
		else
		{
			GetBPV_Death_Avatar()->SetIsEnabled(false);
		}
		GetBPV_Death_UserName()->SetText(Data.UserName);
		GetBPV_Death_UserName()->SetColorAndOpacity(FLinearColor(TeamColor));
		GetBPV_Death_HealthBar()->SetPercent(Data.HealthPercentage);
		//GetBPV_Death_HealthBar()->SetFillColorAndOpacity(FLinearColor(TeamColor));
		GetBPV_Death_LevelLabel()->SetText(FText::FromString(FString::FromInt(Data.Level)));
	}
}