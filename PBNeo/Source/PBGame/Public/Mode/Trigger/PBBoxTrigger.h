// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "PBBoxTrigger.generated.h"

UCLASS()
class PBGAME_API APBBoxTrigger : public AActor
{

	GENERATED_BODY()
	
public:	

	APBBoxTrigger();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Trigger)
	USceneComponent* SceneRoot;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Trigger)
	class UBoxComponent* TriggerBox;

	UFUNCTION()
	void OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	virtual void TriggerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, bool bFromSweep, const FHitResult & SweepResult);

	UFUNCTION()
	void OnUnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	virtual void TriggerEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherCompt);

};
