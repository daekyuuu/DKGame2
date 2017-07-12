// Fill out your copyright notice in the Description page of Project Settings.

#include "PBGame.h"
#include "PBSphereTrigger.h"


// Sets default values
APBSphereTrigger::APBSphereTrigger()
{
	PrimaryActorTick.bCanEverTick = true;

	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
	RootComponent = SceneRoot;
	RootComponent->bShouldUpdatePhysicsVolume = true;

	TriggerSphere = CreateDefaultSubobject<USphereComponent>(TEXT("TriggerSphere"));
	TriggerSphere->SetCollisionProfileName(TEXT("PlayerTrigger"));
	TriggerSphere->SetupAttachment(RootComponent);
	TriggerSphere->OnComponentBeginOverlap.AddDynamic(this, &APBSphereTrigger::OnOverlap);
	TriggerSphere->OnComponentEndOverlap.AddDynamic(this, &APBSphereTrigger::OnUnOverlap);
}

void APBSphereTrigger::OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	if (Role == ROLE_Authority && OverlappedComponent && OverlappedComponent == TriggerSphere)
	{
		TriggerBeginOverlap(OverlappedComponent, OtherActor, OtherComp, bFromSweep, SweepResult);
	}
}

void APBSphereTrigger::TriggerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, bool bFromSweep, const FHitResult & SweepResult)
{

}

void APBSphereTrigger::OnUnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (Role == ROLE_Authority && OverlappedComponent && OverlappedComponent == TriggerSphere)
	{
		TriggerEndOverlap(OverlappedComponent, OtherActor, OtherComp);
	}
}

void APBSphereTrigger::TriggerEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherCompt)
{

}
