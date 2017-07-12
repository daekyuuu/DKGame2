// Fill out your copyright notice in the Description page of Project Settings.

#include "PBGame.h"
#include "PBWeap_Bomb.h"
#include "PBMission_BombMissionAttack.h"
#include "PBProjectile.h"
#include "Mode/Trigger/PBTrigger_BombSite.h"

APBWeap_Bomb::APBWeap_Bomb()
{
	WeaponType = EWeaponType::BOMB;
}

void APBWeap_Bomb::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	// Force the weapon to have infinite clip
	WeaponConfig.bInfiniteClip = true;
}

void APBWeap_Bomb::ServerFireProjectile_Implementation(EWeaponPoint ePoint, FVector Origin, FVector_NetQuantizeNormal ShootDir, float InitialSpeedProportion /* = 1.0f */)
{
	if (GetWorld())
	{
		APBGameState* GS = Cast<APBGameState>(GetWorld()->GetGameState());
		APBGameMode* GM = Cast<APBGameMode>(GetWorld()->GetAuthGameMode());
		if (GM && GS)
		{
			// Find the attack mission in this game
			APBMission_BombMissionAttack* AttackMission = nullptr;
			for (uint8 i = 0; i < (uint8)EPBTeamType::Max; ++i)
			{
				AttackMission = Cast<APBMission_BombMissionAttack>(GS->GetTeamMission((EPBTeamType)i));
				if (AttackMission)
					break;
			}

			if (AttackMission && AttackMission->CanPlantBomb())
			{
				// This actually spawns the bomb
				{
					FCollisionQueryParams TraceParams(FName("BombTrace"), false, this);
					TraceParams.AddIgnoredActor(GetOwnerPawn());
					const FVector StartLocation = Origin + FVector(ShootDir.X, ShootDir.Y, 0).GetSafeNormal() * 100;
					const FVector EndLocation = StartLocation + FVector(0,0,-1000);

					FHitResult Hit(ForceInit);
					GetWorld()->LineTraceSingleByChannel(Hit, Origin, EndLocation, COLLISION_WEAPON, TraceParams);

					FVector SpawnLocation = Hit.bBlockingHit ? Hit.Location : EndLocation;
					FRotator SpawnRotation = Hit.bBlockingHit ? FRotationMatrix::MakeFromZX(Hit.ImpactNormal, ShootDir).Rotator() : ShootDir.Rotation();
					FTransform SpawnTM(SpawnRotation, SpawnLocation);

					APBProjectile* Projectile = Cast<APBProjectile>(SpawnBullet(ePoint, SpawnTM));
					if (Projectile)
					{
						auto MC = Projectile->GetMovementComp();
						if (MC)
						{
							MC->InitialSpeed = 0.f;
						}
						UPBGameplayStatics::FinishSpawningActor(Projectile, SpawnTM);

						Projectile->Instigator = Instigator;
						Projectile->OnRep_Instigator();

						//auto ProjBomb = Cast<APBProj_Bomb>(Projectile);
						//if (ProjBomb)
						//{
						//	ProjBomb->SetupSecondsUntilExplosion();
						//}
						//GetOwnerPawn()->ServerSetIsPlantedBomb(true);
					}
				}
				GM->OnGlobalEvent(FName(TEXT("BombPlanted")));
				ClientConfirmBombPlanted();
			}
		}
	}
}

void APBWeap_Bomb::ClientConfirmBombPlanted_Implementation()
{
	if (GetOwnerPawn() && GetOwnerPawn()->IsLocallyControlled())
	{
		APBPlayerController* PC = Cast<APBPlayerController>(GetOwnerPawn()->Controller);
		if (PC)
		{
			PC->WidgetEventDispatcher->OnBombPlantingDone.Broadcast();
		}
	}
}
