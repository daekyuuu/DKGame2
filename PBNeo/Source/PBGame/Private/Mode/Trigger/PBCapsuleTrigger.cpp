// Fill out your copyright notice in the Description page of Project Settings.

#include "PBGame.h"
#include "PBCapsuleTrigger.h"


// Sets default values
APBCapsuleTrigger::APBCapsuleTrigger()
{
	PrimaryActorTick.bCanEverTick = true;

	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
	RootComponent = SceneRoot;
	RootComponent->bShouldUpdatePhysicsVolume = true;

	TriggerCapsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("TriggerCapsule"));
	TriggerCapsule->SetCollisionProfileName(TEXT("PlayerTrigger"));
	TriggerCapsule->SetupAttachment(RootComponent);
	TriggerCapsule->OnComponentBeginOverlap.AddDynamic(this, &APBCapsuleTrigger::OnOverlap);
	TriggerCapsule->OnComponentEndOverlap.AddDynamic(this, &APBCapsuleTrigger::OnUnOverlap);
}

void APBCapsuleTrigger::OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	if (Role == ROLE_Authority && OverlappedComponent && OverlappedComponent == TriggerCapsule)
	{
		TriggerBeginOverlap(OverlappedComponent, OtherActor, OtherComp, bFromSweep, SweepResult);
	}
}

void APBCapsuleTrigger::TriggerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, bool bFromSweep, const FHitResult & SweepResult)
{

}

void APBCapsuleTrigger::OnUnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (Role == ROLE_Authority && OverlappedComponent && OverlappedComponent == TriggerCapsule)
	{
		TriggerEndOverlap(OverlappedComponent, OtherActor, OtherComp);
	}
}

void APBCapsuleTrigger::TriggerEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherCompt)
{

}
