// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "PBMode_TeamDeathMatch.generated.h"

class APBPlayerState;
class APBAIController;

UCLASS(config = Game, HideDropdown)
class APBMode_TeamDeathMatch : public APBGameMode
{
	GENERATED_BODY()

public:

	APBMode_TeamDeathMatch();

	virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;


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

	int32 GetTargetTeamScore(const FString& Options) override;

	/* ------------------------------------------------------------------------------- */
	//  Missions
	/* ------------------------------------------------------------------------------- */

	class APBMission* CreateMissionForTeam(EPBTeamType Team) override;

	/* ------------------------------------------------------------------------------- */
	// Respawn
	/* ------------------------------------------------------------------------------- */

	virtual bool IsSpawnpointAllowed(APlayerStart* SpawnPoint, AController* Player);

	/* ------------------------------------------------------------------------------- */
	// Damage Modification
	/* ------------------------------------------------------------------------------- */
	 
	/** can players damage each other? */
	virtual bool CanDealDamage(APBPlayerState* DamageInstigator, APBPlayerState* DamagedPlayer) const override;

	/* ------------------------------------------------------------------------------- */
	// Final Match
	/* ------------------------------------------------------------------------------- */

	bool MatchIsFinalMatch() override;

	/* ------------------------------------------------------------------------------- */
	// End
	/* ------------------------------------------------------------------------------- */
};
