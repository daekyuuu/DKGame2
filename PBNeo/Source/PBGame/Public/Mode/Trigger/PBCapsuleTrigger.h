// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "PBCapsuleTrigger.generated.h"

UCLASS()
class PBGAME_API APBCapsuleTrigger : public AActor
{

	GENERATED_BODY()
	
public:	

	APBCapsuleTrigger();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Trigger)
	USceneComponent* SceneRoot;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Trigger)
	class UCapsuleComponent* TriggerCapsule; 
	
	UFUNCTION()
	void OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	virtual void TriggerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp ,bool bFromSweep, const FHitResult & SweepResult);

	UFUNCTION()
	void OnUnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	virtual void TriggerEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherCompt);

};
