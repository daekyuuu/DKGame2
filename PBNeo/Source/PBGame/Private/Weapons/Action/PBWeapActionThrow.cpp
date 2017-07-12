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

		// �� ������ ���� �̿ϼ�. ��� ���⸦ ���� �� �ְ� �ϴ°� ������ǥ. 
		// ������ ������ ���� �� �ִ� ���� Ÿ���� ������ �ְ� �� ���� Ŭ���� ������ ������ ������ ó���Ѵ�.
		// Weap_Throw, Weap_ThrowingKnife ��
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
