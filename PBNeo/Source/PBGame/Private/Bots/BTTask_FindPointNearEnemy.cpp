// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "PBGame.h"
#include "Bots/BTTask_FindPointNearEnemy.h"
#include "Bots/PBAIController.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyAllTypes.h"
#include "PBGameplayStatics.h"

UBTTask_FindPointNearEnemy::UBTTask_FindPointNearEnemy(const FObjectInitializer& ObjectInitializer) 
	: Super(ObjectInitializer)
{
	PointDistanceToEnemy = 0.f;
	RandomRangeFromFindPoint = 0.f;
}

// 적 가까운 곳으로 Destination 설정
// (적 앞쪽 600.0f cm 지점에서 200 cm 반경 안의 적당한 지점을 찾는다)
EBTNodeResult::Type UBTTask_FindPointNearEnemy::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	APBAIController* MyController = Cast<APBAIController>(OwnerComp.GetAIOwner());
	if (MyController == NULL)
	{
		return EBTNodeResult::Failed;
	}
	
	APawn* MyBot = MyController->GetPawn();
	APBCharacter* Enemy = MyController->GetEnemy();
	if (Enemy && MyBot)
	{
		const float SearchRadius = RandomRangeFromFindPoint;
		const FVector SearchOrigin = Enemy->GetActorLocation() + PointDistanceToEnemy * (MyBot->GetActorLocation() - Enemy->GetActorLocation()).GetSafeNormal();
		const FVector Loc = UNavigationSystem::GetRandomReachablePointInRadius(MyController, SearchOrigin, SearchRadius);

		//UPBGameplayStatics::ShowDebugSphere(MyBot, SearchOrigin, SearchRadius, FColor::Green, 10.f);
		DrawDebugPoint(GetWorld(), Loc, 10.f, FColor::Red, false, 10.f);

		if (Loc != FVector::ZeroVector)
		{
			OwnerComp.GetBlackboardComponent()->SetValue<UBlackboardKeyType_Vector>(BlackboardKey.GetSelectedKeyID(), Loc);
			return EBTNodeResult::Succeeded;
		}
	}

	return EBTNodeResult::Failed;
}
