// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "PBGame.h"
#include "PBMode_TeamDeathMatch.h"
#include "Player/PBPlayerState.h"
#include "Mode/SpawnPoint/PBPlayerStart.h"
#include "PBMission_TeamDeathMatch.h"

APBMode_TeamDeathMatch::APBMode_TeamDeathMatch()
{
	PlayerKillScore = 1;
	PlayerDeathScore = 0;

	PickupDestroyDelay = 10.0f;
}

void APBMode_TeamDeathMatch::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);

	APBGameState* GS = Cast<APBGameState>(GameState);
	if (GS)
	{
		GS->SetGameModeType(EPBGameModeType::TDM);
	}
}

UClass* APBMode_TeamDeathMatch::GetDefaultPawnClassForController_Implementation(AController* InController)
{
	if (GetWorld() && GetWorld()->IsPlayInEditor())
	{
		APBPlayerState* PlayerState = Cast<APBPlayerState>(InController->PlayerState);
		if (PlayerState)
		{
			switch (PlayerState->GetTeamType())
			{
			case EPBTeamType::Alpha:
				return PawnClassTeamA;
			case EPBTeamType::Bravo:
				return PawnClassTeamB;
			}
		}
	}

	return Super::GetDefaultPawnClassForController_Implementation(InController);
}

EPBTeamType APBMode_TeamDeathMatch::ChooseTeam(APBPlayerState* ForPlayerState) const
{
	// 플레이어가 들어오면 인원이 적은 팀으로 배정한다.

	TArray<int32> TeamPlayerNums;
	TeamPlayerNums.AddZeroed((int)EPBTeamType::Max);

	// get current team player's count   각팀의 인원수를 구한다
	for (int32 i = 0; i < GameState->PlayerArray.Num(); i++)
	{
		APBPlayerState const* const TestPlayerState = Cast<APBPlayerState>(GameState->PlayerArray[i]);
		int32 TeamIndex = (int32)TestPlayerState->GetTeamType();

		if (TestPlayerState && TestPlayerState != ForPlayerState && TeamPlayerNums.IsValidIndex(TeamIndex))
		{
			TeamPlayerNums[TeamIndex]++;
		}
	}

	// find least populated one,		가장 적은 인원수를 찾는다.
	int32 LeastPlayerNum = TeamPlayerNums[0];
	for (int32 i = 1; i < TeamPlayerNums.Num(); i++)
	{
		if (LeastPlayerNum > TeamPlayerNums[i])
		{
			LeastPlayerNum = TeamPlayerNums[i];
		}
	}

	// there could be more than one...,	 그 인원수 만큼 보유한 팀들을 찾고
	TArray<int32> BestTeams;
	for (int32 i = 0; i < TeamPlayerNums.Num(); i++)
	{
		if (TeamPlayerNums[i] == LeastPlayerNum)
		{
			BestTeams.Add(i);
		}
	}

	// get random from best list	,	인원이 가장 적은 팀들중에서 한 팀을 선택한다.
	const int32 RandomBestTeam = BestTeams[FMath::RandHelper(BestTeams.Num())];
	return (EPBTeamType)RandomBestTeam;
}

int32 APBMode_TeamDeathMatch::GetTargetTeamScore(const FString& Options)
{
	const APBMission_TeamDeathMatch* Mission = GetDefault<APBMission_TeamDeathMatch>(APBMission_TeamDeathMatch::StaticClass());
	if (Mission)
	{
		return Mission->TargetMatchPoint;
	}

	return Super::GetTargetTeamScore(Options);
}

class APBMission* APBMode_TeamDeathMatch::CreateMissionForTeam(EPBTeamType Team)
{
	UWorld* World = GetWorld();
	if (World && GameState)
	{
		APBMission* NewMission =
			Cast<APBMission>(UPBGameplayStatics::BeginDeferredActorSpawnFromClass(GameState, APBMission_TeamDeathMatch::StaticClass(), FTransform::Identity, ESpawnActorCollisionHandlingMethod::AlwaysSpawn, GameState));

		NewMission->Init(Team);

		UPBGameplayStatics::FinishSpawningActor(NewMission, FTransform::Identity);

		return NewMission;
	}

	return nullptr;
}

bool APBMode_TeamDeathMatch::IsSpawnpointAllowed(APlayerStart* SpawnPoint, AController* Player)
{
	if (Player)
	{
		APBPlayerStart* TeamStart = Cast<APBPlayerStart>(SpawnPoint);
		APBPlayerState* PlayerState = Cast<APBPlayerState>(Player->PlayerState);

		if (PlayerState && TeamStart && TeamStart->SpawnTeam != PlayerState->GetTeamType())
		{
			return false;
		}
	}

	return Super::IsSpawnpointAllowed(SpawnPoint, Player);
}

bool APBMode_TeamDeathMatch::CanDealDamage(APBPlayerState* DamageInstigator, class APBPlayerState* DamagedPlayer) const
{
	return DamageInstigator && DamagedPlayer && (DamagedPlayer == DamageInstigator || DamagedPlayer->GetTeamType() != DamageInstigator->GetTeamType());
}

bool APBMode_TeamDeathMatch::MatchIsFinalMatch()
{
	return true;
}
