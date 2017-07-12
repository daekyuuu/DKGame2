// Fill out your copyright notice in the Description page of Project Settings.

#include "PBGame.h"
#include "Player/PBCharacter.h"
#include "Particles/ParticleSystemComponent.h"
#include "PBWeapAttachment.h"
#include "Effects/PBImpactEffect.h"
#include "Weapons/Bullet/PBBulletBase.h"
#include "Weapons/Bullet/PBInst_MeleeBullet.h"
#include "Weapons/Projectile/PBProjectile.h"
#include "Weapons/Bullet/PBInst_Bullet.h"
#include "Animation/AnimMontage.h"
#include "Table/Item/PBItemTable_Weap.h"
#include "PBGameViewportClient.h"
#include "Player/PBPlayerController.h"
#include "PBGameInstance.h"
#include "PBGameplayStatics.h"

// Sets default values
UPBWeapAttachment::UPBWeapAttachment()
{
	eFiringPoint = EWeaponPoint::Point_R;
	AttachmentData_Right.AttachPoint = EWeaponPoint::Point_R;
	AttachmentData_Right.Owner = this;

	CreateMesh();
}

void UPBWeapAttachment::CreateMesh()
{
	if (AttachmentData_Right.WeapMesh.Mesh1P == nullptr)
	{
		AttachmentData_Right.WeapMesh.Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh1P_Right"));
	}
	if (AttachmentData_Right.WeapMesh.Mesh3P == nullptr)
	{
		AttachmentData_Right.WeapMesh.Mesh3P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh3P_Right"));
	}

	SetupMesh(AttachmentData_Right);
}

void UPBWeapAttachment::SetupMesh(FWeaponAttachmentData& Data)
{
	if (Data.WeapMesh.Mesh1P)
	{
		Data.WeapMesh.Mesh1P->MeshComponentUpdateFlag = EMeshComponentUpdateFlag::OnlyTickPoseWhenRendered;
		Data.WeapMesh.Mesh1P->bReceivesDecals = false;
		Data.WeapMesh.Mesh1P->CastShadow = false;
		Data.WeapMesh.Mesh1P->SetCollisionObjectType(ECC_WorldDynamic);
		Data.WeapMesh.Mesh1P->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		Data.WeapMesh.Mesh1P->SetCollisionResponseToAllChannels(ECR_Ignore);
		//Data.WeapMesh.Mesh1P->SetupAttachment(Weapon->GetRootComponent());
	}

	if (Data.WeapMesh.Mesh3P)
	{
		Data.WeapMesh.Mesh3P->MeshComponentUpdateFlag = EMeshComponentUpdateFlag::OnlyTickPoseWhenRendered;
		Data.WeapMesh.Mesh3P->bReceivesDecals = false;
		Data.WeapMesh.Mesh3P->CastShadow = true;
		Data.WeapMesh.Mesh3P->SetCollisionObjectType(ECC_WorldDynamic);
		Data.WeapMesh.Mesh3P->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		Data.WeapMesh.Mesh3P->SetCollisionResponseToAllChannels(ECR_Ignore);
		Data.WeapMesh.Mesh3P->SetCollisionResponseToChannel(COLLISION_WEAPON, ECR_Block);
		Data.WeapMesh.Mesh3P->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
		Data.WeapMesh.Mesh3P->SetCollisionResponseToChannel(COLLISION_PROJECTILE, ECR_Block);
		//Data.WeapMesh.Mesh3P->SetupAttachment(WeapMesh1p);
	}
}

void UPBWeapAttachment::Init()
{
	Attachments.Empty();

	if (AttachmentData_Right.WeapMesh.Mesh1P || AttachmentData_Right.WeapMesh.Mesh3P)
	{
		Attachments.Add(AttachmentData_Right);

		SetPanini(AttachmentData_Right.WeapMesh.Mesh1P, true);
		SetPanini(AttachmentData_Right.WeapMesh.Mesh3P, false);
	}
}

void UPBWeapAttachment::Empty()
{
	Attachments.Empty();
}

void UPBWeapAttachment::AttachMeshToPawn()
{
	APBCharacter* MyPawn = GetOwnerPawn();
	if (MyPawn == nullptr)
		return;

	DetachMeshFromPawn();

	for (int32 i = 0; i < Attachments.Num(); i++)
	{
		const FWeaponAttachmentData& Data = Attachments[i];
	
		// For locally controller players we attach both weapons and let the bOnlyOwnerSee, bOwnerNoSee flags deal with visibility.
		FName AttachPoint = MyPawn->GetWeaponAttachPointName(Data.AttachPoint);
		//bool bFirstPerson = MyPawn->IsFirstPersonView();
		bool bFirstPerson = GetViewerIsFirstPerson();

		USkeletalMeshComponent* PawnMesh1p = MyPawn->GetSpecificPawnMesh(true);
		USkeletalMeshComponent* PawnMesh3p = MyPawn->GetSpecificPawnMesh(false);
		USkeletalMeshComponent* WeapMesh1p = Data.WeapMesh.Mesh1P;
		USkeletalMeshComponent* WeapMesh3p = Data.WeapMesh.Mesh3P;

		if (WeapMesh1p)
		{
			WeapMesh1p->AttachToComponent(PawnMesh1p, FAttachmentTransformRules::KeepRelativeTransform, AttachPoint);
		}
		if (WeapMesh3p)
		{
			WeapMesh3p->AttachToComponent(PawnMesh3p, FAttachmentTransformRules::KeepRelativeTransform, AttachPoint);
		}
	}	
}

void UPBWeapAttachment::DetachMeshFromPawn()
{
	for (int32 i = 0; i < Attachments.Num(); i++)
	{
		const FWeaponAttachmentData& Data = Attachments[i];

		USkeletalMeshComponent* WeapMesh1p = Data.WeapMesh.Mesh1P;
		USkeletalMeshComponent* WeapMesh3p = Data.WeapMesh.Mesh3P;

		if (WeapMesh1p)
		{
			WeapMesh1p->DetachFromComponent(FDetachmentTransformRules::KeepRelativeTransform);
			WeapMesh1p->SetHiddenInGame(true);
		}
		
		if (WeapMesh3p)
		{
			WeapMesh3p->DetachFromComponent(FDetachmentTransformRules::KeepRelativeTransform);
			WeapMesh3p->SetHiddenInGame(true);
		}		
	}
}

void UPBWeapAttachment::SetPanini(USkeletalMeshComponent* Mesh, bool bPanini)
{
	if (Mesh == nullptr)
	{
		return;
	}

	for (int32 MaterialIndex = 0; MaterialIndex < Mesh->GetNumMaterials(); ++MaterialIndex)
	{
		UMaterialInterface* MI = Mesh->GetMaterial(MaterialIndex);
		FString PathName = MI->GetOutermost()->GetPathName();
		FString Suffix = TEXT("_Panini");
		
		if (bPanini)
		{
			if (!PathName.Contains(*Suffix))
			{
				PathName += Suffix;
			}
		}
		else
		{
			if (PathName.Contains(*Suffix))
			{
				PathName.RemoveFromEnd(Suffix);
			}
		}

		UMaterialInterface* NewMI = LoadObject<UMaterialInterface>(nullptr, *PathName, nullptr, LOAD_NoWarn|LOAD_Quiet);
		if (NewMI)
		{
			//Mesh->CreateDynamicMaterialInstance(MaterialIndex, NewMI);
			Mesh->SetMaterial(MaterialIndex, NewMI);
		}
	}
}

void UPBWeapAttachment::UpdateMeshVisibilitys(bool bForce)
{
	for (int32 i = 0; i < Attachments.Num(); i++)
	{
		UpdateMeshVisibility(EWeaponPoint(i), bForce);
	}
}

void UPBWeapAttachment::UpdateMeshVisibility(EWeaponPoint ePoint, bool bForce)
{
	APBCharacter* MyPawn = GetOwnerPawn();
	if (MyPawn == nullptr)
		return;

	bool bVisible = bForce;
	//bool bFirstPerson = MyPawn->IsFirstPersonView();
	bool bFirstPerson = GetViewerIsFirstPerson();

	int32 i = int32(ePoint);
	if (Attachments.IsValidIndex(i))
	{
		FWeaponAttachmentData& Data = Attachments[i];

		USkeletalMeshComponent* WeapMesh1p = Data.WeapMesh.Mesh1P;
		USkeletalMeshComponent* WeapMesh3p = Data.WeapMesh.Mesh3P;

		if (WeapMesh1p)
		{
			WeapMesh1p->SetHiddenInGame(!bFirstPerson);
		}
		if (WeapMesh3p)
		{
			WeapMesh3p->SetHiddenInGame(bFirstPerson);
		}

		USkeletalMeshComponent* WeapMesh = Data.GetSpecificWeapMesh(bFirstPerson);
		if (WeapMesh)
		{
			WeapMesh->SetHiddenInGame(!bVisible || !bForce);
		}
	}
}

void UPBWeapAttachment::ForceMeshVisibilitys(bool bVisible, bool bFirstPerson)
{
	for (int32 i = 0; i < Attachments.Num(); i++)
	{
		if (Attachments.IsValidIndex(i))
		{
			FWeaponAttachmentData& Data = Attachments[i];
			USkeletalMeshComponent* WeapMesh = Data.GetSpecificWeapMesh(bFirstPerson);

			if (WeapMesh)
			{
				WeapMesh->SetHiddenInGame(!bVisible);
			}
		}
	}
}

void UPBWeapAttachment::OnMeleeAttack()
{
	APBWeapon* Weapon = GetOuterAPBWeapon();
	if (Weapon)
	{
		PlayMeleeAttackEffect(EWeaponPoint::Point_R);
		PlayMeleeAttackEffect(EWeaponPoint::Point_L);
	}
}


void UPBWeapAttachment::OnMeleeAttackEnd()
{
	StopMeleeAttackEffect(EWeaponPoint::Point_R);
	StopMeleeAttackEffect(EWeaponPoint::Point_L);
}

void UPBWeapAttachment::OnFiring()
{
	APBWeapon* Weapon = GetOuterAPBWeapon();
	if (Weapon)
	{
		Weapon->SetFireMode(EFireMode::Single);
		Weapon->Firing(eFiringPoint);
	}
}

void UPBWeapAttachment::PlayMeleeAttackEffect(EWeaponPoint ePoint)
{
	PlaySpecificWeaponAnim(ePoint, EWeaponAnimType::MeleeAttack);
	PlayWeaponSound(ePoint, EWeaponSoundType::MeleeAttack);
	PlayFireForceFeedback(ePoint);
	PlayMeleeAttackCameraShake(ePoint);
}

void UPBWeapAttachment::StopMeleeAttackEffect(EWeaponPoint ePoint)
{
	StopSpecificWeaponAnim(ePoint, EWeaponAnimType::MeleeAttack);
	StopMeleeAttackCameraShake(ePoint);
}

void UPBWeapAttachment::PlayWeaponFireEffects()
{
	PlayWeaponFireEffect(eFiringPoint);
}

void UPBWeapAttachment::PlayWeaponFireEffect(EWeaponPoint ePoint)
{
	APBCharacter* MyPawn = GetOwnerPawn();
	if (MyPawn == nullptr)
		return;

	APBWeapon* Weapon = GetOuterAPBWeapon();
	if (Weapon == nullptr)
		return;

	//const bool bFirstPerson = MyPawn->IsFirstPersonView();
	const bool bFirstPerson = GetViewerIsFirstPerson();

	int32 i = int32(ePoint);
	if (Attachments.IsValidIndex(i))
	{
		FWeaponAttachmentData& Data = Attachments[i];		
		USkeletalMeshComponent* WeapMesh1p = Data.WeapMesh.Mesh1P;
		USkeletalMeshComponent* WeapMesh3p = Data.WeapMesh.Mesh3P;

		// FX		
		if (bFirstPerson)
		{
			FTransform AttachedOffset;

			// apply panini projection position			
			if (WeapMesh1p && WeapMesh1p->GetMaterial(0))
			{				
				FTransform MuzzleTransform = WeapMesh1p->GetSocketTransform(Data.MuzzleFX.FXPointName);
				FTransform PaniniTransform = MuzzleTransform;
				FVector AdjustedFXLocation = Weapon->GetPaniniLocation(MuzzleTransform.GetLocation(), WeapMesh1p->GetMaterial(0));
				PaniniTransform.SetLocation(AdjustedFXLocation);
				AttachedOffset = PaniniTransform.GetRelativeTransform(MuzzleTransform);
			}

			if (WeapMesh1p && Data.MuzzleFX.Data1P.FX && (!Data.MuzzleFX.Data1P.bFXLooped || Data.MuzzlePSC == nullptr))
			{
				Data.MuzzlePSC = UGameplayStatics::SpawnEmitterAttached(Data.MuzzleFX.Data1P.FX, WeapMesh1p, Data.MuzzleFX.FXPointName, AttachedOffset.GetLocation(), AttachedOffset.GetRotation().Rotator());
				if (Data.MuzzlePSC)
				{
					Data.MuzzlePSC->bOwnerNoSee = false;
					Data.MuzzlePSC->bOnlyOwnerSee = true;
					Data.MuzzlePSC->SetRelativeScale3D(Data.MuzzleFX.Data1P.FXScale);
				}
			}

			//for split screen
			if (WeapMesh3p && Data.MuzzleFX.Data3P.FX && (!Data.MuzzleFX.Data3P.bFXLooped || Data.MuzzlePSCSecondary == nullptr))
			{
				Data.MuzzlePSCSecondary = UGameplayStatics::SpawnEmitterAttached(Data.MuzzleFX.Data3P.FX, WeapMesh3p, Data.MuzzleFX.FXPointName);
				if (Data.MuzzlePSCSecondary)
				{
					Data.MuzzlePSCSecondary->bOwnerNoSee = true;
					Data.MuzzlePSCSecondary->bOnlyOwnerSee = false;
					Data.MuzzlePSCSecondary->SetRelativeScale3D(Data.MuzzleFX.Data3P.FXScale);
				}
			}
		}
		else
		{
			if (WeapMesh3p && Data.MuzzleFX.Data3P.FX && (!Data.MuzzleFX.Data3P.bFXLooped || Data.MuzzlePSC == nullptr))
			{
				Data.MuzzlePSC = UGameplayStatics::SpawnEmitterAttached(Data.MuzzleFX.Data3P.FX, WeapMesh3p, Data.MuzzleFX.FXPointName);
				if (Data.MuzzlePSC)
				{
					Data.MuzzlePSC->SetRelativeScale3D(Data.MuzzleFX.Data3P.FXScale);
				}
			}
		}

		// Anim
		if (!Data.WeapAnim.bLoopedFireAnim || !Data.bPlayingFireAnim)
		{
			const FWeaponAnim& Animation = Data.GetAnimation(Weapon->GetFireAnimType());
			USkeletalMeshComponent* UseMesh = Data.GetSpecificWeapMesh(bFirstPerson);
			Weapon->PlaySpecificWeaponAnim(UseMesh, Animation);
			Data.bPlayingFireAnim = true;
		}

		// Sound
		if (Data.WeapSound.bLoopedFireSound)
		{
			if (Data.FireAC == nullptr)
			{
				Data.FireAC = Weapon->PlayWeaponSoundCue(Data.WeapSound.FireLoopSound);
			}
		}
		else
		{
			Data.FireAC = Weapon->PlayWeaponSoundCue(Data.WeapSound.FireSound);
		}

		
		APBPlayerController* PC = (MyPawn != nullptr) ? Cast<APBPlayerController>(MyPawn->Controller) : nullptr;
		if (PC != nullptr && PC->IsLocalController())
		{
			// Physics Feedback
			if (Data.FireForceFeedback != nullptr)
			{
				PC->ClientPlayForceFeedback(Data.FireForceFeedback, false, "Weapon");
			}

			if (Data.FireCameraShake)
			{
				PC->ClientPlayCameraShake(Data.FireCameraShake);
			}
		}
	}
}

void UPBWeapAttachment::StopWeaponFireEffects()
{
	APBWeapon* Weapon = GetOuterAPBWeapon();
	if (Weapon == nullptr)
		return;

	//bool bFirstPerson = GetOwnerPawn() ? GetOwnerPawn()->IsFirstPersonView() : false;
	bool bFirstPerson = GetViewerIsFirstPerson();

	for (int32 i = 0; i < Attachments.Num(); i++)
	{
		FWeaponAttachmentData& Data = Attachments[i];

		// effect
		if (Data.MuzzleFX.Data1P.bFXLooped || Data.MuzzleFX.Data3P.bFXLooped)
		{
			if (Data.MuzzlePSC)
			{
				Data.MuzzlePSC->DeactivateSystem();
				Data.MuzzlePSC = nullptr;
			}
			if (Data.MuzzlePSCSecondary)
			{
				Data.MuzzlePSCSecondary->DeactivateSystem();
				Data.MuzzlePSCSecondary = nullptr;
			}
		}

		// anim
		if (Data.WeapAnim.bLoopedFireAnim && Data.bPlayingFireAnim)
		{
			FWeaponAnim Animation = Data.GetAnimation(Weapon->GetFireAnimType());
			USkeletalMeshComponent* UseMesh = Data.GetSpecificWeapMesh(bFirstPerson);
			Weapon->StopSpecificWeaponAnim(UseMesh, Animation, Data.WeapAnim.bLoopedFireAnim);
			Data.bPlayingFireAnim = false;
		}

		// sound
		if (Data.FireAC)
		{
			Data.FireAC->FadeOut(Data.WeapSound.FireSoundFadeOutTime, 0.0f);
			Data.FireAC = nullptr;

			Weapon->PlayWeaponSoundCue(Data.WeapSound.FireFinishSound);
		}
	}
}

void UPBWeapAttachment::PlayFireForceFeedback(EWeaponPoint ePoint)
{
	int32 i = int32(ePoint);
	if (Attachments.IsValidIndex(i))
	{
		FWeaponAttachmentData& Data = Attachments[i];

		// Physics Feedback
		APBCharacter* MyPawn = GetOwnerPawn();
		APBPlayerController* PC = (MyPawn != nullptr) ? Cast<APBPlayerController>(MyPawn->Controller) : nullptr;
		if (PC != nullptr && PC->IsLocalController())
		{
			if (Data.FireForceFeedback != nullptr)
			{
				PC->ClientPlayForceFeedback(Data.FireForceFeedback, false, "Weapon");
			}
		}
	}
}

void UPBWeapAttachment::PlayMeleeAttackCameraShake(EWeaponPoint ePoint)
{
	int32 i = int32(ePoint);
	if (Attachments.IsValidIndex(i))
	{
		FWeaponAttachmentData& Data = Attachments[i];

		PlayCameraShake(Data.MeleeAttackCameraShake);
	}
}

void UPBWeapAttachment::StopMeleeAttackCameraShake(EWeaponPoint ePoint)
{
	int32 i = int32(ePoint);
	if (Attachments.IsValidIndex(i))
	{
		FWeaponAttachmentData& Data = Attachments[i];

		StopCameraShake(Data.MeleeAttackCameraShake);
	}
}

void UPBWeapAttachment::PlayFireCameraShake(EWeaponPoint ePoint)
{
	int32 i = int32(ePoint);
	if (Attachments.IsValidIndex(i))
	{
		FWeaponAttachmentData& Data = Attachments[i];

		PlayCameraShake(Data.FireCameraShake);
	}
}

void UPBWeapAttachment::PlayEquipCameraShake(EWeaponPoint ePoint)
{
	int32 i = int32(ePoint);
	if (Attachments.IsValidIndex(i))
	{
		FWeaponAttachmentData& Data = Attachments[i];

		PlayCameraShake(Data.EquipCameraShake);
	}
}

void UPBWeapAttachment::StopEquipCameraShake(EWeaponPoint ePoint)
{
	int32 i = int32(ePoint);
	if (Attachments.IsValidIndex(i))
	{
		FWeaponAttachmentData& Data = Attachments[i];

		StopCameraShake(Data.ReloadCameraShake);
	}
}

void UPBWeapAttachment::PlayReloadCameraShake(EWeaponPoint ePoint)
{
	int32 i = int32(ePoint);
	if (Attachments.IsValidIndex(i))
	{
		FWeaponAttachmentData& Data = Attachments[i];

		PlayCameraShake(Data.ReloadCameraShake);
	}
}

void UPBWeapAttachment::StopReloadCameraShake(EWeaponPoint ePoint)
{
	int32 i = int32(ePoint);
	if (Attachments.IsValidIndex(i))
	{
		FWeaponAttachmentData& Data = Attachments[i];

		StopCameraShake(Data.ReloadCameraShake);
	}
}

void UPBWeapAttachment::PlayCameraShake(TSubclassOf <UCameraShake> ShakeClass)
{
	APBCharacter* MyPawn = GetOwnerPawn();
	APBPlayerController* PC = (MyPawn != nullptr) ? Cast<APBPlayerController>(MyPawn->Controller) : nullptr;
	bool bFirstPerson = GetViewerIsFirstPerson();
	if (PC && bFirstPerson)
	{
		if (ShakeClass)
		{
			PC->ClientPlayCameraShake(ShakeClass);
		}		
	}
}

void UPBWeapAttachment::StopCameraShake(TSubclassOf <UCameraShake> ShakeClass)
{
	APBCharacter* MyPawn = GetOwnerPawn();
	APBPlayerController* PC = (MyPawn != nullptr) ? Cast<APBPlayerController>(MyPawn->Controller) : nullptr;
	if (PC)
	{
		if (ShakeClass)
		{
			PC->ClientStopCameraShake(ShakeClass);
		}		
	}
}

//////////////////////////////////////////////////////////////////////////

void UPBWeapAttachment::PlayWeaponSounds(EWeaponSoundType eSound)
{
	for (int32 i = 0; i < Attachments.Num(); i++)
	{
		PlayWeaponSound(EWeaponPoint(i), eSound);
	}
}

UAudioComponent* UPBWeapAttachment::PlayWeaponSound(EWeaponPoint ePoint, EWeaponSoundType eSound)
{
	int32 i = int32(ePoint);
	if (Attachments.IsValidIndex(i))
	{
		FWeaponAttachmentData& Data = Attachments[i];

		APBWeapon* Weapon = GetOuterAPBWeapon();
		if (Weapon)
		{
			USoundCue* Sound = Data.GetSound(eSound);
			return Weapon->PlayWeaponSoundCue(Sound);
		}
	}
	return nullptr;
}

//////////////////////////////////////////////////////////////////////////

float UPBWeapAttachment::GetWeaponAnimationLength(EWeaponAnimType eAnim)
{
	float Duration = 0.0f;
	bool bFirstPerson = GetViewerIsFirstPerson();

	for (int32 i = 0; i < Attachments.Num(); i++)
	{
		float Length = GetSpecificWeaponAnimLength(EWeaponPoint(i), eAnim);
		if (Duration < Length)
			Duration = Length;
	}

	return Duration;
}

float UPBWeapAttachment::GetSpecificWeaponAnimLength(EWeaponPoint ePoint, EWeaponAnimType eAnim)
{
	float Duration = 0.0f;
	bool bFirstPerson = GetViewerIsFirstPerson();
	int32 i = int32(ePoint);

	if (Attachments.IsValidIndex(i))
	{
		FWeaponAttachmentData& Data = Attachments[i];
		const FWeaponAnim& Animation = Data.GetAnimation(eAnim);

		APBWeapon* Weapon = GetOuterAPBWeapon();

		bool IsFirstPerson = Weapon->GetViewerIsFirstPerson();
		
		UAnimMontage* WeapAnim = IsFirstPerson ? Animation.Weap1P : Animation.Weap3P;
		if (WeapAnim && WeapAnim->GetPlayLength() > Duration)
		{
			Duration = WeapAnim->GetPlayLength();
		}

		UAnimMontage* PawnAnim = IsFirstPerson ? Animation.Pawn1P : Animation.Pawn3P;
		if (PawnAnim && PawnAnim->GetPlayLength() > Duration)
		{
			Duration = PawnAnim->GetPlayLength();
		}
	}

	return Duration;
}

float UPBWeapAttachment::PlayWeaponAnimation(EWeaponAnimType eAnim)
{
	float Duration = 0.0f;

	for (int32 i = 0; i < Attachments.Num(); i++)
	{
		float Length = PlaySpecificWeaponAnim(EWeaponPoint(i), eAnim);
		if (Duration < Length)
			Duration = Length;
	}

	return Duration;
}

float UPBWeapAttachment::PlaySpecificWeaponAnim(EWeaponPoint ePoint, EWeaponAnimType eAnim)
{
	float Duration = 0.0f;
	bool bFirstPerson = GetViewerIsFirstPerson();
	int32 i = int32(ePoint);

	if (Attachments.IsValidIndex(i))
	{
		FWeaponAttachmentData& Data = Attachments[i];
		const FWeaponAnim& Animation = Data.GetAnimation(eAnim);

		APBWeapon* Weapon = GetOuterAPBWeapon();
		if (Weapon)
		{
			USkeletalMeshComponent* UseMesh = Data.GetSpecificWeapMesh(bFirstPerson);
			Duration = Weapon->PlaySpecificWeaponAnim(UseMesh, Animation);
		}
	}

	return Duration;
}

void UPBWeapAttachment::StopWeaponAnimation(EWeaponAnimType eAnim, bool GotoEndSection /*= false*/)
{
	for (int32 i = 0; i < Attachments.Num(); i++)
	{
		StopSpecificWeaponAnim(EWeaponPoint(i), eAnim, GotoEndSection);
	}
}

void UPBWeapAttachment::StopSpecificWeaponAnim(EWeaponPoint ePoint, EWeaponAnimType eAnim, bool GotoEndSection /*= false*/)
{
	float Duration = 0.0f;
	bool bFirstPerson = GetViewerIsFirstPerson();
	int32 i = int32(ePoint);

	if (Attachments.IsValidIndex(i))
	{
		FWeaponAttachmentData& Data = Attachments[i];
		const FWeaponAnim& Animation = Data.GetAnimation(eAnim);

		APBWeapon* Weapon = GetOuterAPBWeapon();
		if (Weapon)
		{
			USkeletalMeshComponent* UseMesh = Data.GetSpecificWeapMesh(bFirstPerson);
			Weapon->StopSpecificWeaponAnim(UseMesh, Animation, GotoEndSection);
		}
	}
}

APBBulletBase* UPBWeapAttachment::SpawnBullet(EWeaponPoint ePoint, const FTransform& Transform)
{
	int32 i = int32(ePoint);
	if (Attachments.IsValidIndex(i))
	{
		return Attachments[i].SpawnBullet(GetOuterAPBWeapon(), Transform);
	}
	return nullptr;
}

APBBulletBase* UPBWeapAttachment::GetBullet(EWeaponPoint ePoint)
{
	int32 i = int32(ePoint);
	if (Attachments.IsValidIndex(i))
	{
		FWeaponAttachmentData& Data = Attachments[i];
		
		if (Data.Bullet == nullptr)
		{
			return Data.SpawnBullet(GetOuterAPBWeapon(), FTransform::Identity);
		}
		return Data.Bullet;
	}
	return nullptr;
}

TSubclassOf<class APBBulletBase> UPBWeapAttachment::GetBulletClass(EWeaponPoint ePoint)
{
	int32 i = int32(ePoint);
	if (Attachments.IsValidIndex(i))
	{
		FWeaponAttachmentData& Data = Attachments[i];
		return Data.BulletClass;
	}
	return nullptr;
}

APBInst_MeleeBullet* UPBWeapAttachment::SpawnMeleeBullet(EWeaponPoint ePoint, const FTransform& Transform)
{
	int32 i = int32(ePoint);
	if (Attachments.IsValidIndex(i))
	{
		return Attachments[i].SpawnMeleeBullet(GetOuterAPBWeapon(), Transform);
	}
	return nullptr;
}

APBInst_MeleeBullet* UPBWeapAttachment::GetMeleeBullet(EWeaponPoint ePoint)
{
	int32 i = int32(ePoint);
	if (Attachments.IsValidIndex(i))
	{
		FWeaponAttachmentData& Data = Attachments[i];

		if (Data.Bullet == nullptr)
		{
			return Data.SpawnMeleeBullet(GetOuterAPBWeapon(), FTransform::Identity);
		}
		return Data.MeleeBullet;
	}
	return nullptr;
}

void UPBWeapAttachment::SetBulletInstigator(APawn* Pawn)
{
	for (int32 i = 0; i < Attachments.Num(); i++)
	{
		FWeaponAttachmentData& Data = Attachments[i];

		if (Data.Bullet)
		{
			Data.Bullet->Instigator = Pawn;
		}

		if (Data.MeleeBullet)
		{
			Data.MeleeBullet->Instigator = Pawn;
		}			
	}
}

bool UPBWeapAttachment::ApplyTableData(const struct FPBWeaponTableData* Data)
{
	bool bIsDirty = false;

	for (FWeaponAttachmentData& AttachData : Attachments)
	{
		bool bTempIsDirty = false;

		if (AttachData.BulletClass)
		{
			auto DefaultBullet = Cast<APBBulletBase>(AttachData.BulletClass->GetDefaultObject());
			if (DefaultBullet)
			{
				bTempIsDirty = DefaultBullet->ApplyTableData(Data);

				if (!bIsDirty)
				{
					bIsDirty = bTempIsDirty;
				}
			}
		}

		if (AttachData.MeleeBulletClass)
		{
			auto DefaultMeleeBullet = Cast<APBBulletBase>(AttachData.MeleeBulletClass->GetDefaultObject());
			if (DefaultMeleeBullet)
			{
				bTempIsDirty = DefaultMeleeBullet->ApplyTableData(Data);

				if (!bIsDirty)
				{
					bIsDirty = bTempIsDirty;
				}
			}
		}
	}
	return bIsDirty;
}

USkeletalMeshComponent* UPBWeapAttachment::GetWeaponMesh(EWeaponPoint ePoint)
{
	/*
	APBCharacter* MyPawn = GetOwnerPawn();	
	if (MyPawn)
	{
		int32 i = int32(ePoint);
		if (Attachments.IsValidIndex(i))
		{
			return Attachments[i].GetSpecificWeapMesh(MyPawn->IsFirstPersonView());
		}
	}

	return nullptr;
	*/

	int32 i = int32(ePoint);

	if (nullptr == GetOwnerPawn() || false == Attachments.IsValidIndex(i))
		return nullptr;

	return Attachments[i].GetSpecificWeapMesh(GetViewerIsFirstPerson());
}

USkeletalMeshComponent* UPBWeapAttachment::GetSpecificWeapMesh(EWeaponPoint ePoint, bool bFirstPerson)
{
	/*
	APBCharacter* MyPawn = GetOwnerPawn();
	if (MyPawn)
	{
		int32 i = int32(ePoint);
		if (Attachments.IsValidIndex(i))
		{
			return Attachments[i].GetSpecificWeapMesh(bFirstPerson);
		}
	}

	return nullptr;
	*/

	int32 i = int32(ePoint);

	if (nullptr == GetOwnerPawn() || false == Attachments.IsValidIndex(i))
		return nullptr;

	return Attachments[i].GetSpecificWeapMesh(bFirstPerson);
}

USoundCue* UPBWeapAttachment::GetSound(EWeaponPoint ePoint, EWeaponSoundType eSound)
{
	int32 i = int32(ePoint);
	if (Attachments.IsValidIndex(i))
	{
		return Attachments[i].GetSound(eSound);
	}

	return nullptr;
}

const FWeaponAnim& UPBWeapAttachment::GetAnimation(EWeaponPoint ePoint, EWeaponAnimType eAnim)
{
	int32 i = int32(ePoint);
	if (Attachments.IsValidIndex(i))
	{
		return Attachments[i].GetAnimation(eAnim);
	}

	return WeaponAnimNone;
}

FName UPBWeapAttachment::GetMuzzlePointName(EWeaponPoint ePoint)
{
	int32 i = int32(ePoint);
	if (Attachments.IsValidIndex(i))
	{
		return Attachments[i].MuzzleFX.FXPointName;
	}
	return FName();
}

EWeaponPoint UPBWeapAttachment::GetFiringPoint()
{
	return EWeaponPoint::Point_R;
}

int32 UPBWeapAttachment::GetAttachmentDataCount()
{
	return Attachments.Num();
}

TArray<FWeaponAttachmentData>& UPBWeapAttachment::GetAttachmentDataList()
{
	return Attachments;
}

//////////////////////////////////////////////////////////////////////////

USoundCue* FWeaponAttachmentData::GetSound(EWeaponSoundType eSound)
{
	switch (eSound)
	{
	case EWeaponSoundType::Fire:		return WeapSound.FireSound;
	case EWeaponSoundType::FireLoop:	return WeapSound.FireLoopSound;
	case EWeaponSoundType::FireFinish:	return WeapSound.FireFinishSound;
	case EWeaponSoundType::OutofAmmo:	return WeapSound.OutOfAmmoSound;
	case EWeaponSoundType::Reload:		return WeapSound.ReloadSound;
	case EWeaponSoundType::Equip:		return WeapSound.EquipSound;
	case EWeaponSoundType::MeleeAttack: return WeapSound.MeleeAttackSound;
	}
	return nullptr;
}

const FWeaponAnim& FWeaponAttachmentData::GetAnimation(EWeaponAnimType eAnim)
{
	auto GI = Cast<UPBGameInstance>(UGameplayStatics::GetGameInstance(Owner));
	const bool bLobbyAnim = GI ? !GI->IsCurrentState(PBGameInstanceState::Playing) : false;

	// Lobby
	APBCharacter* MyPawn = Owner->GetOwnerPawn();
	if (bLobbyAnim && MyPawn)
	{
		const bool bMale = MyPawn->GetCharaterData().Gender == EPBCharacterGender::Male;

		switch (eAnim)
		{
		case EWeaponAnimType::Equip: //To Lobby_Equip
		case EWeaponAnimType::Lobby_Equip:			return bMale ? WeapAnim_LobbyMale.EquipAnim : WeapAnim_LobbyFemale.EquipAnim;
		case EWeaponAnimType::Lobby_WalkingStart:	return bMale ? WeapAnim_LobbyMale.WalkingStartAnim : WeapAnim_LobbyFemale.WalkingStartAnim;
		case EWeaponAnimType::Lobby_WalkingFinish:	return bMale ? WeapAnim_LobbyMale.WalkingFinishAnim : WeapAnim_LobbyFemale.WalkingFinishAnim;
		}
	}

	// InGame
	switch (eAnim)
	{
	case EWeaponAnimType::Fire:			return WeapAnim.FireAnim;
	case EWeaponAnimType::FireZoom:		return WeapAnim.FireAnim_Zoom;
	case EWeaponAnimType::Reload:		return WeapAnim.ReloadAnim;
	case EWeaponAnimType::Equip:		return WeapAnim.EquipAnim;
	case EWeaponAnimType::MeleeAttack:	return WeapAnim.MeleeAttackAnim;
	}
	return WeaponAnimNone;
}

USkeletalMeshComponent* FWeaponAttachmentData::GetSpecificWeapMesh(bool bFirstPerson)
{
	return bFirstPerson ? WeapMesh.Mesh1P : WeapMesh.Mesh3P;
}

APBBulletBase* FWeaponAttachmentData::SpawnBullet(APBWeapon* Weapon, const FTransform& Transform)
{
	if (Weapon == nullptr || Weapon->GetWorld() == nullptr)
		return nullptr;

	APBBulletBase* Inst = nullptr;

	if (BulletClass && Bullet == nullptr)
	{
		if (BulletClass->IsChildOf(APBInst_Bullet::StaticClass()))
		{
			// 무기 생성시 한번 호출된다.
			FActorSpawnParameters SpawnInfo;
			SpawnInfo.Instigator = Weapon->GetInstigator();
			SpawnInfo.Owner = Weapon;
			SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

			Inst = Bullet = Weapon->GetWorld()->SpawnActor<APBBulletBase>(BulletClass, SpawnInfo);
		}
		else if (BulletClass->IsChildOf(APBProjectile::StaticClass()))
		{
			// 현재 프로젝타일은 실시간으로 생성함. Bullet 에 레퍼런싱을 하지 않음.(차후 오브젝트풀 만들면 그곳에서 관리할 예정)
			Inst = Cast<APBBulletBase>(UPBGameplayStatics::BeginDeferredActorSpawnFromClass(Weapon, BulletClass, Transform, ESpawnActorCollisionHandlingMethod::AlwaysSpawn, Weapon));
		}
	}

	if (Inst)
	{
		Inst->WeaponId = Weapon->GetItemID();
		Inst->BulletPoint = AttachPoint;
	}		

	return Inst;
}

APBInst_MeleeBullet* FWeaponAttachmentData::SpawnMeleeBullet(APBWeapon* Weapon, const FTransform& Transform)
{
	if (Weapon == nullptr || Weapon->GetWorld() == nullptr)
		return nullptr;

	if (MeleeBulletClass && MeleeBullet == nullptr)
	{
		// 무기 생성시 한번 호출된다.
		FActorSpawnParameters SpawnInfo;
		SpawnInfo.Instigator = Weapon->GetInstigator();
		SpawnInfo.Owner = Weapon;
		SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		MeleeBullet = Weapon->GetWorld()->SpawnActor<APBInst_MeleeBullet>(MeleeBulletClass, SpawnInfo);
		MeleeBullet->WeaponId = Weapon->GetItemID();
	}
	return MeleeBullet;
}
