// Fill out your copyright notice in the Description page of Project Settings.

#include "PBGame.h"
#include "PBWeapAttach_Dual.h"


UPBWeapAttach_Dual::UPBWeapAttach_Dual()
{
	CreateMesh();
	AttachmentData_Left.AttachPoint = EWeaponPoint::Point_L;
	AttachmentData_Left.Owner = this;

	eFiringPoint = EWeaponPoint::Point_R;
	eFireMode = EFireMode::Dual_Simultaneous;	
}

void UPBWeapAttach_Dual::CreateMesh()
{
	if (AttachmentData_Left.WeapMesh.Mesh1P == nullptr)
	{
		AttachmentData_Left.WeapMesh.Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh1P_Left"));
	}
	if (AttachmentData_Left.WeapMesh.Mesh3P == nullptr)
	{
		AttachmentData_Left.WeapMesh.Mesh3P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh3P_Left"));
	}

	SetupMesh(AttachmentData_Left);
}

void UPBWeapAttach_Dual::Init()
{
	Super::Init();

	if (AttachmentData_Left.WeapMesh.Mesh1P || AttachmentData_Left.WeapMesh.Mesh3P)
	{
		Attachments.Add(AttachmentData_Left);

		SetPanini(AttachmentData_Left.WeapMesh.Mesh1P, true);
		SetPanini(AttachmentData_Left.WeapMesh.Mesh3P, false);
	}
}

void UPBWeapAttach_Dual::OnMeleeAttack()
{
	APBWeapon* Weapon = GetOuterAPBWeapon();
	if (Weapon)
	{
		Weapon->SetFireMode(eFireMode);

		switch (eFireMode)
		{
		case EFireMode::Single:
			PlayMeleeAttackEffect(eFiringPoint);
			break;
		case EFireMode::Dual_Alternating:
			Weapon->SetFirePoint(eFiringPoint);
			PlayMeleeAttackEffect(eFiringPoint);
			SwitchFiringPoint();
			break;
		case EFireMode::Dual_Simultaneous:
			Weapon->SetFirePoint(EWeaponPoint::Point_R);
			PlayMeleeAttackEffect(EWeaponPoint::Point_R);

			Weapon->SetFirePoint(EWeaponPoint::Point_L);
			PlayMeleeAttackEffect(EWeaponPoint::Point_L);
			break;
		}
	}
}

void UPBWeapAttach_Dual::OnFiring()
{
	APBWeapon* Weapon = GetOuterAPBWeapon();
	if (Weapon)
	{
		Weapon->SetFireMode(eFireMode);

		switch (eFireMode)
		{
		case EFireMode::Single:
			Weapon->Firing(eFiringPoint);
			break;
		case EFireMode::Dual_Alternating:
			Weapon->SetFirePoint(eFiringPoint);
			Weapon->Firing(eFiringPoint);
			SwitchFiringPoint();
			break;
		case EFireMode::Dual_Simultaneous:
			Weapon->SetFirePoint(EWeaponPoint::Point_R);
			Weapon->Firing(EWeaponPoint::Point_R);

			Weapon->SetFirePoint(EWeaponPoint::Point_L);
			Weapon->Firing(EWeaponPoint::Point_L);
			break;
		}
	}
}

void UPBWeapAttach_Dual::PlayWeaponFireEffects()
{
	APBWeapon* Weapon = GetOuterAPBWeapon();
	if (Weapon)
	{

		if (Weapon->GetFireMode() == EFireMode::Dual_Simultaneous)
		{
			PlayWeaponFireEffect(EWeaponPoint::Point_R);
			PlayWeaponFireEffect(EWeaponPoint::Point_L);
		}
		else
		{		
			EWeaponPoint FirePoint = Weapon->GetFirePoint();
			PlayWeaponFireEffect(FirePoint);
		}
	}
}

void UPBWeapAttach_Dual::PlayWeaponFireEffect(EWeaponPoint ePoint)
{
	Super::PlayWeaponFireEffect(ePoint);
}

EWeaponPoint UPBWeapAttach_Dual::GetFiringPoint()
{
	return eFiringPoint;
}

void UPBWeapAttach_Dual::SwitchFiringPoint()
{
	eFiringPoint = (eFiringPoint == EWeaponPoint::Point_L) ? EWeaponPoint::Point_R : EWeaponPoint::Point_L;
}
