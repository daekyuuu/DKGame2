// Fill out your copyright notice in the Description page of Project Settings.

#include "PBGame.h"
#include "FPBKillMarkConditionChecker.h"
#include "PBPlayerState.h"
#include "PBWeapon.h"
#include "FPBKillMarkProperties.h"

bool FPBKillMarkConditionType::SpecialGunner::CheckInternal(class APBPlayerState* KillerPlayerState, class APBPlayerState* VictimPlayerState, AActor *DamageCauser, const FTakeDamageInfo &InTakeDamageInfo)
{
	EWeaponType KillerWeapType = FPBKillMarkConditionChecker::GetWeaponTypeFromDamagCauser(DamageCauser);
	return KillerWeapType == EWeaponType::PS;
}

bool FPBKillMarkConditionType::Assassin::CheckInternal(class APBPlayerState* KillerPlayerState, class APBPlayerState* VictimPlayerState, AActor *DamageCauser, const FTakeDamageInfo &InTakeDamageInfo)
{
	EWeaponType KillerWeapType = FPBKillMarkConditionChecker::GetWeaponTypeFromDamagCauser(DamageCauser);
	if (KillerWeapType == EWeaponType::KNIFE)
	{
		auto victim = VictimPlayerState->GetOwnerCharacter();
		auto killer = KillerPlayerState->GetOwnerCharacter();

		if (victim && killer)
		{
			FVector VictimForwardDir = victim->GetActorForwardVector();
			VictimForwardDir.Normalize();

			FVector KillerToVictimDir = victim->GetActorLocation() - killer->GetActorLocation();
			KillerToVictimDir.Normalize();

			bool IsBackStab = (FVector::DotProduct(VictimForwardDir, KillerToVictimDir) > 0);
			if (IsBackStab)
			{
				return true;
			}
		}
	}

	return false;
}

bool FPBKillMarkConditionType::BombShot::CheckInternal(class APBPlayerState* KillerPlayerState, class APBPlayerState* VictimPlayerState, AActor *DamageCauser, const FTakeDamageInfo &InTakeDamageInfo)
{
	EWeaponType KillerWeapType = FPBKillMarkConditionChecker::GetWeaponTypeFromDamagCauser(DamageCauser);
	return APBWeapon::IsExplosive(KillerWeapType);
}

bool FPBKillMarkConditionType::OneShotOneKill::CheckInternal(class APBPlayerState* KillerPlayerState, class APBPlayerState* VictimPlayerState, AActor *DamageCauser, const FTakeDamageInfo &InTakeDamageInfo)
{
	if (KillerPlayerState->GetKillMarkProperties().FiringCnt <= 1)
	{
		if (false == KillerPlayerState->GetKillMarkProperties().bAchieveOneShotOnekill)
		{
			EWeaponType KillerWeapType = FPBKillMarkConditionChecker::GetWeaponTypeFromDamagCauser(DamageCauser);
			if (APBWeapon::IsGun(KillerWeapType))
			{
				return true;
			}
		}

	}
	return false;
}

bool FPBKillMarkConditionType::ChainSlugger::CheckInternal(class APBPlayerState* KillerPlayerState, class APBPlayerState* VictimPlayerState, AActor *DamageCauser, const FTakeDamageInfo &InTakeDamageInfo)
{
	EWeaponType KillerWeapType = FPBKillMarkConditionChecker::GetWeaponTypeFromDamagCauser(DamageCauser);
	return (KillerPlayerState->GetKillMarkProperties().NumMeleeKills >= 2) && (KillerWeapType == EWeaponType::KNIFE);
}

bool FPBKillMarkConditionType::PiercingShot::CheckInternal(class APBPlayerState* KillerPlayerState, class APBPlayerState* VictimPlayerState, AActor *DamageCauser, const FTakeDamageInfo &InTakeDamageInfo)
{
	EWeaponType KillerWeapType = FPBKillMarkConditionChecker::GetWeaponTypeFromDamagCauser(DamageCauser);
	return KillerWeapType == EWeaponType::SR;
}

bool FPBKillMarkConditionType::MassKill::CheckInternal(class APBPlayerState* KillerPlayerState, class APBPlayerState* VictimPlayerState, AActor *DamageCauser, const FTakeDamageInfo &InTakeDamageInfo)
{
	EWeaponType KillerWeapType = FPBKillMarkConditionChecker::GetWeaponTypeFromDamagCauser(DamageCauser);

	if (KillerWeapType == EWeaponType::GRENADE ||
		KillerWeapType == EWeaponType::LC ||
		KillerWeapType == EWeaponType::RPG ||
		KillerWeapType == EWeaponType::SG)
	{
		return true;
	}

	return false;
}

bool FPBKillMarkConditionType::ChainStopper::CheckInternal(class APBPlayerState* KillerPlayerState, class APBPlayerState* VictimPlayerState, AActor *DamageCauser, const FTakeDamageInfo &InTakeDamageInfo)
{
	return VictimPlayerState->GetKillMarkProperties().NumSerialKills >= 4;
}

bool FPBKillMarkConditionType::ChainKiller::CheckInternal(class APBPlayerState* KillerPlayerState, class APBPlayerState* VictimPlayerState, AActor *DamageCauser, const FTakeDamageInfo &InTakeDamageInfo)
{
	return KillerPlayerState->GetKillMarkProperties().NumSerialKills >= 4;
}

bool FPBKillMarkConditionType::ChainHeadShot::CheckInternal(class APBPlayerState* KillerPlayerState, class APBPlayerState* VictimPlayerState, AActor *DamageCauser, const FTakeDamageInfo &InTakeDamageInfo)
{
	if (InTakeDamageInfo.bIsHeadShot)
	{
		if (KillerPlayerState->GetKillMarkProperties().NumHeadKills == 2)
		{
			return true;
		}
	}

	return false;
}

bool FPBKillMarkConditionType::HeadShot::CheckInternal(class APBPlayerState* KillerPlayerState, class APBPlayerState* VictimPlayerState, AActor *DamageCauser, const FTakeDamageInfo &InTakeDamageInfo)
{
	return InTakeDamageInfo.bIsHeadShot;
}

bool FPBKillMarkConditionType::HotKiller::CheckInternal(class APBPlayerState* KillerPlayerState, class APBPlayerState* VictimPlayerState, AActor *DamageCauser, const FTakeDamageInfo &InTakeDamageInfo)
{
	if (5 == GetKillGap(KillerPlayerState, VictimPlayerState))
	{
		return true;
	}

	return false;
}

bool FPBKillMarkConditionType::Nightmare::CheckInternal(class APBPlayerState* KillerPlayerState, class APBPlayerState* VictimPlayerState, AActor *DamageCauser, const FTakeDamageInfo &InTakeDamageInfo)
{
	if (6 == GetKillGap(KillerPlayerState, VictimPlayerState))
	{
		return true;
	}

	const FHotKillerData* KillerKilledVictimData = KillerPlayerState->GetKillMarkProperties().GetHotkillerData(VictimPlayerState);
	if (KillerKilledVictimData &&
		2 == KillerKilledVictimData->NumHotKillerGet)
	{
		return true;
	}

	return false;
}

bool FPBKillMarkConditionType::MeatChopper::CheckInternal(class APBPlayerState* KillerPlayerState, class APBPlayerState* VictimPlayerState, AActor *DamageCauser, const FTakeDamageInfo &InTakeDamageInfo)
{
	if (7 == GetKillGap(KillerPlayerState, VictimPlayerState))
	{
		return true;
	}

	const FHotKillerData* KillerKilledVictimData = KillerPlayerState->GetKillMarkProperties().GetHotkillerData(VictimPlayerState);
	if (KillerKilledVictimData &&
		3 == KillerKilledVictimData->NumHotKillerGet)
	{
		return true;
	}


	return false;
}

bool FPBKillMarkConditionType::Payback::CheckInternal(class APBPlayerState* KillerPlayerState, class APBPlayerState* VictimPlayerState, AActor *DamageCauser, const FTakeDamageInfo &InTakeDamageInfo)
{
	const FHotKillerData* VictimKilledKillerData = VictimPlayerState->GetKillMarkProperties().GetHotkillerData(KillerPlayerState);

	if (VictimKilledKillerData)
	{
		if (GetKillGap(VictimPlayerState, KillerPlayerState) >= 5 || VictimKilledKillerData->NumHotKillerGet > 0)
		{

			return true;
		}
	}


	return false;
}

bool FPBKillMarkConditionType::Avenger::CheckInternal(class APBPlayerState* KillerPlayerState, class APBPlayerState* VictimPlayerState, AActor *DamageCauser, const FTakeDamageInfo &InTakeDamageInfo)
{
	const FHotKillerData* KillerKilledVictimData = KillerPlayerState->GetKillMarkProperties().GetHotkillerData(VictimPlayerState);
	if (KillerKilledVictimData)
	{
		if (KillerKilledVictimData->NumSerialPayback >= 2)
		{
			return true;
		}
	}


	return false;
}

bool FPBKillMarkConditionType::Stalker::CheckInternal(class APBPlayerState* KillerPlayerState, class APBPlayerState* VictimPlayerState, AActor *DamageCauser, const FTakeDamageInfo &InTakeDamageInfo)
{
	const FHotKillerData* KillerKilledVictimData = KillerPlayerState->GetKillMarkProperties().GetHotkillerData(VictimPlayerState);
	const FHotKillerData* VictimKilledKillerData = VictimPlayerState->GetKillMarkProperties().GetHotkillerData(KillerPlayerState);

	if (KillerKilledVictimData && VictimKilledKillerData)
	{
		if (VictimKilledKillerData->NumHotKillerGet > 0)
		{
			if ((KillerKilledVictimData->NumKills - VictimKilledKillerData->NumKills) == 0)
			{
				return true;
			}

		}
	}

	return false;
}

bool FPBKillMarkConditionType::LoveShot::CheckInternal(class APBPlayerState* KillerPlayerState, class APBPlayerState* VictimPlayerState, AActor *DamageCauser, const FTakeDamageInfo &InTakeDamageInfo)
{
	auto PlayerWhoKilledKiller = KillerPlayerState->GetKillMarkProperties().PlayerWhoKilledMe;
	if (PlayerWhoKilledKiller)
	{
		if (PlayerWhoKilledKiller == VictimPlayerState)
		{
			return true;
		}
	}

	return false;
}

bool FPBKillMarkConditionType::DoubleKill::CheckInternal(class APBPlayerState* KillerPlayerState, class APBPlayerState* VictimPlayerState, AActor *DamageCauser, const FTakeDamageInfo &InTakeDamageInfo)
{
	return KillerPlayerState->GetKillMarkProperties().NumSerialKills == 2;
}

bool FPBKillMarkConditionType::TripleKill::CheckInternal(class APBPlayerState* KillerPlayerState, class APBPlayerState* VictimPlayerState, AActor *DamageCauser, const FTakeDamageInfo &InTakeDamageInfo)
{
	return KillerPlayerState->GetKillMarkProperties().NumSerialKills == 3;
}

bool FPBKillMarkConditionType::FallOff::CheckInternal(class APBPlayerState* KillerPlayerState, class APBPlayerState* VictimPlayerState, AActor *DamageCauser, const FTakeDamageInfo &InTakeDamageInfo)
{
	return InTakeDamageInfo.bFallOff;
}

TArray<KillMarkType> FPBKillMarkConditionChecker::CheckSingleKill(class APBPlayerState* KillerPlayerState, class APBPlayerState* VictimPlayerState, AActor *DamageCauser, const FTakeDamageInfo &InTakeDamageInfo)
{
	TArray<KillMarkType> checkedTypes;

	if (KillerPlayerState && VictimPlayerState)
	{
		TArray<KillMarkType> ArrayForCheck = 
		{
			FPBKillMarkConditionType::SpecialGunner::GetStaticUID()
			, FPBKillMarkConditionType::Assassin::GetStaticUID()
			, FPBKillMarkConditionType::BombShot::GetStaticUID()
			, FPBKillMarkConditionType::OneShotOneKill::GetStaticUID()
			, FPBKillMarkConditionType::ChainSlugger::GetStaticUID()

			, FPBKillMarkConditionType::ChainStopper::GetStaticUID()
			, FPBKillMarkConditionType::ChainKiller::GetStaticUID()
			, FPBKillMarkConditionType::ChainHeadShot::GetStaticUID()
			, FPBKillMarkConditionType::HeadShot::GetStaticUID()
			, FPBKillMarkConditionType::HotKiller::GetStaticUID()
			, FPBKillMarkConditionType::Nightmare::GetStaticUID()
			, FPBKillMarkConditionType::MeatChopper::GetStaticUID()
			, FPBKillMarkConditionType::Stalker::GetStaticUID()
			, FPBKillMarkConditionType::Avenger::GetStaticUID()
			, FPBKillMarkConditionType::Payback::GetStaticUID()
			, FPBKillMarkConditionType::LoveShot::GetStaticUID()
			, FPBKillMarkConditionType::TripleKill::GetStaticUID()
			, FPBKillMarkConditionType::DoubleKill::GetStaticUID()
		};

		for (KillMarkType CurKillMarkType : ArrayForCheck)
		{
			FKillMarkCondition **FoundKillMarkCondition = FKillMarkCondition::ObjectMap.Find(CurKillMarkType);
			if (nullptr == FoundKillMarkCondition || nullptr == *FoundKillMarkCondition)
				continue;

			if ((*FoundKillMarkCondition)->CheckInternal(KillerPlayerState, VictimPlayerState, DamageCauser, InTakeDamageInfo))
				checkedTypes.AddUnique(CurKillMarkType);
		}
	}

	return checkedTypes;
}

TArray<KillMarkType> FPBKillMarkConditionChecker::CheckKillOwnself(class APBPlayerState* KillerPlayerState, class APBPlayerState* VictimPlayerState, AActor *DamageCauser, const FTakeDamageInfo &InTakeDamageInfo)
{
	TArray<KillMarkType> checkedTypes;

	if (KillerPlayerState && VictimPlayerState
		&& KillerPlayerState == VictimPlayerState)
	{
		TArray<KillMarkType> ArrayForCheck =
		{
			FPBKillMarkConditionType::FallOff::GetStaticUID()
		};

		for (KillMarkType CurKillMarkType : ArrayForCheck)
		{
			FKillMarkCondition **FoundKillMarkCondition = FKillMarkCondition::ObjectMap.Find(CurKillMarkType);
			if (nullptr == FoundKillMarkCondition || nullptr == *FoundKillMarkCondition)
				continue;

			if ((*FoundKillMarkCondition)->CheckInternal(KillerPlayerState, VictimPlayerState, DamageCauser, InTakeDamageInfo))
				checkedTypes.AddUnique(CurKillMarkType);
		}
	}

	return checkedTypes;
}


TArray<KillMarkType> FPBKillMarkConditionChecker::CheckAll(class APBPlayerState* KillerPlayerState, class APBPlayerState* VictimPlayerState, AActor *DamageCauser, const FTakeDamageInfo &InTakeDamageInfo)
{
	TArray<KillMarkType> checkedTypes;

	for (auto Itr : FKillMarkCondition::ObjectArray)
	{
		if (Itr->CheckInternal(KillerPlayerState, VictimPlayerState, DamageCauser, InTakeDamageInfo))
		{
			checkedTypes.Add(Itr->GetUniqueID());
		} 
	}

	return checkedTypes;
}

EWeaponType FPBKillMarkConditionChecker::GetWeaponTypeFromDamagCauser(AActor *DamageCauser)
{
	EWeaponType weapType = EWeaponType::AR;
	if (DamageCauser)
	{
		APBWeapon* weap = Cast<APBWeapon>(DamageCauser);
		if (weap)
		{
			weapType = weap->GetWeaponType();
		}
		else
		{
			// probably DamageCauser is projectile or bullet
			weap = Cast<APBWeapon>(DamageCauser->GetOwner());
			if (weap)
			{
				weapType = weap->GetWeaponType();
			}
		}
	}

	return weapType;
}

int32 FPBKillMarkConditionChecker::GetKillGap(class APBPlayerState* KillerPlayerState, class APBPlayerState* VictimPlayerState)
{
	const FHotKillerData* KillerKilledVictimData = KillerPlayerState->GetKillMarkProperties().GetHotkillerData(VictimPlayerState);
	const FHotKillerData* VictimKilledKillerData = VictimPlayerState->GetKillMarkProperties().GetHotkillerData(KillerPlayerState);

	if (KillerKilledVictimData && VictimKilledKillerData)
	{
		return (KillerKilledVictimData->NumKills - VictimKilledKillerData->NumKills);
	}

	return 0;
}

