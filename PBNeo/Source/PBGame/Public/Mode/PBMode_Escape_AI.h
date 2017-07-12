// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Mode/PBGameMode.h"
#include "PBMode_Escape_AI.generated.h"

/**
 * 
 */
UCLASS(config = Game, HideDropdown)
class PBGAME_API APBMode_Escape_AI : public APBGameMode
{
	GENERATED_BODY()

public:
	APBMode_Escape_AI();

	/* ------------------------------------------------------------------------------- */
	// Teams
	/* ------------------------------------------------------------------------------- */

	virtual EPBTeamType ChooseTeam(APBPlayerState* ForPlayerState) const override;

	/* ------------------------------------------------------------------------------- */
	//  Team Score
	/* ------------------------------------------------------------------------------- */

	int32 GetTargetTeamScore(const FString& Options) override;
	
	/* ------------------------------------------------------------------------------- */
	//  Missions
	/* ------------------------------------------------------------------------------- */

	class APBMission* CreateMissionForTeam(EPBTeamType Team) override;

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
