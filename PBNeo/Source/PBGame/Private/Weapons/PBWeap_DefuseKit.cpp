// Fill out your copyright notice in the Description page of Project Settings.

#include "PBGame.h"
#include "PBWeap_DefuseKit.h"
#include "PBMission_BombMissionDefense.h"
#include "PBProj_Bomb.h"

APBWeap_DefuseKit::APBWeap_DefuseKit()
{
}

void APBWeap_DefuseKit::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(APBWeap_DefuseKit, BombToDefuse);
}

void APBWeap_DefuseKit::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	// Force the weapon to have infinite clip
	WeaponConfig.bInfiniteClip = true;
}

void APBWeap_DefuseKit::SetTargetBomb(APBProj_Bomb* Target)
{
	if (BombToDefuse != Target)
	{
		BombToDefuse = Target;
		OnRep_BombToDefuse();
	}
}

void APBWeap_DefuseKit::OnRep_BombToDefuse()
{
	if (MyPawn && MyPawn->IsLocallyControlled())
	{
		if (BombToDefuse != nullptr)
		{
			APBPlayerController* MyPC = Cast<APBPlayerController>(MyPawn->Controller);
			if (MyPC)
			{
				MyPC->WidgetEventDispatcher->OnCanDefuseBomb.Broadcast(true);
			}
		}

		if (BombToDefuse == nullptr)
		{
			APBPlayerController* MyPC = Cast<APBPlayerController>(MyPawn->Controller);
			if (MyPC)
			{
				MyPC->WidgetEventDispatcher->OnCanDefuseBomb.Broadcast(false);
			}
		}
	}
}

APBProj_Bomb* APBWeap_DefuseKit::GetTargetBomb()
{
	if (BombToDefuse.IsValid())
	{
		return BombToDefuse.Get();
	}

	return nullptr;
}

bool APBWeap_DefuseKit::CanDefuseBomb()
{
	if (BombToDefuse.IsValid())
	{
		APBPlayerController* const PlayerController = Instigator ? Cast<APBPlayerController>(Instigator->Controller) : NULL;
		if (PlayerController)
		{
			FVector CamLoc; FRotator CamRot;
			PlayerController->GetPlayerViewPoint(CamLoc, CamRot);
			FVector PlayerEyeToBombVector = BombToDefuse->GetActorLocation() - CamLoc;

			const float MinimumBombDefuseDistance = 250;
			const float MinimumBombDefusionAngleRad = (PI * 0.2);

			if (PlayerEyeToBombVector.SizeSquared() < FMath::Square(MinimumBombDefuseDistance)
				&&
				FVector::DotProduct(PlayerEyeToBombVector.GetSafeNormal(), CamRot.Vector().GetSafeNormal()) >= FMath::Cos(MinimumBombDefusionAngleRad))
			{
				return true;
			}
		}
	}

	return false;
}

void APBWeap_DefuseKit::FireWeapon(EWeaponPoint ePoint)
{
	FVector ShootDir = GetAdjustedAim();
	FVector Origin = GetMuzzleLocation();

	// trace from camera to check what's under crosshair
	const float ProjectileAdjustRange = 10000.0f;
	const FVector StartTrace = GetStartFireLoc(ShootDir);
	const FVector EndTrace = StartTrace + ShootDir * ProjectileAdjustRange;
	FHitResult Impact = WeaponTrace(StartTrace, EndTrace);

	// and adjust directions to hit that actor
	if (Impact.bBlockingHit)
	{
		const FVector AdjustedDir = (Impact.ImpactPoint - Origin).GetSafeNormal();
		bool bWeaponPenetration = false;

		const float DirectionDot = FVector::DotProduct(AdjustedDir, ShootDir);
		if (DirectionDot < 0.0f)
		{
			// shooting backwards = weapon is penetrating
			bWeaponPenetration = true;
		}
		else if (DirectionDot < 0.5f)
		{
			// check for weapon penetration if angle difference is big enough
			// raycast along weapon mesh to check if there's blocking hit

			FVector MuzzleStartTrace = Origin - GetMuzzleDirection() * 150.0f;
			FVector MuzzleEndTrace = Origin;
			FHitResult MuzzleImpact = WeaponTrace(MuzzleStartTrace, MuzzleEndTrace);

			if (MuzzleImpact.bBlockingHit)
			{
				bWeaponPenetration = true;
			}
		}

		if (bWeaponPenetration)
		{
			// spawn at crosshair position
			Origin = Impact.ImpactPoint - ShootDir * 10.0f;
		}
		else
		{
			// adjust direction to hit
			ShootDir = AdjustedDir;
		}
	}

	ServerFireWeapon(ePoint, Origin, ShootDir);
}

bool APBWeap_DefuseKit::ServerFireWeapon_Validate(EWeaponPoint ePoint, FVector Origin, FVector_NetQuantizeNormal ShootDir)
{
	return true;
}

void APBWeap_DefuseKit::ServerFireWeapon_Implementation(EWeaponPoint ePoint, FVector Origin, FVector_NetQuantizeNormal ShootDir)
{
	if (GetWorld())
	{
		APBGameState* GS = Cast<APBGameState>(GetWorld()->GetGameState());
		APBGameMode* GM = Cast<APBGameMode>(GetWorld()->GetAuthGameMode());
		if (GM && GS)
		{
			// Find the defense mission in this game
			APBMission_BombMissionDefense* DefenseMission = nullptr;
			for (uint8 i = 0; i < (uint8)EPBTeamType::Max; ++i)
			{
				DefenseMission = Cast<APBMission_BombMissionDefense>(GS->GetTeamMission((EPBTeamType)i));
				if (DefenseMission)
					break;
			}

			if (DefenseMission && DefenseMission->CanDefuseBomb())
			{
				if (BombToDefuse.IsValid())
				{
					BombToDefuse->Defuse();
					ClientConfirmBombDefused();
				}
			}
		}
	}
}

void APBWeap_DefuseKit::ClientConfirmBombDefused_Implementation()
{
	if (GetOwnerPawn() && GetOwnerPawn()->IsLocallyControlled())
	{
		APBPlayerController* PC = Cast<APBPlayerController>(GetOwnerPawn()->Controller);
		if (PC)
		{
			PC->WidgetEventDispatcher->OnBombDefusingDone.Broadcast();
		}
	}
}