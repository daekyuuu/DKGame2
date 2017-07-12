// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Player/PBCharacter.h"
#include "PBBot.generated.h"

UCLASS()
class APBBot : public APBCharacter
{
	GENERATED_UCLASS_BODY()

	UPROPERTY(EditAnywhere, Category=Behavior)
	class UBehaviorTree* BotBehavior;

	virtual void Tick(float DeltaSeconds) override;

	virtual void PostInitializeComponents() override;
	virtual float TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, class AActor* DamageCauser) override;
	virtual float ApplyDamagedSpeed(float InSpeed) const override;

	virtual bool IsFirstPersonView() const override;

	virtual void FaceRotation(FRotator NewRotation, float DeltaTime = 0.f) override;

	virtual void Respawn() override;

public:
	virtual bool CanRespawn();
	void UseRespawn();

protected:

	UPROPERTY(EditDefaultsOnly, Category = Config)
	float DamagedSpeedMultiplier;

	UPROPERTY(EditDefaultsOnly, Category = Config)
	float DamagedTime;

	UPROPERTY(Transient)
	float CurrentDamagedTime;

	UPROPERTY(EditAnywhere, Replicated, Category = Config)
	int32 RemainRespawnCount;
};