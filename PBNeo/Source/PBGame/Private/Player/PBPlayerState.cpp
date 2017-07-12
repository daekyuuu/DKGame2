// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "PBGame.h"
#include "PBPlayerState.h"
#include "Weapons/PBWeapon.h"
#include "Player/PBPlayerController.h"
#include "FPBKillMarkConditionChecker.h"

APBPlayerState::APBPlayerState(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	InitQuitter();
	InitKillDeathScore();
	InitTeamType();

	KillMarkProperties.Init();

	bIsInitiallyReplicated = false;
}

void APBPlayerState::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(APBPlayerState, OwnerCharacter);

	DOREPLIFETIME(APBPlayerState, NumKills);
	DOREPLIFETIME(APBPlayerState, NumDeaths);

	DOREPLIFETIME(APBPlayerState, KillMarkProperties);

	DOREPLIFETIME(APBPlayerState, TeamType);

	DOREPLIFETIME_CONDITION(APBPlayerState, ReplicateEquippedCharItems, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(APBPlayerState, ReplicateEquippedWeapItems, COND_OwnerOnly);

	//DOREPLIFETIME(APBPlayerState, PlayerDebugContext);
}

void APBPlayerState::Reset()
{
	Super::Reset();

	ResetQuitter();
	ResetKillDeathScore();

	KillMarkProperties.ResetAll();
}

void APBPlayerState::CopyProperties(APlayerState* PlayerState)
{
	Super::CopyProperties(PlayerState);

	CopyTeamType(PlayerState);
}

void APBPlayerState::OverrideWith(APlayerState* PlayerState)
{
	Super::OverrideWith(PlayerState);

	// When player leave a game they will leave "inactive player state"
	// When player enter game again this "inactive player state" may be reattached at PostLogin
	// However before PostLogin some PlayerState attribute needs to be decided with the PlayerState the PlayerController is born with
	// So we need to transfer these decided attribute to the "inactive player state" before it is reattached

	CopyTeamTypeFrom(PlayerState);
}

void APBPlayerState::UnregisterPlayerWithSession()
{
	if (!bFromPreviousLevel)
	{
		Super::UnregisterPlayerWithSession();
	}
}

APBCharacter* APBPlayerState::GetOwnerCharacter()
{
	AController* OwnerController = Cast<AController>(GetOwner());
	if (OwnerController)
	{
		return Cast<APBCharacter>(OwnerController->GetCharacter());
	}
	else
	{
		// case of remote player's player state.
		return OwnerCharacter;
	}
}

FString APBPlayerState::GetShortPlayerName() const
{
	if (PlayerName.Len() > MAX_PLAYER_NAME_LENGTH)
	{
		return PlayerName.Left(MAX_PLAYER_NAME_LENGTH) + "...";
	}
	return PlayerName;
}

void APBPlayerState::InitQuitter()
{
	bQuitter = false;
}

void APBPlayerState::ResetQuitter()
{
	bQuitter = false;
}

void APBPlayerState::SetQuitter(bool bInQuitter)
{
	bQuitter = bInQuitter;
}

bool APBPlayerState::IsQuitter() const
{
	return bQuitter;
}

void APBPlayerState::BroadcastDeath_Implementation(class APBPlayerState* KillerPlayerState, const UDamageType* KillerDamageType, class APBPlayerState* KilledPlayerState, const FTakeDamageInfo& TakeDamageInfo)
{
	if (GetWorld())
	{
		for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
		{
			// all local players get death messages so they can update their huds.
			APBPlayerController* TestPC = Cast<APBPlayerController>(*It);
			if (TestPC && TestPC->IsLocalController())
			{
				TestPC->NotifyDeath(KillerPlayerState, this, KillerDamageType, TakeDamageInfo);
			}
		}
	}
}

void APBPlayerState::BroadcastSpawn_Implementation()
{
	if (GetWorld())
	{
		for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
		{
			// all local players get death messages so they can update their huds.
			APBPlayerController* TestPC = Cast<APBPlayerController>(*It);
			if (TestPC && TestPC->IsLocalController())
			{
				TestPC->NotifySpawn(this);
			}
		}
	}
}

void APBPlayerState::ScoreKill(APBPlayerState* Victim, int32 Points)
{
	NumKills++;
	Score += Points;
}

void APBPlayerState::ScoreDeath(APBPlayerState* KilledBy, int32 Points)
{
	bDiedAtLeastOnceInMatch = true;

	NumDeaths++;
	Score += Points;
	Score = FMath::Max(Score, 0.f);
}

bool APBPlayerState::DiedAtLeastOnceInMatch()
{
	return bDiedAtLeastOnceInMatch;
}

int32 APBPlayerState::GetKills() const
{
	return NumKills;
}

int32 APBPlayerState::GetDeaths() const
{
	return NumDeaths;
}

float APBPlayerState::GetScore() const
{
	return Score;
}

int32 APBPlayerState::GetSquadSortValue() const
{
	// Yes. I know. If someone score numerous assist points, It could be higher than kill count 1.
	// but It's not going to happen normally.

	int32 SortingValue = GetKills() * 1000;
	//SortingValue += GetAssist() * 10000;
	SortingValue -= GetDeaths() * 1;

	return SortingValue;
}

void APBPlayerState::InitKillDeathScore()
{
	bDiedAtLeastOnceInMatch = false;

	NumKills = 0;
	NumDeaths = 0;
}

void APBPlayerState::ResetKillDeathScore()
{
	bDiedAtLeastOnceInMatch = false;
}

void APBPlayerState::CopyKillDeathScore(APlayerState* NewPlayerState)
{
	APBPlayerState* PBPlayer = Cast<APBPlayerState>(NewPlayerState);
	if (PBPlayer)
	{
		PBPlayer->NumKills = NumKills;
		PBPlayer->NumDeaths = NumDeaths;
	}
}

FPBKillMarkProperties& APBPlayerState::GetKillMarkProperties()
{
	return KillMarkProperties;
}

void APBPlayerState::UpdateKillMark(AController* Killer, const TArray<AController*>& KilledPlayers, AActor* DamageCauser, const TArray<FTakeDamageInfo>& TakeDamageInfos)
{

	/**
	* Note that: In this function, Regard the Owner Character of this playerState as a killer
	*/

	EWeaponType WeapType = FPBKillMarkConditionChecker::GetWeaponTypeFromDamagCauser(DamageCauser);

	if (KilledPlayers.Num() == 1)
	{
		APBPlayerState* VictimPlayerState = KilledPlayers[0] ? Cast<APBPlayerState>(KilledPlayers[0]->PlayerState) : nullptr;
		auto &TakeDamageInfo = TakeDamageInfos[0];

		if (this != VictimPlayerState)
		{
			/**
			* 1. Update preset values to check kill mark conditions
			*/
			UpdateKillMarkBeforeChecking(VictimPlayerState, DamageCauser, TakeDamageInfo);


			/**
			* 2. Check and get the kill mark IDs, except related with multiple kills
			*/
			//TArray<EKillMarkTypes> CheckedKillMarks = FPBKillMarkConditionChecker::CheckSingleKill(this, VictimPlayerState, GetWeaponTypeFromDamagCauser(DamageCauser), bIsHeadShot);
			TArray<KillMarkType> CheckedKillMarks = FPBKillMarkConditionChecker::CheckSingleKill(this, VictimPlayerState, DamageCauser, TakeDamageInfo);

			/**
			* 3. Update kill mark values that must be updated after checking
			*/
			UpdateKillMarkAfterChecking(CheckedKillMarks, VictimPlayerState, DamageCauser, TakeDamageInfo);


			/**
			* 4. Convert TArray<Enum> to TArray<uint8>
			* because RPC function cause an error if I don't use TArray<uint8> instead of TArray<EnumType>.
			*/
			//auto KillMarkIds = ConvertKillMarkTypesToInteger(CheckedKillMarks);


			/**
			* 5. Notify the KillMarEvent to client using client RPC
			*/
			ClientKillMarkEvent(CheckedKillMarks, WeapType);
		}
		else
		{
			/**
			* 1. Update preset values to check kill mark conditions
			*/
			UpdateKillMarkBeforeChecking(VictimPlayerState, DamageCauser, TakeDamageInfo);

			/**
			* 2. Check and get the kill mark IDs, except related with multiple kills
			*/
			TArray<KillMarkType> CheckedKillMarks = FPBKillMarkConditionChecker::CheckKillOwnself(this, VictimPlayerState, DamageCauser, TakeDamageInfo);

			/**
			* 3. Update kill mark values that must be updated after checking
			*/
			UpdateKillMarkAfterChecking(CheckedKillMarks, VictimPlayerState, DamageCauser, TakeDamageInfo);


			/**
			* 4. Convert TArray<Enum> to TArray<uint8>
			* because RPC function cause an error if I don't use TArray<uint8> instead of TArray<EnumType>.
			*/
			//auto KillMarkIds = ConvertKillMarkTypesToInteger(CheckedKillMarks);


			/**
			* 5. Notify the KillMarEvent to client using client RPC
			*/
			ClientKillMarkEvent(CheckedKillMarks, WeapType);
		}

		// Dead character need to reset some properties
		if (VictimPlayerState)
		{
			VictimPlayerState->ResetKillMarkProperties();
		}
	}
	else if (KilledPlayers.Num() > 1)
	{
		TArray<KillMarkType> AccumulatedKillMarks;

		for (int i = 0; i< KilledPlayers.Num(); ++i)
		{
			APBPlayerState* VictimPlayerState = KilledPlayers[i] ? Cast<APBPlayerState>(KilledPlayers[i]->PlayerState) : nullptr;
			//bool bIsHeadShot = TakeDamageInfos[i].bIsHeadShot;
			auto &TakeDamageInfo = TakeDamageInfos[i];
			if (this != VictimPlayerState)
			{
				UpdateKillMarkBeforeChecking(VictimPlayerState, DamageCauser, TakeDamageInfo);

				// check all kill marks and get
				TArray<KillMarkType> CheckedKillMarks = FPBKillMarkConditionChecker::CheckAll(this, VictimPlayerState, DamageCauser, TakeDamageInfo);
				
				// accumulate the kill marks
				for (int j = 0; j < CheckedKillMarks.Num(); ++j)
				{
					AccumulatedKillMarks.Add(CheckedKillMarks[j]);
				}

				UpdateKillMarkAfterChecking(CheckedKillMarks, VictimPlayerState, DamageCauser, TakeDamageInfo);
			}

			if (VictimPlayerState)
			{
				VictimPlayerState->ResetKillMarkProperties();
			}
		}

		// notify kill mark events accumulated to client
		// there's possibility of duplicated kill marks. but it will filtered in UMG widget logic
		ClientKillMarkEvent(AccumulatedKillMarks, WeapType);
	}

}

void APBPlayerState::ResetKillMarkProperties()
{

	/**
	* Note that: Regarding the Owner Character of this playerState is a Victim
	*/

	KillMarkProperties.ResetRespawnClearProperties();
}

void APBPlayerState::UpdateKillMarkBeforeChecking(APBPlayerState* VictimPlayerState, AActor *DamageCauser, const FTakeDamageInfo &TakeDamageInfo)
{	
	EWeaponType WeapType = FPBKillMarkConditionChecker::GetWeaponTypeFromDamagCauser(DamageCauser);
	if (WeapType == EWeaponType::KNIFE)
	{
		KillMarkProperties.NumMeleeKills++;
	}

	KillMarkProperties.NumSerialKills++;

	KillMarkProperties.IncreaseUserKill(VictimPlayerState);

	if (TakeDamageInfo.bIsHeadShot)
	{
		KillMarkProperties.NumHeadKills++;
	}

	VictimPlayerState->GetKillMarkProperties().PlayerWhoKilledMe = this;

	if (FKillMarkCondition::Check<FPBKillMarkConditionType::HotKiller>(this, VictimPlayerState, DamageCauser, TakeDamageInfo))
	{
		KillMarkProperties.IncreaseHotKiller(VictimPlayerState);
	}

	if (FKillMarkCondition::Check<FPBKillMarkConditionType::Payback>(this, VictimPlayerState, DamageCauser, TakeDamageInfo))
	{
		KillMarkProperties.IncreaseSerialPayback(VictimPlayerState);
	}
}

void APBPlayerState::UpdateKillMarkAfterChecking(const TArray<int32>& CheckedKillMarks, APBPlayerState* VictimPlayerState, AActor *DamageCauser, const FTakeDamageInfo &TakeDamageInfo)
{	
	EWeaponType WeapType = FPBKillMarkConditionChecker::GetWeaponTypeFromDamagCauser(DamageCauser);

	if (INDEX_NONE != CheckedKillMarks.Find(FPBKillMarkConditionType::OneShotOneKill::GetStaticUID()))
	{
		KillMarkProperties.bAchieveOneShotOnekill = true;
	}

	if (INDEX_NONE != CheckedKillMarks.Find(FPBKillMarkConditionType::Stalker::GetStaticUID()))
	{
		// if there was stalker, then reset hot killer
		VictimPlayerState->GetKillMarkProperties().ClearHotKillerData(this);
		this->GetKillMarkProperties().ClearHotKillerData(VictimPlayerState);
	}

	// if there's LoveShot, then notify to victim too
	if (INDEX_NONE != CheckedKillMarks.Find(FPBKillMarkConditionType::LoveShot::GetStaticUID()))
	{
		TArray<int32> temp;
		temp.Add((int32)FPBKillMarkConditionType::LoveShot::GetStaticUID());
		VictimPlayerState->ClientKillMarkEvent(temp, WeapType);
	}
}

void APBPlayerState::ClientKillMarkEvent_Implementation(const TArray<int32>& CheckedKillMarks, EWeaponType WeapType)
{
	if (CheckedKillMarks.Num() > 0)
	{
		APBPlayerController* PBController = Cast<APBPlayerController>(GetOwner());
		if (IsSafe(PBController))
		{
			PBController->WidgetEventDispatcher->OnShowKillMark.Broadcast(CheckedKillMarks, WeapType);
		}
	}
}

void APBPlayerState::SetTeamType(EPBTeamType NewTeamType)
{
	TeamType = NewTeamType;
	UpdateTeamColors();
}

EPBTeamType APBPlayerState::GetTeamType() const
{
	return TeamType;
}

void APBPlayerState::InitTeamType()
{
	TeamType = EPBTeamType::Max;
}

void APBPlayerState::CopyTeamType(APlayerState* NewPlayerState)
{
	APBPlayerState* PBPlayer = Cast<APBPlayerState>(NewPlayerState);
	if (PBPlayer)
	{
		PBPlayer->TeamType = TeamType;
	}
}

void APBPlayerState::CopyTeamTypeFrom(APlayerState* OldPlayerState)
{
	APBPlayerState* PBPlayer = Cast<APBPlayerState>(OldPlayerState);
	if (PBPlayer)
	{
		TeamType = PBPlayer->TeamType;
	}
}

void APBPlayerState::OnRep_TeamType()
{
	UpdateTeamColors();
}

void APBPlayerState::UpdateTeamColors()
{
	AController* OwnerController = Cast<AController>(GetOwner());
	if (OwnerController != NULL)
	{
		APBCharacter* PBCharacter = Cast<APBCharacter>(OwnerController->GetCharacter());
		if (PBCharacter != NULL)
		{
			PBCharacter->UpdateTeamColorsAllMIDs();
			return;
		}
	}
}

APBMission* APBPlayerState::GetOwnMission()
{
	if (!GetWorld()) return nullptr;

	APBGameState* GS = Cast<APBGameState>(GetWorld()->GetGameState());
	if (!GS)
	{
		return nullptr;
	}

	return GS->GetTeamMission(GetTeamType());
}

bool APBPlayerState::IsInitiallyReplicated()
{
	return bIsInitiallyReplicated;
}

void APBPlayerState::SetInitiallyReplicated()
{
	bIsInitiallyReplicated = true;
}

bool APBPlayerState::IsPlayerStarted()
{
	return bIsPlayerStarted;
}

void APBPlayerState::SetPlayerStarted()
{
	bIsPlayerStarted = true;
}

bool APBPlayerState::HasCharacterItems()
{
	return ReplicateEquippedCharItems.Num() > 0;
}

bool APBPlayerState::HasWeaponItems()
{
	return ReplicateEquippedWeapItems.Num() > 0;
}
