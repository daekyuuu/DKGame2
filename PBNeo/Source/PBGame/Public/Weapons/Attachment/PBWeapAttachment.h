// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Object.h"
#include "Weapons/PBWeaponTypes.h"
#include "Weapons/PBWeapon.h"
#include "PBWeapAttachment.generated.h"

// dual weapon 을 위해 attachment 개념을 적용하였다. 메쉬와 이펙트를 두개로 만들어서 듀얼처럼 보이게 눈속임한다.
// 모든 무기는 최소 한개의 AttachmentData 를 가져야 한다. dual 은 AttachmentData 2개.
class APBBulletBase;
class APBInst_MeleeBullet;

USTRUCT(BlueprintType)
struct FWeaponAttachmentData
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY()
	class UPBWeapAttachment* Owner;

	UPROPERTY(EditDefaultsOnly, Category = Bullet, meta = (DisplayName = "Bullet"))
	TSubclassOf<class APBBulletBase> BulletClass;
	class APBBulletBase* Bullet;

	UPROPERTY(EditDefaultsOnly, Category = Bullet, meta = (DisplayName = "MeleeBullet"))
	TSubclassOf<class APBInst_MeleeBullet> MeleeBulletClass;
	class APBInst_MeleeBullet* MeleeBullet;

	/** socket or bone name for attaching weapon mesh */
	UPROPERTY(VisibleDefaultsOnly, Category = Point)
	EWeaponPoint AttachPoint;

	UPROPERTY(EditAnywhere,		Category = Mesh,		meta = (DisplayName = "WeapMesh  /////"))
	FWeaponMesh WeapMesh;
	UPROPERTY(EditDefaultsOnly, Category = Sound,		meta = (DisplayName = "WeapSound  /////"))
	FWeaponSounds WeapSound;
	UPROPERTY(EditDefaultsOnly, Category = Animation,	meta = (DisplayName = "WeapAnim  /////"))
	FWeaponAnims WeapAnim;
	UPROPERTY(EditDefaultsOnly, Category = Animation,	meta = (DisplayName = "WeapAnim_LobbyMale  /////"))
	FLobbyWeaponAnims WeapAnim_LobbyMale;
	UPROPERTY(EditDefaultsOnly, Category = Animation,	meta = (DisplayName = "WeapAnim_LobbyFemale  /////"))
	FLobbyWeaponAnims WeapAnim_LobbyFemale;
	UPROPERTY(EditDefaultsOnly, Category = Effects,		meta = (DisplayName = "MuzzleFX  /////"))
	FWeaponMuzzleFX MuzzleFX;

	/** force feedback effect to play when the weapon is fired */
	UPROPERTY(EditDefaultsOnly, Category = Effects)
	class UForceFeedbackEffect *FireForceFeedback;

	UPROPERTY(EditDefaultsOnly, Category = Effects)
	TSubclassOf <class UCameraShake> MeleeAttackCameraShake;
	UPROPERTY(EditDefaultsOnly, Category = Effects)
	TSubclassOf <class UCameraShake> FireCameraShake;
	UPROPERTY(EditDefaultsOnly, Category = Effects)
	TSubclassOf <class UCameraShake> EquipCameraShake;
	UPROPERTY(EditDefaultsOnly, Category = Effects)
	TSubclassOf <class UCameraShake> ReloadCameraShake;
	//////////////////////////////////////////////////////////////////////////Transient

	UPROPERTY(Transient)
	uint32 bPlayingFireAnim : 1;
	UPROPERTY(Transient)
	class UAudioComponent* FireAC;
	UPROPERTY(Transient)
	class UParticleSystemComponent* MuzzlePSC;
	UPROPERTY(Transient)
	class UParticleSystemComponent* MuzzlePSCSecondary; //for split screen

	class USoundCue* GetSound(EWeaponSoundType eSound);
	const FWeaponAnim& GetAnimation(EWeaponAnimType eAnim);
	USkeletalMeshComponent* GetSpecificWeapMesh(bool bFirstPerson);
	class APBBulletBase* SpawnBullet(class APBWeapon* Weapon, const FTransform& Transform);
	class APBInst_MeleeBullet* SpawnMeleeBullet(class APBWeapon* Weapon, const FTransform& Transform);

	FWeaponAttachmentData()
	{
		AttachPoint = EWeaponPoint::Point_R;		
		bPlayingFireAnim = false;
		Bullet = nullptr;
		MeleeBullet = nullptr;
	}
};

UCLASS(DefaultToInstanced, BlueprintType, EditInlineNew, CustomConstructor, Within = PBWeapon, meta=(DisplayName="Weapon Single Setting"))
class PBGAME_API UPBWeapAttachment : public UObject
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	UPBWeapAttachment();

	inline bool GetViewerIsFirstPerson()
	{
		auto PlayerCharacter = Cast<class APBCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));
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

	inline class APBCharacter* GetOwnerPawn()
	{
		return GetOuterAPBWeapon() ? GetOuterAPBWeapon()->GetOwnerPawn() : nullptr;
	}
	virtual class UWorld* GetWorld() const override
	{
		return GetOuterAPBWeapon() ? GetOuterAPBWeapon()->GetWorld() : nullptr;
	}
	
	virtual void CreateMesh();
	void SetupMesh(FWeaponAttachmentData& Data);
	virtual void Init();
	virtual void Empty();

	/** attaches weapon mesh to pawn's mesh */
	virtual void AttachMeshToPawn();

	/** detaches weapon mesh from pawn */
	virtual void DetachMeshFromPawn();

	void SetPanini(class USkeletalMeshComponent* Mesh, bool bPanini);

	void UpdateMeshVisibilitys(bool bForce=false);
	void UpdateMeshVisibility(EWeaponPoint ePoint, bool bForce=false);

	void ForceMeshVisibilitys(bool bVisible, bool bFirstPerson = false);

	virtual void OnMeleeAttack();
	virtual void OnMeleeAttackEnd();
	virtual void OnFiring();	

	virtual void PlayMeleeAttackEffect(EWeaponPoint ePoint);
	virtual void StopMeleeAttackEffect(EWeaponPoint ePoint);

	virtual void PlayWeaponFireEffects();
	virtual void PlayWeaponFireEffect(EWeaponPoint ePoint);

	/** Called in network play to stop cosmetic fx (e.g. for a looping shot). */
	virtual void StopWeaponFireEffects();

	void PlayFireForceFeedback(EWeaponPoint ePoint);

	void PlayMeleeAttackCameraShake(EWeaponPoint ePoint);
	void StopMeleeAttackCameraShake(EWeaponPoint ePoint);
	void PlayFireCameraShake(EWeaponPoint ePoint);
	void StopFireCameraShake(EWeaponPoint ePoint);
	void PlayEquipCameraShake(EWeaponPoint ePoint);
	void StopEquipCameraShake(EWeaponPoint ePoint);
	void PlayReloadCameraShake(EWeaponPoint ePoint);
	void StopReloadCameraShake(EWeaponPoint ePoint);

	void PlayCameraShake(TSubclassOf <UCameraShake> ShakeClass);
	void StopCameraShake(TSubclassOf <UCameraShake> ShakeClass);

	void PlayWeaponSounds(EWeaponSoundType eSound);
	class UAudioComponent* PlayWeaponSound(EWeaponPoint ePoint, EWeaponSoundType eSound);

	float GetWeaponAnimationLength(EWeaponAnimType eAnim);
	float GetSpecificWeaponAnimLength(EWeaponPoint ePoint, EWeaponAnimType eAnim);

	float PlayWeaponAnimation(EWeaponAnimType eAnim);
	float PlaySpecificWeaponAnim(EWeaponPoint ePoint, EWeaponAnimType eAnim);
	void StopWeaponAnimation(EWeaponAnimType eAnim, bool GotoEndSection = false);
	void StopSpecificWeaponAnim(EWeaponPoint ePoint, EWeaponAnimType eAnim, bool GotoEndSection = false);

	class APBBulletBase* SpawnBullet(EWeaponPoint ePoint, const FTransform& Transform);
	class APBBulletBase* GetBullet(EWeaponPoint ePoint);
	TSubclassOf<class APBBulletBase> GetBulletClass(EWeaponPoint ePoint);

	class APBInst_MeleeBullet* SpawnMeleeBullet(EWeaponPoint ePoint, const FTransform& Transform);
	class APBInst_MeleeBullet* GetMeleeBullet(EWeaponPoint ePoint);

	void SetBulletInstigator(APawn* Pawn);
	bool ApplyTableData(const struct FPBWeaponTableData* Data);

	// return 1p mesh or 3p mesh
	class USkeletalMeshComponent* GetWeaponMesh(EWeaponPoint ePoint);
	class USkeletalMeshComponent* GetSpecificWeapMesh(EWeaponPoint ePoint, bool bFirstPerson);

	class USoundCue* GetSound(EWeaponPoint ePoint, EWeaponSoundType eSound);
	const FWeaponAnim& GetAnimation(EWeaponPoint ePoint, EWeaponAnimType eAnim);
	FName GetMuzzlePointName(EWeaponPoint ePoint);
	virtual EWeaponPoint GetFiringPoint();	
	int32 GetAttachmentDataCount();
	TArray<FWeaponAttachmentData>& GetAttachmentDataList();

protected:
	EWeaponPoint eFiringPoint;

	UPROPERTY(EditAnywhere, Category = Weapon, meta = (DisplayName="Right Hand Weapon =============================="))
	FWeaponAttachmentData AttachmentData_Right;

	UPROPERTY()
	TArray<FWeaponAttachmentData> Attachments;
};
