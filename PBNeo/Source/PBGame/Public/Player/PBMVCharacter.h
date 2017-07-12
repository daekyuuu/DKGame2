// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Character.h"
#include "PBMVCharacter.generated.h"


/**
* A character class to use in the lobby as a model view.
*/
UCLASS()
class PBGAME_API APBMVCharacter : public APBCharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	APBMVCharacter();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	virtual void PostInitializeComponents() override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* InInputComponent) override;


	void SetMVCharacter(class APBCharacter* CharacterCDO);

	UFUNCTION(BlueprintCallable, Category = "LobbyCharacter")
	void SetMVCharacter(TSubclassOf<class APBCharacter> CharacterClass);

	void SetMVWeapon(int32 id);

	UFUNCTION(BlueprintCallable, Category = "LobbyCharacter")
	void SetMVWeapon(TSubclassOf<class APBWeapon> WeapClass);

private:
	void SetAnimClassBy(EPBCharacterGender Gender);

protected:

	UFUNCTION(BlueprintImplementableEvent, Category = "LobbyCharacter")
	void NotifyMeshHasChanged(bool bPlayAnimation);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Animation)
	class UAnimMontage* AnimMeshHasChanged;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Animation)
	class TSubclassOf<UAnimInstance> AnimClass_Male;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Animation)
	class TSubclassOf<UAnimInstance> AnimClass_Female;	

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LobbyCharacter")
	EPBTeamType TeamType;
};
