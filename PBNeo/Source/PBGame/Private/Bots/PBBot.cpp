// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "PBGame.h"
#include "Bots/PBBot.h"
#include "Bots/PBAIController.h"

APBBot::APBBot(const FObjectInitializer& ObjectInitializer) 
	: Super(ObjectInitializer)
{
	AIControllerClass = APBAIController::StaticClass();

	UpdateMeshVisibility(true);

	bUseControllerRotationYaw = true;
	Health = 100.f;
	DamagedTime = 0.f;
	CurrentDamagedTime = 0.f;
	DamagedSpeedMultiplier = 1.f;
	RemainRespawnCount = 0;
}


void APBBot::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(APBBot, RemainRespawnCount);
}

void APBBot::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	CurrentDamagedTime = FMath::FInterpTo(CurrentDamagedTime, 0.f, DeltaSeconds, 20);
}

void APBBot::PostInitializeComponents()
{
	Super::PostInitializeComponents();

}

float APBBot::TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, class AActor* DamageCauser)
{
	if (Health <= 0.f)
	{
		return 0.f;
	}

	// Modify based on game rules.
	APBGameMode* const Game = GetWorld()->GetAuthGameMode<APBGameMode>();
	Damage = Game ? Game->ModifyDamage(Damage, this, DamageEvent, EventInstigator, DamageCauser) : 0.f;

	float ActualDamage = ACharacter::TakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser);
	if (ActualDamage > 0.f)
	{
		Health -= ActualDamage;

		if (Health <= 0)
		{
			Die(ActualDamage, DamageEvent, EventInstigator, DamageCauser);
		}
		else
		{
			PlayHit(ActualDamage, DamageEvent, EventInstigator ? EventInstigator->GetPawn() : NULL, DamageCauser);
		}

		MakeNoise(1.0f, EventInstigator ? EventInstigator->GetPawn() : this);
	}

	// юс╫ц
	const float HalfDamageTime = DamagedTime * 0.5f;
	CurrentDamagedTime = (CurrentDamagedTime > HalfDamageTime) ? DamagedTime + HalfDamageTime : DamagedTime;

	return ActualDamage;
}

float APBBot::ApplyDamagedSpeed(float InSpeed) const
{
	const float SpeedRate = (CurrentDamagedTime == 0.f || DamagedTime == 0.f) ? 1.f : (1.0f - CurrentDamagedTime / DamagedTime);

	InSpeed *= SpeedRate;

	//print(FString::Printf(TEXT("OutSpeed: %f, SpeedRate:%f"), InSpeed, SpeedRate));

	return InSpeed;
}

bool APBBot::IsFirstPersonView() const
{
	return false;
}

void APBBot::FaceRotation(FRotator NewRotation, float DeltaTime)
{
	FRotator CurrentRotation = FMath::RInterpTo(GetActorRotation(), NewRotation, DeltaTime, 8.0f);

	Super::FaceRotation(CurrentRotation, DeltaTime);
}

void APBBot::Respawn()
{
	APBAIController* PC = Cast<APBAIController>(GetController());
	if (PC)
	{
		// set respawn
		PC->SpawnAndPossessGhost(LastTakeHitInfo.PawnInstigator.Get());
	}
}

bool APBBot::CanRespawn()
{
	return RemainRespawnCount > 0;
}

void APBBot::UseRespawn()
{
	RemainRespawnCount--;
}
