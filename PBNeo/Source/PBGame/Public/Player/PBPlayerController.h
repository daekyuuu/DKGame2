// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Online.h"
#include "FPBWidgetEventDispatcher.h"
#include "Online/PBUserInfo.h"
#include "PBPlayerController.generated.h"

UCLASS(config=Game)
class APBPlayerController : public APlayerController
{
	GENERATED_UCLASS_BODY()

	virtual void PostInitializeComponents() override;

	virtual void SetPlayer(UPlayer* Player) override;

	virtual void InitInputSystem() override;

	virtual void SetupInputComponent() override;

	virtual void ClientGameEnded_Implementation(class AActor* EndGameFocus, bool bIsWinner) override;

	virtual void PreClientTravel(const FString& PendingGameURL, ETravelType TravelType, bool bIsSeamlessTravel) override;

	virtual void OnRep_PlayerState() override;
	
	virtual void ReceivedSpectatorClass(TSubclassOf<class ASpectatorPawn> SpectatorClass) override;

	virtual void BeginPlay() override;

	virtual void BeginDestroy() override;

	virtual bool CanRestartPlayer() override;

	virtual void GetSeamlessTravelActorList(bool bToEntry, TArray<class AActor*>& ActorList) override;

	virtual void CalcCamera(float DeltaTime, struct FMinimalViewInfo& OutResult) override;
	virtual void GetActorEyesViewPoint(FVector& Location, FRotator& Rotation) const override;

	virtual void SetInitialLocationAndRotation(const FVector& NewLocation, const FRotator& NewRotation) override;

	virtual void SetSpawnRotation(const FRotator& NewRotation);
	FRotator GetSpawnRotation() { return SpawnRotation; }
	UPROPERTY(Replicated)
	FRotator SpawnRotation;

	/* ------------------------------------------------------------------------------- */
	// Notify : Game Started
	/* ------------------------------------------------------------------------------- */

public:

	UFUNCTION(reliable, client)
	void ClientGameStarted();

	/* ------------------------------------------------------------------------------- */
	// Notify : Warmup
	/* ------------------------------------------------------------------------------- */

public:

	UFUNCTION(reliable, client)
	void ClientWarmupStarted();

	UFUNCTION(reliable, client)
	void ClientWarmupEnded();

	/* ------------------------------------------------------------------------------- */
	// Notify : Conclude Match
	/* ------------------------------------------------------------------------------- */

public:

	UFUNCTION(reliable, client)
	void ClientConcludeMatchStarted();

	UFUNCTION(reliable, client)
	void ClientConcludeMatchEnded();

	/* ------------------------------------------------------------------------------- */
	// Notify : Conclude All Matches
	/* ------------------------------------------------------------------------------- */

public:

	UFUNCTION(reliable, client)
	void ClientConcludeAllMatchesStarted();

	UFUNCTION(reliable, client)
	void ClientConcludeAllMatchesEnded();

	/* ------------------------------------------------------------------------------- */
	// Notify : Death
	/* ------------------------------------------------------------------------------- */

public:

	void NotifyDeath(class APBPlayerState* KillerPlayerState, class APBPlayerState* KilledPlayerState, const UDamageType* KillerDamageType, const FTakeDamageInfo& TakeDamageInfo);

	/* ------------------------------------------------------------------------------- */
	// Notify : Spawn
	/* ------------------------------------------------------------------------------- */

public:

	void NotifySpawn(class APBPlayerState* SpawnedPlayerState);


	/* ------------------------------------------------------------------------------- */
	// ServerTravel Loading Screen
	/* ------------------------------------------------------------------------------- */

protected: // Implementation

	// Show loading screen as we travel between levels in game
	void ShowLoadingScreenOnTravel();

	/* ------------------------------------------------------------------------------- */
	// Exit To Main Menu
	/* ------------------------------------------------------------------------------- */

	// When we are made to return to main menu by host
	void ClientReturnToMainMenu_Implementation(const FString& ReturnReason) override;

	/* ------------------------------------------------------------------------------- */
	// Input Management
	/* ------------------------------------------------------------------------------- */

public:
	void SetInputAllowed(bool Value);
	bool IsGameInputAllowed() const;

protected:
	void InitInputManagement();

protected:
	uint8 bAllowGameActions : 1;

	/* ------------------------------------------------------------------------------- */
	// Unbeatable
	/* ------------------------------------------------------------------------------- */

protected:
	void GrantUnbeatability();

	/* ------------------------------------------------------------------------------- */
	// ReSpawn
	/* ------------------------------------------------------------------------------- */
public:
	virtual void Possess(APawn* aPawn) override;
	virtual void UnPossess() override;

public:
	void RestartPlayerCharacter();

	/* ------------------------------------------------------------------------------- */
	// Widget Event Dispatcher
	/* ------------------------------------------------------------------------------- */

public:
	TSharedPtr<FPBWidgetEventDispatcher> WidgetEventDispatcher;

	/* ------------------------------------------------------------------------------- */
	// In game Menu
	/* ------------------------------------------------------------------------------- */

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Widget)
	TSubclassOf<class UPBInGameMenuWidget> InGameMenuClass;

	void ShowInGameMenu();
	void OnToggleInGameMenu();
	bool IsGameMenuVisible() const;

protected:
	void InitInGameMenu();
	void StartUpInGameMenu();
	void RegisterInGameMenuInputHandlers();

	UPROPERTY(BlueprintReadOnly, Category = Widget)
	class UPBInGameMenuWidget* InGameMenuWidget;

	/* ------------------------------------------------------------------------------- */
	// Scene Capture
	/* ------------------------------------------------------------------------------- */

private:
	void InitSceneCaptureComp();
	void StartUpSceneCaptureComp();

protected:

	UPROPERTY()
		class UPBSceneCaptureComponent2D* PBSceneCaptureComp2D;

	UPROPERTY(EditAnywhere, Category = "Blur")
		UTextureRenderTarget2D* RenderTargetForBlur;

	/* ------------------------------------------------------------------------------- */
	// InGameWidget
	/* ------------------------------------------------------------------------------- */

public:
	UFUNCTION(BlueprintCallable, Category = Widget, Reliable, Client)
	void ClientSetInGameWidget(TSubclassOf<class UPBInGameWidget> NewInGameWidgetCLass);

	UFUNCTION(BlueprintCallable, Category = Widget)
	class UPBInGameWidget* GetInGameWidget();

protected:
	void InitInGameWidget();
	void SetInGameWidgetState(FName MatchState, uint8 MatchSubstateIndex = 0);
	void NotifyMatchResultToInGameWidget(bool bIsWinner);
	void CleanUpInGameWidget();

	UPROPERTY(BlueprintReadOnly, Category = Widget)
	class UPBInGameWidget* InGameWidget;

	/* ------------------------------------------------------------------------------- */
	// Situation Board Widget Notifies
	/* ------------------------------------------------------------------------------- */

public:
	void OnToggleScoreboard();
	void OnShowScoreboard();
	void OnHideScoreboard();

protected:
	void RegisterSituationBoardInputHandlers();

	/* ------------------------------------------------------------------------------- */
	// Death Widget Notifies
	/* ------------------------------------------------------------------------------- */

protected:
	void NotifyWidgetOfDeath(class APBPlayerState* KillerPlayerState, class APBPlayerState* KilledPlayerState, const UDamageType* KillerDamageType, const FTakeDamageInfo& TakeDamageInfo);

	/* ------------------------------------------------------------------------------- */
	// BGM
	/* ------------------------------------------------------------------------------- */

protected:
	void PlayBGM();
	void FadeBGM();

	/* ------------------------------------------------------------------------------- */
	// Save Data
	/* ------------------------------------------------------------------------------- */

public:
	APBPlayerState* GetPBPlayerState();
	class UPBPersistentUser* GetPersistentUser() const;
	struct FPBUserBaseInfo& GetPersistentUserBaseInfo();

	// 1. PlayerState 가 최초 리플리케이트 되었다면 내정보(캐릭터 및 무기정보)와 함께 서버에 게임 시작을 요청한다.
	// 2. 요청을 받은 서버는 유저의 상태를 체크해서 바로 게임 시작, 대기, 관전등을 유저상태와 게임모드에 따라 선택한다.
	UFUNCTION(reliable, server, WithValidation)
	void ServerUserReadyToStartMatch(const FPBUserBaseInfo& MyUserBaseInfo);

	void SetPlayerStateInitiallyReplicated();
	void SetReplicateUserBaseInfo(const FPBUserBaseInfo& MyUserBaseInfo);
	void SetPlayerStarted();

	// Valid everywhere. Once the PlayerState is replicated, the client is ready to start the game.
	bool UserReadyToStart();
	// Valid only on server. Mark if the client has started the game.
	bool UserStarted();

protected:
	void LoadSavedKeybindings();

	void UpdateSaveFileOnGameEnd(bool bIsWinner);

	/* ------------------------------------------------------------------------------- */
	// Session
	/* ------------------------------------------------------------------------------- */

public:
	/** Starts the online game using the session name in the PlayerState */
	UFUNCTION(reliable, client)
	void ClientStartOnlineGame();

	/** Ends the online game using the session name in the PlayerState */
	UFUNCTION(reliable, client)
	void ClientEndOnlineGame();	

private:
	/** Handle for efficient management of ClientStartOnlineGame timer */
	FTimerHandle TimerHandle_ClientStartOnlineGame;

	/* ------------------------------------------------------------------------------- */
	// Cheats
	/* ------------------------------------------------------------------------------- */

public:
	/** sends cheat message */
	UFUNCTION(reliable, server, WithValidation)
	void ServerCheat(const FString& Msg);
	
	void SetInfiniteAmmo(bool bEnable);
	bool HasInfiniteAmmo() const;

	void SetInfiniteClip(bool bEnable);
	bool HasInfiniteClip() const;

	void SetHealthRegen(bool bEnable);
	bool HasHealthRegen() const;

	void SetGodMode(bool bEnable);
	bool HasGodMode() const;

	virtual void Suicide();

protected:
	void StartUpCheatSys();

protected:

	/** infinite ammo cheat */
	UPROPERTY(Transient, Replicated)
	uint8 bInfiniteAmmo : 1;

	/** infinite clip cheat */
	UPROPERTY(Transient, Replicated)
	uint8 bInfiniteClip : 1;

	/** health regen cheat */
	UPROPERTY(Transient, Replicated)
	uint8 bHealthRegen : 1;

	/** god mode cheat */
	UPROPERTY(Transient, Replicated)
	uint8 bGodMode : 1;

	FTimerHandle TimerHandle_Unbeatable;

	/* ------------------------------------------------------------------------------- */
	// End
	/* ------------------------------------------------------------------------------- */
};

