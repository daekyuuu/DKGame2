// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "PBGame.h"
#include "Weapons/PBWeap_Instant.h"
#include "PBCharacter.h"
#include "Particles/ParticleSystemComponent.h"
#include "Effects/PBImpactEffect.h"
#include "Weapons/Bullet/PBInst_Bullet.h"
#include "Weapons/Attachment/PBWeapAttachment.h"
#include "PBGameUserSettings.h"
#include "PBCheatManager.h"

APBWeap_Instant::APBWeap_Instant(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

void APBWeap_Instant::OnFireFinished()
{
	// stop firing FX on remote clients
	FiringSync.BurstCounter = 0;

	// stop firing FX locally, unless it's a dedicated server
	if (GetNetMode() != NM_DedicatedServer)
	{
		StopWeaponFireEffects();
	}

	GetWorldTimerManager().ClearTimer(TimerHandle_ReFiring);
	//GetWorldTimerManager().ClearTimer(TimerHandle_PostFiringEnd);
	bRefiring = false;
	bPendingFiring = false;
	bPostFiring = false;
}

const FInstantWeaponData& APBWeap_Instant::GetWeapInstData() const
{
	return InstantConfig;
}

//////////////////////////////////////////////////////////////////////////
// Weapon usage

void APBWeap_Instant::CreateBullets()
{
	Super::CreateBullets();

	if (WeapAttachment)
	{
		for (int32 i = 0; i < WeapAttachment->GetAttachmentDataCount(); ++i)
		{
			WeapAttachment->SpawnBullet(EWeaponPoint(i), FTransform::Identity);
		}
	}
}

APBInst_Bullet* APBWeap_Instant::GetBullet() const
{
	if (WeapAttachment)
	{
		return Cast<APBInst_Bullet>(WeapAttachment->GetBullet(FiringSync.FirePoint));
	}
	return nullptr;
}

void APBWeap_Instant::FireWeapon(EWeaponPoint ePoint)
{
	//UPBGameUserSettings* Settings = Cast<UPBGameUserSettings>(GEngine->GetGameUserSettings());
	bool UsePierce = bUsePierceWeapon; //Settings->GetUsePierceWeapon();
	
	const int32 RandomSeed = FMath::Rand();
	FRandomStream WeaponRandomStream(RandomSeed);
	const float CurrentSpread = GetShootingDeviation();
	const float ConeHalfAngle = FMath::DegreesToRadians(CurrentSpread);

	const FVector AimDir = GetAdjustedAim();
	const FVector StartTrace = GetStartFireLoc(AimDir);
	const FVector ShootDir = WeaponRandomStream.VRandCone(AimDir, ConeHalfAngle, ConeHalfAngle);

	APBInst_Bullet* Bullet = GetBullet();
	if (Bullet)
	{
		// ���� ���� ���
		if (UsePierce)
		{
			// ���� ������Ʈ Array (������Ʈ�� �ո�, �޸� Hit ������ �������)
			TArray<FPierceHitResult> PierceImpacts;

			// ���� Trace Hit ���� 
			TArray<FHitResult> Impacts;

			// ������ Trace Hit ���� (����� ������Ʈ �޸� Hit ��ġ üũ�� ����)
			TArray<FHitResult> ReverseImpacts;
			int32 ReverseImpactsLastIndex = 0;

			// ���� Trace
			Bullet->TraceHits(this, StartTrace, ShootDir, EAttackDirType::FORWARD, Impacts);

			if (Impacts.Num() > 0)
			{
				int32 ImpactsLastIndex = Impacts.Num() - 1;
				FVector ReverseStartTrace;
				FVector ReverseShootDir = -ShootDir;

				// ������ Trace �������� ����
				// ���ŷ ���η� ��ŸƮ ������ �����Ѵ�.
				// ���ŷ �� ��� ���ŷ �������� ������ Trace �� ���.
				// ���ŷ ���� ���� ��� ������ Trace ������������ �Ѿ� ��Ÿ���ŭ ������ ��ġ���� ������ Trace�� ���.
				if (Impacts[ImpactsLastIndex].bBlockingHit)
				{
					ReverseStartTrace = Impacts[ImpactsLastIndex].ImpactPoint;
				}
				else
				{
					ReverseStartTrace = (StartTrace + (ShootDir * Bullet->BulletData.BulletRange));
				}

				// ��Ʈ ����� 1�̸� ���ŷ ���� ���� ���, ��Ʈ ����� 1�� ���� ��� ���� ó��
				if ((Impacts.Num() == 1 && !Impacts[ImpactsLastIndex].bBlockingHit) || Impacts.Num() > 1)
				{
					Bullet->TraceHits(this, ReverseStartTrace, ReverseShootDir, EAttackDirType::FORWARD, ReverseImpacts);
					ReverseImpactsLastIndex = ReverseImpacts.Num() - 1;
				}

				// Hit Object Data �Է�
				// ���� ���� Trace ��Ʈ ������ ��´�.
				for (int32 i = 0; i < Impacts.Num(); ++i)
				{
					FPierceHitResult PierceImpact;
					PierceImpact.Impact = Impacts[i];
					PierceImpacts.Add(PierceImpact);
				}

				// ����� ������Ʈ �޸� ���� �Է�
				int32 PierceImpactsLastIndex = PierceImpacts.Num() - 1;
				int32 ReverseImpactsCount = 0;
				for (int32 i = PierceImpactsLastIndex; i >= 0; --i)
				{
					if (i == PierceImpactsLastIndex && PierceImpactsLastIndex > 0)
					{
						continue;
					}

					// �⺻������ ������ Trace ��Ʈ ������ 1�� �̻��� �־�� �Ѵ�.
					if (ReverseImpacts.Num() > 0)
					{
						// ������� �������� Actor ������ ��ġ���� �˻�
						if (PierceImpacts[i].Impact.GetActor() == ReverseImpacts[ReverseImpactsCount].GetActor())
						{
							PierceImpacts[i].ImpactPierce = ReverseImpacts[ReverseImpactsCount];
							++ReverseImpactsCount;
						}
					}
				}
			}

			APBCharacter* OwnerCharacter = Cast<APBCharacter>(Instigator);

			// ���� �����. ���� ������ ���İ� �� ���� ���ҵ� ������� �Էµȴ�.
			float PiercingPower = GetPiercingPower();

			// Hit Process
			for (int32 i = 0; i < PierceImpacts.Num(); ++i)
			{
				// ù ��Ʈ �÷��� ����
				if (i == 0)
				{
					PierceImpacts[i].IsFirstHit = true;
				}
				
				// ������� 0 �����̸�, ù ��Ʈ ����� �ƴ� ��� ���� ����Ʈ���� ���ܽ�Ų��.
				if (!PierceImpacts[i].IsFirstHit && PiercingPower <= 0.0f)
				{
					PierceImpacts.RemoveAt(i);
					--i;
					continue;
				}

				// ���� ������Ʈ ó���� �ѹ��� �����ϵ��� �Ѵ�.
				if (i > 0)
				{
					if (PierceImpacts[i].Impact.GetActor() == PierceImpacts[i - 1].Impact.GetActor())
						continue;
				}

				// ĳ������ ��� ���� ������ üũ
				// ���� ���� ��� Hit ����Ʈ���� ����
				APBCharacter* TargetCharacter = Cast<APBCharacter>(PierceImpacts[i].Impact.GetActor());
				if (TargetCharacter != NULL)
				{
					if (OwnerCharacter->GetTeamType() == TargetCharacter->GetTeamType())
					{
						PierceImpacts.RemoveAt(i);
						--i;
						continue;
					}
				}

				PiercingPower = ProcessInstantHit(PierceImpacts[i], StartTrace, ShootDir, CurrentSpread, PiercingPower);
				PierceImpacts[i].ResultPiercingPower = PiercingPower;
			}

			Bullet->PlayPierceFXClientRemote(PierceImpacts, StartTrace, ShootDir);

			if (UPBCheatManager::DebugPierceToggle)
			{
				// Pierce Debug Line Draw
				UWorld* ThisWorld = GEngine->GetWorldFromContextObject(this);
				FVector LineEnd = StartTrace + (ShootDir * Bullet->BulletData.BulletRange);

				if (PierceImpacts.Num() > 0)
				{
					// Log End Line
					GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::White, TEXT("----------------------------------------------------------------------------------"));

					int32 LastIndex = PierceImpacts.Num() - 1;
					if (PierceImpacts[LastIndex].Impact.bBlockingHit || PierceImpacts[LastIndex].ResultPiercingPower <= 0.0f)
					{
						LineEnd = PierceImpacts[LastIndex].Impact.ImpactPoint;
					}
				}
				DrawDebugLine(ThisWorld, StartTrace, LineEnd, FColor::Red, false, 10.0f, 0, 0.5f);
			}
		}

		// ���� ���� �̻��
		else
		{
			FHitResult Impact;
			Bullet->TraceHit(this, StartTrace, ShootDir, EAttackDirType::FORWARD, Impact);
			ProcessInstantHit(Impact, StartTrace, ShootDir, CurrentSpread);
		}
	}	
}

bool APBWeap_Instant::ServerNotifyHit_Validate(const FHitResult& Impact, FVector_NetQuantizeNormal ShootDir, float ReticleSpread)
{
	return true;
}

void APBWeap_Instant::ServerNotifyHit_Implementation(const FHitResult& Impact, FVector_NetQuantizeNormal ShootDir, float ReticleSpread)
{
	const float WeaponAngleDot = FMath::Abs(FMath::Sin(ReticleSpread * PI / 180.f));

	// if we have an instigator, calculate dot between the view and the shot
	if (Instigator && (Impact.GetActor() || Impact.bBlockingHit))
	{
		const FVector Origin = GetMuzzleLocation();
		const FVector ViewDir = (Impact.Location - Origin).GetSafeNormal();

		// is the angle between the hit and the view within allowed limits (limit + weapon max angle)
		const float ViewDotHitDir = FVector::DotProduct(Instigator->GetViewRotation().Vector(), ViewDir);
		if (ViewDotHitDir > InstantConfig.AllowedViewDotHitDir - WeaponAngleDot)
		{
			if (CurrentState != EWeaponState::Idle)
			{
				if (Impact.GetActor() == NULL)
				{
					if (Impact.bBlockingHit)
					{
						ProcessInstantHit_Confirmed(Impact, Origin, ShootDir);
					}
				}
				// assume it told the truth about static things because the don't move and the hit 
				// usually doesn't have significant gameplay implications
				else if (Impact.GetActor()->IsRootComponentStatic() || Impact.GetActor()->IsRootComponentStationary())
				{
					ProcessInstantHit_Confirmed(Impact, Origin, ShootDir);
				}
				else
				{
					// Get the component bounding box
					const FBox HitBox = Impact.GetActor()->GetComponentsBoundingBox();

					// calculate the box extent, and increase by a leeway
					FVector BoxExtent = 0.5 * (HitBox.Max - HitBox.Min);
					BoxExtent *= InstantConfig.ClientSideHitLeeway;

					// avoid precision errors with really thin objects
					BoxExtent.X = FMath::Max(20.0f, BoxExtent.X);
					BoxExtent.Y = FMath::Max(20.0f, BoxExtent.Y);
					BoxExtent.Z = FMath::Max(20.0f, BoxExtent.Z);

					// Get the box center
					const FVector BoxCenter = (HitBox.Min + HitBox.Max) * 0.5;

					// if we are within client tolerance
					if (FMath::Abs(Impact.Location.Z - BoxCenter.Z) < BoxExtent.Z &&
						FMath::Abs(Impact.Location.X - BoxCenter.X) < BoxExtent.X &&
						FMath::Abs(Impact.Location.Y - BoxCenter.Y) < BoxExtent.Y)
					{
						ProcessInstantHit_Confirmed(Impact, Origin, ShootDir);
					}
					else
					{
						UE_LOG(LogPBWeapon, Log, TEXT("%s Rejected client side hit of %s (outside bounding box tolerance)"), *GetNameSafe(this), *GetNameSafe(Impact.GetActor()));
					}
				}
			}
		}
		else if (ViewDotHitDir <= InstantConfig.AllowedViewDotHitDir)
		{
			UE_LOG(LogPBWeapon, Log, TEXT("%s Rejected client side hit of %s (facing too far from the hit direction)"), *GetNameSafe(this), *GetNameSafe(Impact.GetActor()));
		}
		else
		{
			UE_LOG(LogPBWeapon, Log, TEXT("%s Rejected client side hit of %s"), *GetNameSafe(this), *GetNameSafe(Impact.GetActor()));
		}
	}
}

bool APBWeap_Instant::ServerNotifyPierceHit_Validate(const FPierceHitResult& PierceImpact, FVector_NetQuantizeNormal ShootDir, float ReticleSpread, float PiercingPower)
{
	return true;
}

void APBWeap_Instant::ServerNotifyPierceHit_Implementation(const FPierceHitResult& PierceImpact, FVector_NetQuantizeNormal ShootDir, float ReticleSpread, float PiercingPower)
{
	const float WeaponAngleDot = FMath::Abs(FMath::Sin(ReticleSpread * PI / 180.f));

	// if we have an instigator, calculate dot between the view and the shot
	if (Instigator && (PierceImpact.Impact.GetActor() || PierceImpact.Impact.bBlockingHit))
	{
		const FVector Origin = GetMuzzleLocation();
		const FVector ViewDir = (PierceImpact.Impact.Location - Origin).GetSafeNormal();

		// is the angle between the hit and the view within allowed limits (limit + weapon max angle)
		const float ViewDotHitDir = FVector::DotProduct(Instigator->GetViewRotation().Vector(), ViewDir);
		if (ViewDotHitDir > InstantConfig.AllowedViewDotHitDir - WeaponAngleDot)
		{
			if (CurrentState != EWeaponState::Idle)
			{
				if (PierceImpact.Impact.GetActor() == NULL)
				{
					if (PierceImpact.Impact.bBlockingHit)
					{
						ProcessInstantHit_Confirmed(PierceImpact, Origin, ShootDir, PiercingPower);
					}
				}
				// assume it told the truth about static things because the don't move and the hit 
				// usually doesn't have significant gameplay implications
				else if (PierceImpact.Impact.GetActor()->IsRootComponentStatic() || PierceImpact.Impact.GetActor()->IsRootComponentStationary())
				{
					ProcessInstantHit_Confirmed(PierceImpact, Origin, ShootDir, PiercingPower);
				}
				else
				{
					// Get the component bounding box
					const FBox HitBox = PierceImpact.Impact.GetActor()->GetComponentsBoundingBox();

					// calculate the box extent, and increase by a leeway
					FVector BoxExtent = 0.5 * (HitBox.Max - HitBox.Min);
					BoxExtent *= InstantConfig.ClientSideHitLeeway;

					// avoid precision errors with really thin objects
					BoxExtent.X = FMath::Max(20.0f, BoxExtent.X);
					BoxExtent.Y = FMath::Max(20.0f, BoxExtent.Y);
					BoxExtent.Z = FMath::Max(20.0f, BoxExtent.Z);

					// Get the box center
					const FVector BoxCenter = (HitBox.Min + HitBox.Max) * 0.5;

					// if we are within client tolerance
					if (FMath::Abs(PierceImpact.Impact.Location.Z - BoxCenter.Z) < BoxExtent.Z &&
						FMath::Abs(PierceImpact.Impact.Location.X - BoxCenter.X) < BoxExtent.X &&
						FMath::Abs(PierceImpact.Impact.Location.Y - BoxCenter.Y) < BoxExtent.Y)
					{
						ProcessInstantHit_Confirmed(PierceImpact, Origin, ShootDir, PiercingPower);
					}
					else
					{
						UE_LOG(LogPBWeapon, Log, TEXT("%s Rejected client side hit of %s (outside bounding box tolerance)"), *GetNameSafe(this), *GetNameSafe(PierceImpact.Impact.GetActor()));
					}
				}
			}
		}
		else if (ViewDotHitDir <= InstantConfig.AllowedViewDotHitDir)
		{
			UE_LOG(LogPBWeapon, Log, TEXT("%s Rejected client side hit of %s (facing too far from the hit direction)"), *GetNameSafe(this), *GetNameSafe(PierceImpact.Impact.GetActor()));
		}
		else
		{
			UE_LOG(LogPBWeapon, Log, TEXT("%s Rejected client side hit of %s"), *GetNameSafe(this), *GetNameSafe(PierceImpact.Impact.GetActor()));
		}
	}
}

bool APBWeap_Instant::ServerNotifyMiss_Validate(FVector_NetQuantizeNormal ShootDir)
{
	return true;
}

void APBWeap_Instant::ServerNotifyMiss_Implementation(FVector_NetQuantizeNormal ShootDir)
{
	APBInst_Bullet* Bullet = GetBullet();
	if (Bullet)
	{
		const FVector Origin = GetMuzzleLocation();
		Bullet->PointHit_Miss(Origin, ShootDir);
	}
}

void APBWeap_Instant::ProcessInstantHit(const FHitResult& Impact, const FVector& Origin, const FVector& ShootDir, float ReticleSpread)
{
	if (MyPawn && MyPawn->IsLocallyControlled() && GetNetMode() == NM_Client)
	{
		// if we're a client and we've hit something that is being controlled by the server
		if (Impact.GetActor() && Impact.GetActor()->GetRemoteRole() == ROLE_Authority)
		{
			// notify the server of the hit
			ServerNotifyHit(Impact, ShootDir, ReticleSpread);
		}
		else if (Impact.GetActor() == NULL)
		{
			if (Impact.bBlockingHit)
			{
				// notify the server of the hit
				ServerNotifyHit(Impact, ShootDir, ReticleSpread);
			}
			else
			{
				// notify server of the miss
				ServerNotifyMiss(ShootDir);
			}
		}
	}

	// process a confirmed hit
	ProcessInstantHit_Confirmed(Impact, Origin, ShootDir);
}

float APBWeap_Instant::ProcessInstantHit(const FPierceHitResult& PierceImpact, const FVector& Origin, const FVector& ShootDir, float ReticleSpread, float PiercingPower)
{
	if (MyPawn && MyPawn->IsLocallyControlled() && GetNetMode() == NM_Client)
	{
		// if we're a client and we've hit something that is being controlled by the server
		if (PierceImpact.Impact.GetActor() && PierceImpact.Impact.GetActor()->GetRemoteRole() == ROLE_Authority)
		{
			// notify the server of the hit
			ServerNotifyPierceHit(PierceImpact, ShootDir, ReticleSpread, PiercingPower);
		}
		else if (PierceImpact.Impact.GetActor() == NULL)
		{
			if (PierceImpact.Impact.Actor != NULL)
			{
				// notify the server of the hit
				ServerNotifyPierceHit(PierceImpact, ShootDir, ReticleSpread, PiercingPower);
			}
			else
			{
				// notify server of the miss
				ServerNotifyMiss(ShootDir);
			}
		}
	}

	// process a confirmed hit
	float FinalPiercingPower = ProcessInstantHit_Confirmed(PierceImpact, Origin, ShootDir, PiercingPower);

	return FinalPiercingPower;
}

void APBWeap_Instant::ProcessInstantHit_Confirmed(const FHitResult& Impact, const FVector& Origin, const FVector& ShootDir)
{
	// deal damage, play effect
	APBInst_Bullet* Bullet = GetBullet();
	if (Bullet)
	{
		Bullet->PointHit_Confirm(Impact, Origin, ShootDir);
	}
}

float APBWeap_Instant::ProcessInstantHit_Confirmed(const FPierceHitResult& PierceImpact, const FVector& Origin, const FVector& ShootDir, float PiercingPower)
{
	float FinalPiercingPower = PiercingPower;

	// deal damage, play effect
	APBInst_Bullet* Bullet = GetBullet();
	if (Bullet)
	{
		FinalPiercingPower = Bullet->PointHit_Confirm(PierceImpact, Origin, ShootDir, PiercingPower);
	}

	return FinalPiercingPower;
}
