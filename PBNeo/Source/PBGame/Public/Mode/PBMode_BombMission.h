// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Mode/PBGameMode.h"
#include "PBMode_BombMission.generated.h"

/**
 * 
 */
UCLASS(Config = Game)
class PBGAME_API APBMode_BombMission : public APBGameMode
{
	GENERATED_BODY()


public:

	APBMode_BombMission();
	virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;


private:
	/* ------------------------------------------------------------------------------- */
	// Team default pawns
	/* ------------------------------------------------------------------------------- */

	UClass* GetDefaultPawnClassForController_Implementation(AController* InController) override;

public:
	/** The default pawn class used by players. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Classes)
	TSubclassOf<class APawn> PawnClassTeamA;

	/** The default pawn class used by players. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Classes)
	TSubclassOf<class APawn> PawnClassTeamB;

	/* ------------------------------------------------------------------------------- */
	// Teams
	/* ------------------------------------------------------------------------------- */

	/** pick team with least players in or random when it's equal */
	EPBTeamType ChooseTeam(APBPlayerState* ForPlayerState) const override;

	/* ------------------------------------------------------------------------------- */
	//  Team Score
	/* ------------------------------------------------------------------------------- */

	UPROPERTY(config)
	int32 TargetTeamScoreBombMissionOnly_DEPRECATED;

	int32 GetTargetTeamScore(const FString& Options) override;

	/* ------------------------------------------------------------------------------- */
	//  Missions
	/* ------------------------------------------------------------------------------- */

	class APBMission* CreateMissionForTeam(EPBTeamType Team) override;

	// Defuse all the not-yet-exploded bombs
	virtual  void PreConcludeMatch();

	/* ------------------------------------------------------------------------------- */
	// Respawn
	/* ------------------------------------------------------------------------------- */

	bool IsSpawnpointAllowed(APlayerStart* SpawnPoint, AController* Player) override;

	/* ------------------------------------------------------------------------------- */
	// Damage Modification
	/* ------------------------------------------------------------------------------- */

	/** can players damage each other? */
	bool CanDealDamage(APBPlayerState* DamageInstigator, APBPlayerState* DamagedPlayer) const override;

	/* ------------------------------------------------------------------------------- */
	// Mode Special Weapon
	/* ------------------------------------------------------------------------------- */

	UPROPERTY(EditDefaultsOnly, Category = Classes)
	FWeaponModeSetClass BombWeaponClass;

	UPROPERTY(EditDefaultsOnly, Category = Classes)
	FWeaponModeSetClass DefuseKitClass;

	void AddSpecifyWeaponsTo(APBCharacter* Pawn) override;

	/* ------------------------------------------------------------------------------- */
	// Final Match
	/* ------------------------------------------------------------------------------- */

	bool MatchIsFinalMatch() override;

	/* ------------------------------------------------------------------------------- */
	// End
	/* ------------------------------------------------------------------------------- */
	
	
};
