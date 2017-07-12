// Fill out your copyright notice in the Description page of Project Settings.

#include "PBGame.h"
#include "PBWeapFanTrace.h"
#include "Weapons/PBWeapon.h"

UPBWeapFanTrace::UPBWeapFanTrace()
{
	TraceAngleSegment = 7;
	TraceAngleRange = 70;
}


bool UPBWeapFanTrace::TraceHit(APBWeapon* Weapon, const FVector& StartTrace, const FVector& ShootDir, float Range, EAttackDirType AttackDirType, FHitResult& OutHit)
{
	return FanTraceHit(Weapon, StartTrace, ShootDir, Range, AttackDirType, OutHit);
}

bool UPBWeapFanTrace::FanTraceHit(APBWeapon* Weapon, const FVector& StartTrace, const FVector& ShootDir, float Range, EAttackDirType AttackDirType, FHitResult& OutHit)
{
	DebugLineNumber = 0;

	// first Center
	LineTraceHit(Weapon, StartTrace, ShootDir, Range, OutHit);

	// forward 방향은 일단 범위 체크 안하는걸로
	if (OutHit.bBlockingHit || AttackDirType == EAttackDirType::FORWARD)
		return OutHit.bBlockingHit;

	//////////////////////////////////////////////////////////////////////////
	// next fan shape
	const int32 AngleSegment = TraceAngleSegment;
	const int32 AngleDeviation = TraceAngleRange / TraceAngleSegment;
	const float HalfAngleStart = AngleSegment * AngleDeviation * 0.5f;
	float RotateZ = HalfAngleStart;
	float RotateX = 0.f;

	bool bHit = false;

	for (int i = 0; i < AngleSegment + 1; ++i)
	{
		const int32 Deviation = (i * AngleDeviation);

		GetCalcTraceRotate(AttackDirType, HalfAngleStart, Deviation, RotateZ, RotateX);

		FVector OutAdjustedDir = ShootDir;
		AdjustTraceDir(ShootDir, RotateZ, RotateX, OutAdjustedDir);

		LineTraceHit(Weapon, StartTrace, OutAdjustedDir, Range, OutHit);

		bHit |= OutHit.bBlockingHit;

		// 최초 한번 hit 되면 바로 종료한다.(일단 지금은 그렇다)
		if (bHit)
			break;
	}

	return bHit;
}

void UPBWeapFanTrace::AdjustTraceDir(const FVector& InShootDir, float InRotateZ, float InRotateX, FVector& OutAdjustedDir)
{
	FMatrix Mat = FRotationMatrix(InShootDir.Rotation());
	FVector const DirZ = Mat.GetUnitAxis(EAxis::Z);
	FVector const DirX = Mat.GetUnitAxis(EAxis::X);

	OutAdjustedDir = InShootDir.RotateAngleAxis(InRotateZ, DirZ);
	OutAdjustedDir = OutAdjustedDir.RotateAngleAxis(InRotateX, DirX);
}

void UPBWeapFanTrace::GetCalcTraceRotate(EAttackDirType AttackDirType, float AngleStart, float AngleDeviation, float& OutRotateZ, float& OutRotateX)
{
	// 각도 : 정면은 0도, 정면에서 왼쪽방향은(음수), 오른쪽 방향은(양수)

	switch (AttackDirType)
	{
	case EAttackDirType::FORWARD:
	case EAttackDirType::L_To_R:
		OutRotateZ = -AngleStart + AngleDeviation;
		OutRotateX = 0.f;
		break;
	case EAttackDirType::R_To_L:
		OutRotateZ = AngleStart - AngleDeviation;
		OutRotateX = 0.f;
		break;
	case EAttackDirType::T_To_B:
		OutRotateZ = -AngleStart + AngleDeviation;
		OutRotateX = -90.f;
		break;
	case EAttackDirType::B_To_T:
		OutRotateZ = -AngleStart + AngleDeviation;
		OutRotateX = 90.f;
		break;
	case EAttackDirType::LT_To_RB:
		OutRotateZ = -AngleStart + AngleDeviation;
		OutRotateX = -45.f;
		break;
	case EAttackDirType::LB_To_RT:
		OutRotateZ = -AngleStart + AngleDeviation;
		OutRotateX = 45.f;
		break;
	case EAttackDirType::RT_To_LB:
		OutRotateZ = AngleStart - AngleDeviation;
		OutRotateX = 45.f;
		break;
	case EAttackDirType::RB_To_LT:
		OutRotateZ = AngleStart - AngleDeviation;
		OutRotateX = -45.f;
		break;
	}
}
