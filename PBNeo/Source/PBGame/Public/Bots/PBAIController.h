// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#pragma once
#include "AIController.h"
#include "PBAIController.generated.h"

class UBehaviorTreeComponent;
class UBlackboardComponent;

UCLASS(config=Game)
class APBAIController : public AAIController
{
	GENERATED_UCLASS_BODY()

private:
	UPROPERTY(transient)
	UBlackboardComponent* BlackboardComp;

	/* Cached BT component */
	UPROPERTY(transient)
	UBehaviorTreeComponent* BehaviorComp;
public:

	// Begin AController interface
	virtual void GameHasEnded(class AActor* EndGameFocus = NULL, bool bIsWinner = false) override;
	virtual void Possess(class APawn* InPawn) override;
	virtual void UnPossess() override;
	virtual void BeginInactiveState() override;
	// End APlayerController interface

	UClass* GetDefaultPawnClass() const;

	void Respawn();

	void CheckAmmo(const class APBWeapon* Weapon);

	void SetInfiniteAmmo(bool bEnable);
	bool HasInfiniteAmmo() const;

	void SetInfiniteClip(bool bEnable);
	bool HasInfiniteClip() const;

	void SetEnemy(class APawn* InPawn);

	class APBCharacter* GetEnemy() const;

	/* If there is line of sight to current enemy, start firing at them */
	UFUNCTION(BlueprintCallable, Category=Behavior)
	void ShootEnemy();

	/* Finds the closest enemy and sets them as current target */
	UFUNCTION(BlueprintCallable, Category=Behavior)
	void FindClosestEnemy(float Distance);

	UFUNCTION(BlueprintCallable, Category = Behavior)
	bool FindClosestEnemyWithLOS(float Distance, APBCharacter* ExcludeEnemy);
		
	bool HasWeaponLOSToEnemy(AActor* InEnemyActor, const bool bAnyEnemy) const;

	// Begin AAIController interface
	/** Update direction AI is looking based on FocalPoint */
	virtual void UpdateControlRotation(float DeltaTime, bool bUpdatePawn = true) override;
	// End AAIController interface

	// Call only on server
	void SpawnAndPossessGhost(APawn* Killer);

	FVector GetStartLocation() const;
	FRotator GetStartRotation() const;

	// level 에 배치되어 있는 봇
	bool bLevelBot;

	// level 에 배치되어 있는 봇 character type
	TSubclassOf<class APawn> DefaultPawnClass;

protected:
	/** infinite ammo cheat */
	UPROPERTY(Transient)
	uint8 bInfiniteAmmo : 1;

	/** infinite clip cheat */
	UPROPERTY(Transient)
	uint8 bInfiniteClip : 1;

protected:
	// Check of we have LOS to a character
	bool LOSTrace(APBCharacter* InEnemyChar) const;

	int32 EnemyKeyID;
	int32 NeedAmmoKeyID;
	int32 HomeLocationID;
	int32 HomeRotationID;

	/** Handle for efficient management of Respawn timer */
	FTimerHandle TimerHandle_Respawn;


	bool bInitializedStartPoint;
public:
	/** Returns BlackboardComp subobject **/
	FORCEINLINE UBlackboardComponent* GetBlackboardComp() const { return BlackboardComp; }
	/** Returns BehaviorComp subobject **/
	FORCEINLINE UBehaviorTreeComponent* GetBehaviorComp() const { return BehaviorComp; }
};
