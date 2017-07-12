// Fill out your copyright notice in the Description page of Project Settings.

#include "PBGame.h"
#include "PBBulletBase.h"
#include "Effects/PBImpactEffect.h"
#include "Player/PBCharacter.h"

APBBulletBase::APBBulletBase()
{
	WeaponId = 0;
}

void APBBulletBase::OnPointHit(const FHitResult & HitResult)
{

}

void APBBulletBase::ApplyImpulse(const FHitResult& HitResult)
{

}

bool APBBulletBase::ApplyTableData(const struct FPBWeaponTableData* Data)
{
	return true;
}

APBCharacter* APBBulletBase::GetOwnerPawn() const
{
	return Cast<APBCharacter>(GetInstigator());
}

bool APBBulletBase::GetViewerIsFirstPerson() const
{
	auto PlayerCharacter = Cast<APBCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));
	auto OwnerCharacter = GetOwnerPawn();
	bool bIsViewingOwnself = false;
	bool bIsFirstPerson = false;

	if (nullptr != PlayerCharacter)
	{
		bIsViewingOwnself = PlayerCharacter->IsViewingOwnself();
		if (false == bIsViewingOwnself)
		{
			bIsFirstPerson = PlayerCharacter->IsFirstPersonView();
		}
	}

	if (nullptr != OwnerCharacter && bIsViewingOwnself)
	{
		bIsFirstPerson = OwnerCharacter->IsFirstPersonView();
	}

	return bIsFirstPerson;
}

bool APBBulletBase::ShouldDealDamage(AActor* TestActor) const
{
	return false;
}

void APBBulletBase::DealDamage(const FHitResult& Impact, const FVector& ShootDir)
{

}


void APBBulletBase::SpawnImpactEffects(const FHitResult& Impact)
{

}
