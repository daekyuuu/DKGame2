#pragma once

#include "PBCharacterData.generated.h"


struct FWeaponRecoilData;

static FName BoneName_LFoot(TEXT("Bip001-L-Foot"));
static FName BoneName_RFoot(TEXT("Bip001-R-Foot"));

USTRUCT(BlueprintType)
struct FCharacterVoice
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditDefaultsOnly)
	EPBVoiceType VoiceType;

	UPROPERTY(EditDefaultsOnly)
	USoundCue* Sound;
};

USTRUCT(BlueprintType)
struct FCharacterData
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditDefaultsOnly, Category = CharacterStat)
	int32 ItemID;

	UPROPERTY(EditDefaultsOnly, Category = CharacterStat)
	EPBCharacterGender Gender;

	UPROPERTY(EditDefaultsOnly, Category = "Movement (User Settings)")
	float CrouchedWalkSpeedModifier;

	UPROPERTY(EditDefaultsOnly, Category = "Movement (User Settings)")
	float WalkSpeedModifier;

	UPROPERTY(EditDefaultsOnly, Category = "Movement (User Settings)")
	float RunSpeedModifier;

	/** modifier for max movement speed */
	UPROPERTY(EditDefaultsOnly, Category = "Movement (User Settings)")
	float SprintingSpeedModifier;

	UPROPERTY(EditDefaultsOnly, Category = "Movement (User Settings)")
	float DirectionSpeedModifier;

	/** modifier for max movement speed */
	UPROPERTY(EditDefaultsOnly, Category = "Movement (User Settings)")
	float ZoomingSpeedModifier;

	FCharacterData()
	{
		ItemID = 0;
		Gender = EPBCharacterGender::Male;
		CrouchedWalkSpeedModifier = 0.f;
		WalkSpeedModifier = 0.f;
		RunSpeedModifier = 0.f;
		SprintingSpeedModifier = 0.3f;
		DirectionSpeedModifier = 0.f;
		ZoomingSpeedModifier = -0.5f;
	}
};

USTRUCT(BlueprintType)
struct FBoneImpulseReceptor
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditDefaultsOnly, Category = DeathRagdoll)
	FName BoneName;

	UPROPERTY(EditDefaultsOnly, Category = DeathRagdoll)
	float ImpulseReceptionWeight;

	FBoneImpulseReceptor()
	{
		BoneName = NAME_None;
		ImpulseReceptionWeight = 1.f;
	}

	FBoneImpulseReceptor(FName Arg_BoneName, float Arg_ImpulseReceptionWeight)
	{
		BoneName = Arg_BoneName;
		ImpulseReceptionWeight = Arg_ImpulseReceptionWeight;
	}
};

USTRUCT(BlueprintType)
struct FPBPlayAnimData
{
	GENERATED_BODY()

	UPROPERTY()
	bool bPlay;

	UPROPERTY()
	UAnimMontage* AnimMontage;

	UPROPERTY()
	FName SectionName;

	UPROPERTY()
	int32 RepCounter; //for replicate

	FPBPlayAnimData()
	{
		bPlay = true;
		AnimMontage = nullptr;
		RepCounter = 0;
	}
};