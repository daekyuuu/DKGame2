// Fill out your copyright notice in the Description page of Project Settings.

#include "PBGame.h"
#include "PBMode_BombMission.h"
#include "Player/PBPlayerState.h"
#include "Mode/SpawnPoint/PBPlayerStart.h"
#include "PBMission_BombMissionAttack.h"
#include "PBMission_BombMissionDefense.h"
#include "PBGameState.h"

APBMode_BombMission::APBMode_BombMission()
{
	PlayerKillScore = 1;
	PlayerDeathScore = 0;
	TargetTeamScoreBombMissionOnly_DEPRECATED = 5;
}

void APBMode_BombMission::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);

	APBGameState* GS = Cast<APBGameState>(GameState);
	if (GS)
	{
		GS->SetGameModeType(EPBGameModeType::BOMB);
	}
}

UClass* APBMode_BombMission::GetDefaultPawnClassForController_Implementation(AController* InController)
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

EPBTeamType APBMode_BombMission::ChooseTeam(APBPlayerState* ForPlayerState) const
{
	// �÷��̾ ������ �ο��� ���� ������ �����Ѵ�.

	TArray<int32> TeamPlayerNums;
	TeamPlayerNums.AddZeroed((int)EPBTeamType::Max);

	// get current team player's count   ������ �ο����� ���Ѵ�
	for (int32 i = 0; i < GameState->PlayerArray.Num(); i++)
	{
		APBPlayerState const* const TestPlayerState = Cast<APBPlayerState>(GameState->PlayerArray[i]);
		int32 TeamIndex = (int32)TestPlayerState->GetTeamType();

		if (TestPlayerState && TestPlayerState != ForPlayerState && TeamPlayerNums.IsValidIndex(TeamIndex))
		{
			TeamPlayerNums[TeamIndex]++;
		}
	}

	// find least populated one,		���� ���� �ο����� ã�´�.
	int32 LeastPlayerNum = TeamPlayerNums[0];
	for (int32 i = 1; i < TeamPlayerNums.Num(); i++)
	{
		if (LeastPlayerNum > TeamPlayerNums[i])
		{
			LeastPlayerNum = TeamPlayerNums[i];
		}
	}

	// there could be more than one...,	 �� �ο��� ��ŭ ������ ������ ã��
	TArray<int32> BestTeams;
	for (int32 i = 0; i < TeamPlayerNums.Num(); i++)
	{
		if (TeamPlayerNums[i] == LeastPlayerNum)
		{
			BestTeams.Add(i);
		}
	}

	// get random from best list	,	�ο��� ���� ���� �����߿��� �� ���� �����Ѵ�.
	const int32 RandomBestTeam = BestTeams[FMath::RandHelper(BestTeams.Num())];
	return (EPBTeamType)RandomBestTeam;
}

int32 APBMode_BombMission::GetTargetTeamScore(const FString& Options)
{
	return TargetTeamScoreBombMissionOnly_DEPRECATED;
}

class APBMission* APBMode_BombMission::CreateMissionForTeam(EPBTeamType Team)
{
	UWorld* World = GetWorld();
	if (World && GameState)
	{
		UClass* MissionClassToUse;
		if (Team == EPBTeamType::Alpha)
		{
			MissionClassToUse = APBMission_BombMissionAttack::StaticClass();
		}
		else
		{
			MissionClassToUse = APBMission_BombMissionDefense::StaticClass();
		}

		APBMission* NewMission = World->SpawnActorDeferred<APBMission>(MissionClassToUse, FTransform::Identity, GameState);
		NewMission->Init(Team);
		UPBGameplayStatics::FinishSpawningActor(NewMission, FTransform::Identity);
		return NewMission;
	}

	return nullptr;
}

void APBMode_BombMission::PreConcludeMatch()
{
	Super::PreConcludeMatch();
}

bool APBMode_BombMission::IsSpawnpointAllowed(APlayerStart* SpawnPoint, AController* Player)
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

bool APBMode_BombMission::CanDealDamage(APBPlayerState* DamageInstigator, class APBPlayerState* DamagedPlayer) const
{
	return DamageInstigator && DamagedPlayer && (DamagedPlayer == DamageInstigator || DamagedPlayer->GetTeamType() != DamageInstigator->GetTeamType());
}

void APBMode_BombMission::AddSpecifyWeaponsTo(APBCharacter* Pawn)
{
	Pawn->AddWeaponByModeSetClass(BombMissionWeaponSlot, BombWeaponClass, BombMissionBombSlotIdx);
	Pawn->AddWeaponByModeSetClass(BombMissionWeaponSlot, DefuseKitClass, BombMissionDefuseKitSlotIdx);
}

bool APBMode_BombMission::MatchIsFinalMatch()
{
	if (GetGameState<APBGameState>())
	{
		float AlphaScore = GetGameState<APBGameState>()->GetTeamScore(EPBTeamType::Alpha);
		float BravoScore = GetGameState<APBGameState>()->GetTeamScore(EPBTeamType::Bravo);

		return
			(AlphaScore >= GetGameState<APBGameState>()->GetTeamTargetScore()
			|| BravoScore >= GetGameState<APBGameState>()->GetTeamTargetScore());
	}

	return true;
}


