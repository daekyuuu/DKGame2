// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "PBGame.h"
#include "Bots/BTTask_FindPickup.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyAllTypes.h"
#include "Bots/PBAIController.h"
#include "Bots/PBBot.h"
#include "Pickups/PBPickupAmmo.h"
#include "Weapons/PBWeap_Instant.h"

UBTTask_FindPickup::UBTTask_FindPickup(const FObjectInitializer& ObjectInitializer) 
	: Super(ObjectInitializer)
{
}

EBTNodeResult::Type UBTTask_FindPickup::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	APBAIController* MyController = Cast<APBAIController>(OwnerComp.GetAIOwner());
	APBBot* MyBot = MyController ? Cast<APBBot>(MyController->GetPawn()) : NULL;
	if (MyBot == NULL)
	{
		return EBTNodeResult::Failed;
	}

	APBGameMode* GameMode = MyBot->GetWorld()->GetAuthGameMode<APBGameMode>();
	if (GameMode == NULL)
	{
		return EBTNodeResult::Failed;
	}

	const FVector MyLoc = MyBot->GetActorLocation();
	APBPickupAmmo* BestPickup = NULL;
	float BestDistSq = MAX_FLT;

	for (int32 i = 0; i < GameMode->LevelPickups.Num(); ++i)
	{
		APBPickupAmmo* AmmoPickup = Cast<APBPickupAmmo>(GameMode->LevelPickups[i]);
		if (AmmoPickup && AmmoPickup->IsForWeapon(APBWeap_Instant::StaticClass()) && AmmoPickup->CanPickup(MyBot))
		{
			const float DistSq = (AmmoPickup->GetActorLocation() - MyLoc).SizeSquared();
			if (BestDistSq == -1 || DistSq < BestDistSq)
			{
				BestDistSq = DistSq;
				BestPickup = AmmoPickup;
			}
		}
	}

	if (BestPickup)
	{
		OwnerComp.GetBlackboardComponent()->SetValue<UBlackboardKeyType_Vector>(BlackboardKey.GetSelectedKeyID(), BestPickup->GetActorLocation());
		return EBTNodeResult::Succeeded;
	}

	return EBTNodeResult::Failed;
}
