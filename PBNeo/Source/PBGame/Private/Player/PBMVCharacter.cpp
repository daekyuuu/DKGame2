// Fill out your copyright notice in the Description page of Project Settings.

#include "PBGame.h"
#include "PBMVCharacter.h"
#include "PBItemTableManager.h"
#include "PBWeapon.h"

// Sets default values
APBMVCharacter::APBMVCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	if (CameraComp)
	{
		CameraComp->DestroyComponent();
	}

	TeamType = EPBTeamType::Alpha;

}

// Called when the game starts or when spawned
void APBMVCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APBMVCharacter::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

void APBMVCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	UpdateMeshVisibility(true);	
}

// Called to bind functionality to input
void APBMVCharacter::SetupPlayerInputComponent(class UInputComponent* InInputComponent)
{
	Super::SetupPlayerInputComponent(InInputComponent);

}

void APBMVCharacter::SetMVCharacter(TSubclassOf<class APBCharacter> CharacterClass)
{
	if (CharacterClass)
	{
		auto CDO = Cast<APBCharacter>(CharacterClass->GetDefaultObject());
		if (CDO)
		{
			SetMVCharacter(CDO);
		}
	}
}

void APBMVCharacter::SetMVCharacter(class APBCharacter* CharacterCDO)
{
	if (nullptr == CharacterCDO)
	{
		return;
	}

	USkeletalMeshComponent* NewMeshComp = CharacterCDO->GetMesh3P();
	if (nullptr == NewMeshComp)
	{
		return;
	}

	auto MyMesh = GetMesh3P();
	if (nullptr == MyMesh)
	{
		return;
	}

	// First, Switch the skeletal mesh
	if (NewMeshComp->SkeletalMesh)
	{
		MyMesh->SetSkeletalMesh(NewMeshComp->SkeletalMesh);
	}

	// Second, Switch the materials
	auto NewMaterials = NewMeshComp->GetMaterials();
	const int NewMaterialsNum = NewMaterials.Num();
	for (int i = 0; i < NewMaterialsNum; ++i)
	{
		MyMesh->SetMaterial(i, NewMaterials[i]);
	}

	auto& MyCharacterData = GetCharaterData();
	auto& CDOCharacterData = CharacterCDO->GetCharaterData();
	// Set the MVCharacter's gender to prepare weapon animations.
	MyCharacterData.Gender = CDOCharacterData.Gender;

	SetAnimClassBy(CDOCharacterData.Gender);

	//NotifyMeshHasChanged(true);

}

void APBMVCharacter::SetAnimClassBy(EPBCharacterGender Gender)
{
	if (GetMesh3P() == nullptr)
	{
		return;
	}

	switch (Gender)
	{
	case EPBCharacterGender::Male:
		GetMesh3P()->SetAnimInstanceClass(AnimClass_Male);
		break;
	case EPBCharacterGender::Female:
		GetMesh3P()->SetAnimInstanceClass(AnimClass_Female);
		break;
	}
}

void APBMVCharacter::SetMVWeapon(int32 Id)
{
	auto TM = UPBItemTableManager::Get(this);
	if (nullptr == TM)
	{
		return;
	}

	TM->ApplyWeapItemToPawn(this, Id, true, 0);

}

void APBMVCharacter::SetMVWeapon(TSubclassOf<class APBWeapon> WeapClass)
{
	if (WeapClass)
	{
		auto CDO = Cast<APBWeapon>(WeapClass->GetDefaultObject());
		if (CDO)
		{
			SetMVWeapon(CDO->GetItemID());
		}
	}
	
}