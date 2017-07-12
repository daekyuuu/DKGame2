// Fill out your copyright notice in the Description page of Project Settings.

#include "PBGame.h"
#include "PBWeapActionThrow.h"
#include "Weapons/PBWeap_Throw.h"

UPBWeapActionThrow::UPBWeapActionThrow()
{

}

void UPBWeapActionThrow::BeginState(EFireTrigger eFireTrigger /*= EFireTrigger::Fire1*/)
{
	Super::BeginState(eFireTrigger);

	StartThrow();
}

void UPBWeapActionThrow::EndState(EFireTrigger eFireTrigger /*= EFireTrigger::Fire1*/)
{
	Super::EndState(eFireTrigger);

	EndThrow();
}

void UPBWeapActionThrow::StartThrow()
{
	APBCharacter* MyPawn = GetOwnerPawn();
	if (MyPawn)
	{
		const bool bLocallyControlled = MyPawn->IsLocallyControlled();

		// 이 로직은 아직 미완성. 모든 무기를 던질 수 있게 하는게 최종목표. 
		// 하지만 지금은 던질 수 있는 무기 타입이 정해져 있고 그 무기 클래스 내에서 던지는 로직을 처리한다.
		// Weap_Throw, Weap_ThrowingKnife 등
		MyPawn->OnStartFireTrigger1();
	}
}

void UPBWeapActionThrow::EndThrow()
{
	APBCharacter* MyPawn = GetOwnerPawn();

	if (MyPawn)
	{
		const bool bLocallyControlled = MyPawn->IsLocallyControlled();
		
		MyPawn->OnEndFireTrigger1();
	}
}
