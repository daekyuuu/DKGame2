// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Weapons/Action/PBWeapActionFiring.h"
#include "PBWeapActionFiringBurst.generated.h"

/**
 * 
 */
UENUM(BlueprintType)
enum class EWeapBurstMode : uint8
{
	Auto,			// ��Ƽ� �ѹ� ���� ��� �߻�
	SemiAuto,		// ��Ƽ� �ѹ� ���� ����
};

UCLASS(meta=(DisplayName="FiringBurst"))
class PBGAME_API UPBWeapActionFiringBurst : public UPBWeapActionFiring
{
	GENERATED_BODY()
	
	
public:
	UPBWeapActionFiringBurst();

	virtual bool StartFire(EFireTrigger eFireTrigger) override;
	virtual void EndFire(EFireTrigger eFireTrigger) override;

protected:

	UPROPERTY(EditDefaultsOnly, Category = Config)
	int32 SemiAutoBurstCount;

	EWeapBurstMode BurstMode;
	
};
