// Fill out your copyright notice in the Description page of Project Settings.

#include "PBGame.h"
#include "PBPlayerState.h"
#include "PBMode_Escape_AI.h"
#include "PBMission_Escape_AI.h"

APBMode_Escape_AI::APBMode_Escape_AI()
{
	PlayerKillScore = 0;
	PlayerDeathScore = 0;
}

EPBTeamType APBMode_Escape_AI::ChooseTeam(APBPlayerState* ForPlayerState) const
{
	return ForPlayerState->bIsABot ? EPBTeamType::Alpha : EPBTeamType::Bravo;
}

int32 APBMode_Escape_AI::GetTargetTeamScore(const FString& Options)
{
	return 1;
}

class APBMission* APBMode_Escape_AI::CreateMissionForTeam(EPBTeamType Team)
{
	UWorld* World = GetWorld();
	if (World && GameState)
	{
		UClass* MissionClassToUse;
		if (Team == EPBTeamType::Bravo)
		{
			MissionClassToUse = APBMission_Escape_AI::StaticClass();
		}
		else
		{
			MissionClassToUse = APBMission::StaticClass();
		}

		APBMission* NewMission =
			Cast<APBMission>(UPBGameplayStatics::BeginDeferredActorSpawnFromClass(GameState, MissionClassToUse, FTransform::Identity, ESpawnActorCollisionHandlingMethod::AlwaysSpawn, GameState));

		NewMission->Init(Team);

		UPBGameplayStatics::FinishSpawningActor(NewMission, FTransform::Identity);

		return NewMission;
	}

	return nullptr;
}

bool APBMode_Escape_AI::CanDealDamage(APBPlayerState* DamageInstigator, APBPlayerState* DamagedPlayer) const
{
	return DamageInstigator && DamagedPlayer && (DamagedPlayer == DamageInstigator || DamagedPlayer->GetTeamType() != DamageInstigator->GetTeamType());
}

bool APBMode_Escape_AI::MatchIsFinalMatch()
{
	return true;
}