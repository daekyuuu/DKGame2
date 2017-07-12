// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "PBGame.h"
#include "Bots/PBAIController.h"
#include "Bots/PBBot.h"
#include "Player/PBPlayerState.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Bool.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Object.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Vector.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Rotator.h"
#include "Weapons/PBWeapon.h"

APBAIController::APBAIController(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
 	BlackboardComp = ObjectInitializer.CreateDefaultSubobject<UBlackboardComponent>(this, TEXT("BlackBoardComp"));
 	
	BrainComponent = BehaviorComp = ObjectInitializer.CreateDefaultSubobject<UBehaviorTreeComponent>(this, TEXT("BehaviorComp"));	

	bWantsPlayerState = true;
	bInitializedStartPoint = false;
	bInfiniteAmmo = false;
	bInfiniteClip = true;
	bLevelBot = false;
}

void APBAIController::Possess(APawn* InPawn)
{
	Super::Possess(InPawn);

	APBBot* Bot = Cast<APBBot>(InPawn);

	// start behavior
	if (Bot && Bot->BotBehavior)
	{
		if (Bot->BotBehavior->BlackboardAsset)
		{
			BlackboardComp->InitializeBlackboard(*Bot->BotBehavior->BlackboardAsset);
		}

		EnemyKeyID = BlackboardComp->GetKeyID("Enemy");
		NeedAmmoKeyID = BlackboardComp->GetKeyID("NeedAmmo");
		HomeLocationID = BlackboardComp->GetKeyID("HomeLocation");
		HomeRotationID = BlackboardComp->GetKeyID("HomeRotation");

		//Start spot
		if (!bInitializedStartPoint)
		{
			BlackboardComp->SetValue<UBlackboardKeyType_Vector>(HomeLocationID, InPawn->GetActorLocation());
			BlackboardComp->SetValue<UBlackboardKeyType_Rotator>(HomeRotationID, InPawn->GetActorRotation());
			bInitializedStartPoint = true;
		}
		
		if (bLevelBot)
		{
			DefaultPawnClass = Bot->GetClass();
		}

		SetEnemy(nullptr);

		BehaviorComp->StartTree(*(Bot->BotBehavior));
	}
}

void APBAIController::UnPossess()
{
	Super::UnPossess();

	BehaviorComp->StopTree();
}

void APBAIController::BeginInactiveState()
{
	Super::BeginInactiveState();

	//AGameState* GameState = GetWorld()->GameState;

	//const float MinRespawnDelay = (GameState && GameState->GameModeClass) ? GetDefault<AGameMode>(GameState->GameModeClass)->MinRespawnDelay : 1.0f;

	//GetWorldTimerManager().SetTimer(TimerHandle_Respawn, this, &APBAIController::Respawn, MinRespawnDelay);	

	//float RespawnDelay = 5.f;
	//if (GetWorld() && GetWorld()->GetAuthGameMode())
	//{
	//	RespawnDelay = GetWorld()->GetAuthGameMode()->MinRespawnDelay;
	//}

	//GetWorldTimerManager().SetTimer(TimerHandle_Respawn, this, &APBAIController::Respawn, RespawnDelay, false);
}

UClass* APBAIController::GetDefaultPawnClass() const
{
	return DefaultPawnClass;
}

void APBAIController::SpawnAndPossessGhost(APawn* Killer)
{
	if (Role == ROLE_Authority)
	{
		AGameStateBase const* const GameState = GetWorld()->GetGameState();

		const float MinRespawnDelay = GameState ? GameState->GetPlayerRespawnDelay(this) : 1.0f;

		GetWorldTimerManager().SetTimer(TimerHandle_Respawn, this, &APBAIController::Respawn, MinRespawnDelay);
	}
}

void APBAIController::Respawn()
{
	if (Role == ROLE_Authority)
	{
		APBBot* MyBot = Cast<APBBot>(GetPawn());

		if (MyBot)
		{
			MyBot->SetLifeSpan(0.1f);
			
			if (!MyBot->CanRespawn())
			{
				return;
			}

			MyBot->UseRespawn();
		}
		UnPossess();
		GetWorld()->GetAuthGameMode()->RestartPlayer(this);
	}
}

void APBAIController::FindClosestEnemy(float Distance)
{
	APBBot* MyBot = Cast<APBBot>(GetPawn());
	if (MyBot == NULL || !MyBot->IsAlive())
	{
		return;
	}

	const FVector MyLoc = MyBot->GetActorLocation();
	float BestDistSq = MAX_FLT;
	APBCharacter* BestPawn = NULL;
	SetEnemy(nullptr);

	const float MaxDistSq = Distance * Distance;

	for (FConstPawnIterator It = GetWorld()->GetPawnIterator(); It; ++It)
	{
		APBCharacter* TestPawn = Cast<APBCharacter>(*It);
		if (TestPawn && TestPawn->IsAlive() && TestPawn->IsEnemyFor(this))
		{
			const float DistSq = (TestPawn->GetActorLocation() - MyLoc).SizeSquared();
			if (DistSq < BestDistSq && DistSq < MaxDistSq)
			{
				BestDistSq = DistSq;
				BestPawn = TestPawn;
			}
		}
	}

	if (BestPawn)
	{
		SetEnemy(BestPawn);
	}
}

bool APBAIController::FindClosestEnemyWithLOS(float Distance, APBCharacter* ExcludeEnemy)
{
	bool bGotEnemy = false;
	APBBot* MyBot = Cast<APBBot>(GetPawn());
	const float MaxDistSq = Distance * Distance;

	if (MyBot != NULL && MyBot->IsAlive())
	{
		const FVector MyLoc = MyBot->GetActorLocation();
		float BestDistSq = MAX_FLT;
		APBCharacter* BestPawn = NULL;

		SetEnemy(nullptr);

		for (FConstPawnIterator It = GetWorld()->GetPawnIterator(); It; ++It)
		{
			APBCharacter* TestPawn = Cast<APBCharacter>(*It);
			if (TestPawn && TestPawn != ExcludeEnemy && TestPawn->IsAlive() && TestPawn->IsEnemyFor(this))
			{
				if (HasWeaponLOSToEnemy(TestPawn, true) == true)
				{
					const float DistSq = (TestPawn->GetActorLocation() - MyLoc).SizeSquared();
					if (DistSq < BestDistSq && DistSq < MaxDistSq)
					{
						BestDistSq = DistSq;
						BestPawn = TestPawn;
					}
				}
			}
		}
		if (BestPawn)
		{
			SetEnemy(BestPawn);
			bGotEnemy = true;
		}
	}
	return bGotEnemy;
}

bool APBAIController::HasWeaponLOSToEnemy(AActor* InEnemyActor, const bool bAnyEnemy) const
{
	static FName LosTag = FName(TEXT("AIWeaponLosTrace"));
	
	APBBot* MyBot = Cast<APBBot>(GetPawn());

	bool bHasLOS = false;
	// Perform trace to retrieve hit info
	FCollisionQueryParams TraceParams(LosTag, true, GetPawn());
	TraceParams.bTraceAsyncScene = true;

	TraceParams.bReturnPhysicalMaterial = true;	
	FVector StartLocation = MyBot->GetActorLocation();	
	StartLocation.Z += GetPawn()->BaseEyeHeight; //look from eyes
	
	FHitResult Hit(ForceInit);
	const FVector EndLocation = InEnemyActor->GetActorLocation();
	GetWorld()->LineTraceSingleByChannel(Hit, StartLocation, EndLocation, COLLISION_WEAPON, TraceParams);
	if (Hit.bBlockingHit == true)
	{
		// Theres a blocking hit - check if its our enemy actor
		AActor* HitActor = Hit.GetActor();
		if (Hit.GetActor() != NULL)
		{
			if (HitActor == InEnemyActor)
			{
				bHasLOS = true;
			}
			else if (bAnyEnemy == true)
			{
				// Its not our actor, maybe its still an enemy ?
				ACharacter* HitChar = Cast<ACharacter>(HitActor);
				if (HitChar != NULL)
				{
					APBPlayerState* HitPlayerState = Cast<APBPlayerState>(HitChar->PlayerState);
					APBPlayerState* MyPlayerState = Cast<APBPlayerState>(PlayerState);
					if ((HitPlayerState != NULL) && (MyPlayerState != NULL))
					{
						if (HitPlayerState->GetTeamType() != MyPlayerState->GetTeamType())
						{
							bHasLOS = true;
						}
					}
				}
			}
		}
	}

	

	return bHasLOS;
}

void APBAIController::ShootEnemy()
{
	APBBot* MyBot = Cast<APBBot>(GetPawn());
	APBWeapon* MyWeapon = MyBot ? MyBot->GetCurrentWeapon() : NULL;
	if (MyWeapon == NULL)
	{
		return;
	}

	bool bCanShoot = false;
	APBCharacter* Enemy = GetEnemy();
	if ( Enemy && ( Enemy->IsAlive() )&& (MyWeapon->GetCurrentAmmo() > 0) && ( MyWeapon->CanFire() == true ) )
	{
		if (LineOfSightTo(Enemy, MyBot->GetActorLocation()))
		{
			bCanShoot = true;
		}
	}

	if (bCanShoot)
	{
		MyBot->StartWeaponFire();
	}
	else
	{
		MyBot->StopWeaponFire();
	}
}

void APBAIController::CheckAmmo(const class APBWeapon* Weapon)
{
	if (Weapon && BlackboardComp)
	{
		const int32 Ammo = Weapon->GetCurrentAmmo();
		const int32 MaxAmmo = Weapon->GetMaxAmmo();
		const float Ratio = (float) Ammo / (float) MaxAmmo;

		BlackboardComp->SetValue<UBlackboardKeyType_Bool>(NeedAmmoKeyID, (Ratio <= 0.1f));
	}
}

void APBAIController::SetInfiniteAmmo(bool bEnable)
{
	APBBot* MyBot = Cast<APBBot>(GetPawn());
	APBWeapon* MyWeapon = MyBot ? MyBot->GetCurrentWeapon() : NULL;
	if (MyWeapon)
	{
		MyWeapon->FillAmmo();
	}

	bInfiniteAmmo = bEnable;
}

bool APBAIController::HasInfiniteAmmo() const
{
	return bInfiniteAmmo;
}

void APBAIController::SetInfiniteClip(bool bEnable)
{
	APBBot* MyBot = Cast<APBBot>(GetPawn());
	APBWeapon* MyWeapon = MyBot ? MyBot->GetCurrentWeapon() : NULL;
	if (MyWeapon)
	{
		MyWeapon->FillAmmo();
	}

	bInfiniteClip = bEnable;
}

bool APBAIController::HasInfiniteClip() const
{
	return bInfiniteClip;
}

void APBAIController::SetEnemy(class APawn* InPawn)
{
	if (BlackboardComp)
	{
		BlackboardComp->SetValue<UBlackboardKeyType_Object>(EnemyKeyID, InPawn);
		SetFocus(InPawn);
	}
}

class APBCharacter* APBAIController::GetEnemy() const
{
	if (BlackboardComp)
	{
		return Cast<APBCharacter>(BlackboardComp->GetValue<UBlackboardKeyType_Object>(EnemyKeyID));
	}

	return NULL;
}


void APBAIController::UpdateControlRotation(float DeltaTime, bool bUpdatePawn)
{
	// Look toward focus
	FVector FocalPoint = GetFocalPoint();
	if( !FocalPoint.IsZero() && GetPawn())
	{
		FVector Direction = FocalPoint - GetPawn()->GetActorLocation();
		FRotator NewControlRotation = Direction.Rotation();
		
		NewControlRotation.Yaw = FRotator::ClampAxis(NewControlRotation.Yaw);

		SetControlRotation(NewControlRotation);

		APawn* const P = GetPawn();
		if (P && bUpdatePawn)
		{
			P->FaceRotation(NewControlRotation, DeltaTime);
		}
		
	}
}

FVector APBAIController::GetStartLocation() const
{
	return BlackboardComp->GetValue<UBlackboardKeyType_Vector>(HomeLocationID);	
}

FRotator APBAIController::GetStartRotation() const
{
	return BlackboardComp->GetValue<UBlackboardKeyType_Rotator>(HomeRotationID);
}

void APBAIController::GameHasEnded(AActor* EndGameFocus, bool bIsWinner)
{
	// Stop the behaviour tree/logic
	BehaviorComp->StopTree();

	// Stop any movement we already have
	StopMovement();

	// Cancel the repsawn timer
	GetWorldTimerManager().ClearTimer(TimerHandle_Respawn);

	// Clear any enemy
	SetEnemy(NULL);

	// Finally stop firing
	APBBot* MyBot = Cast<APBBot>(GetPawn());
	APBWeapon* MyWeapon = MyBot ? MyBot->GetCurrentWeapon() : NULL;
	if (MyWeapon == NULL)
	{
		return;
	}
	MyBot->StopWeaponFire();	
}

