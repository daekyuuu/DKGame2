// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Mode/Trigger/PBCapsuleTrigger.h"
#include "PBTrigger_BombSite.generated.h"

/*
 *
 */
UENUM(BlueprintType)
enum class EPBBombSiteSign : uint8
{
	A UMETA(DisplayName = "A"),
	B UMETA(DisplayName = "B"),
	Max
};

FString EPBBombSiteSignAsString(EPBBombSiteSign BombSiteSign);

USTRUCT()
struct FPBBombSiteData
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, Category = BombSite)
	TSubclassOf<class UPBBombSiteIndicatorImageWidget> IndicatorImageWidgetClass;

	UPROPERTY(EditAnywhere, Category = BombSite)
	EPBBombSiteSign SiteSign;
};

/**
 * 
 */
UCLASS()
class PBGAME_API APBTrigger_BombSite : public APBCapsuleTrigger
{

	GENERATED_BODY()
	
public:

	APBTrigger_BombSite();

	void Reset() override;

	void TriggerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, bool bFromSweep, const FHitResult & SweepResult) override;
	
	void TriggerEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherCompt) override;

	// Call only on server
	void NotifyBombPlanted();

	UFUNCTION(BlueprintCallable, Category = "BombSite")
	TSubclassOf<UPBBombSiteIndicatorImageWidget> GetIndicatorImageWidgetClass() const;

	UFUNCTION(BlueprintCallable, Category = "BombSite")
	EPBBombSiteSign GetSiteSign() const;

protected:
	UPROPERTY(Transient, ReplicatedUsing = OnRep_ToBeHidden)
	bool bToBeHidden;
	
	UFUNCTION()
	void OnRep_ToBeHidden();

public:
	UFUNCTION(Reliable, Server, WithValidation)
	void ServerSetIsPlantedBomb(bool bPlantedBomb);

	bool IsPlantedBomb() const;

protected:
	UPROPERTY(Transient, ReplicatedUsing = OnRep_bIsPlantedBomb)
	bool bIsPlantedBomb;

	UFUNCTION()
	void OnRep_bIsPlantedBomb();

	/** BombSite Data */
	UPROPERTY(EditAnywhere, Category = Config)
	FPBBombSiteData BombSiteConfig;
};
