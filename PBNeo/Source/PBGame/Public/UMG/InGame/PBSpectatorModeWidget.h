// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UMG/PBUserWidget.h"
#include "PBSpectatorModeWidget.generated.h"

UENUM()
enum class EPBSpectatorInfoCardStatus : uint8
{
	Death = 0,
	Normal,
	Selected
};

USTRUCT()
struct FPBSpectatorCard
{
	GENERATED_BODY()

	APBPlayerState *PlayerState;
	class UPBSpectatorInfoCard *Card;
};

/**
 * 
 */
UCLASS()
class PBGAME_API UPBSpectatorModeWidget : public UPBSubWidget
{
	GENERATED_BODY()
	
	// Player List
	PBGetterWidgetBlueprintVariable(UScrollBox, PlayerListBox);
	PBGetterWidgetBlueprintVariable(UButton, Button_L1);
	PBGetterWidgetBlueprintVariable(UButton, Button_R1);
	PBGetterWidgetBlueprintVariable(UBorder, SpectatorDescriptionBorder);
	PBGetterWidgetBlueprintVariable(UTextBlock, SpectatorDescription);
	PBGetterWidgetBlueprintVariable(UBorder, ReasonWhyViewEnemyBorder);
	PBGetterWidgetBlueprintVariable(UTextBlock, ReasonWhyViewEnemy);

	// Observable Player Info
	// Name
	PBGetterWidgetBlueprintVariable(UTextBlock, Level_Text);
	PBGetterWidgetBlueprintVariable(UTextBlock, UserName_Text);
	PBGetterWidgetBlueprintVariable(UImage, Trophy_Image);
	// Score
	PBGetterWidgetBlueprintVariable(UTextBlock, Kill_Value);
	PBGetterWidgetBlueprintVariable(UTextBlock, Assist_Value);
	PBGetterWidgetBlueprintVariable(UTextBlock, Death_Value);
	// Weapon
	PBGetterWidgetBlueprintVariable(UImage, Weapon_Image);
	PBGetterWidgetBlueprintVariable(UTextBlock, Weapon_Text);
	// System
	PBGetterWidgetBlueprintVariable(UTextBlock, ChangeLoadoutText);

	void NativeConstruct() override;
	void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	virtual void NativeDestruct() override;

public:
	UPBSpectatorModeWidget();

public:
	void NotifySpawn(class APBPlayerState* PlayerState);
	void NotifyDeath(class APBPlayerState* KillerPlayerState, class APBPlayerState* KilledPlayerState, const UDamageType* KillerDamageType, const FTakeDamageInfo& TakeDamageInfo);
	//void NotifyBecomeFollower(class APBPlayerState* TargetPlayer);
	void NotifyEnterFollowCamera(class APBPlayerState* TargetPlayer, bool bTryToGetNextPlayer, bool bPlaySound);

protected:
	//void UpdateCard(class UPBSpectatorInfoCard *InCard, class APBPlayerState *InPlayerState);
	void AddCards(EPBTeamType TeamType);
	// bOnlyCreateWidget를 true로 하면 Widget만 생성하도록 한다. false인경우 PlayerState에 따른 내용으로 설정도 한다.
	void AddCard(class APBPlayerState *PlayerState, bool bOnlyCreateWidget);
	void RemoveCards();
	void FocusCard(/*class APBPlayerState *PlayerState*/);
	void UpdatePlayerStates();

protected:
	UFUNCTION(BlueprintCallable, Category = "SpectatorMode")
	void SetRefreshRatioInSeconds(float InRefreshRatioInSeconds);
	void ClearRefreshRatioInSeconds();

	UFUNCTION(BlueprintCallable, Category = "SpectatorMode")
	void UpdateFocusedPlayerPanel(/*APBPlayerState *InPlayerState*/);

protected:
	UPROPERTY(EditDefaultsOnly, Category = "SpectatorMode")
	TSubclassOf<class UPBSpectatorInfoCard> SpectatorInfoCardClass;

	// 선택된 것이 없을 경우에는 INDEX_NONE 값을 갖는다.
	UPROPERTY(BlueprintReadOnly, Category = "SpectatorMode")
	int32 SelectedCardIndex;

	UPROPERTY(BlueprintReadOnly, Category = "SpectatorMode")
	TArray<FPBSpectatorCard> Cards;

	UPROPERTY(EditDefaultsOnly, Category = "SpectatorMode")
	float RefreshRatioInSeconds;

	UPROPERTY()
	TWeakObjectPtr<APBPlayerState> FocusedPlayerState;

	UPROPERTY(EditDefaultsOnly, Category = Sound)
	USoundCue* SoundPreviousButton;

	UPROPERTY(EditDefaultsOnly, Category = Sound)
	USoundCue* SoundNextButton;

private:
	FTimerHandle RefreshRatioTimerHandle;
	bool bPreviousStateHasAnyAliveOwnTeamPlayer;
};

USTRUCT()
struct FPBSpectatorInfoCardData
{
	GENERATED_BODY()

	int32 Team;
	EPBSpectatorInfoCardStatus CurrentCharacterStatus;
	FText UserName;
	float HealthPercentage;
	int32 Level;
};

UCLASS()
class PBGAME_API UPBSpectatorInfoCard : public UPBUserWidget
{
	GENERATED_BODY()

	// Switcher
	PBGetterWidgetBlueprintVariable(UWidgetSwitcher, Switcher);

	// Normal
	PBGetterWidgetBlueprintVariable(UCanvasPanel, Item_Normal);
	PBGetterWidgetBlueprintVariable(UImage, Normal_Avatar);
	PBGetterWidgetBlueprintVariable(UTextBlock, Normal_UserName);
	PBGetterWidgetBlueprintVariable(UProgressBar, Normal_HealthBar);
	PBGetterWidgetBlueprintVariable(UTextBlock, Normal_LevelLabel);

	// Selected
	PBGetterWidgetBlueprintVariable(UCanvasPanel, Item_Selected);
	PBGetterWidgetBlueprintVariable(UImage, Selected_Avatar);
	PBGetterWidgetBlueprintVariable(UTextBlock, Selected_UserName);
	PBGetterWidgetBlueprintVariable(UProgressBar, Selected_HealthBar);
	PBGetterWidgetBlueprintVariable(UTextBlock, Selected_LevelLabel);

	// Death
	PBGetterWidgetBlueprintVariable(UCanvasPanel, Item_Death);
	PBGetterWidgetBlueprintVariable(UImage, Death_Avatar);
	PBGetterWidgetBlueprintVariable(UTextBlock, Death_UserName);
	PBGetterWidgetBlueprintVariable(UProgressBar, Death_HealthBar);
	PBGetterWidgetBlueprintVariable(UTextBlock, Death_LevelLabel);

protected:
	void NativeConstruct() override;
	void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	virtual void NativeDestruct() override;
	
public:
	void NotifyDeath(class APBPlayerState* KillerPlayerState, class APBPlayerState* VictimPlayerState, const UDamageType* KillerDamageType, const FTakeDamageInfo& TakeDamageInfo);
	
public:
	bool IsSelected() const;
	void SetActiveSwitcherItemByStatus(EPBSpectatorInfoCardStatus CardStatus);
	void UpdateCard(APBPlayerState *InPlayerState, bool bSelectedCard);

public:
	FPBSpectatorInfoCardData Data;
};
