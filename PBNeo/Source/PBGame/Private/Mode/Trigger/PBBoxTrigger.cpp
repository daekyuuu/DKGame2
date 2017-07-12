// Fill out your copyright notice in the Description page of Project Settings.

#include "PBGame.h"
#include "PBBoxTrigger.h"


// Sets default values
APBBoxTrigger::APBBoxTrigger()
{
	PrimaryActorTick.bCanEverTick = true;

	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
	RootComponent = SceneRoot;
	RootComponent->bShouldUpdatePhysicsVolume = true;

	TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
	TriggerBox->SetCollisionProfileName(TEXT("PlayerTrigger"));
	TriggerBox->SetupAttachment(RootComponent);
	TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &APBBoxTrigger::OnOverlap);
	TriggerBox->OnComponentEndOverlap.AddDynamic(this, &APBBoxTrigger::OnUnOverlap);
}

void APBBoxTrigger::OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	if (Role == ROLE_Authority && OverlappedComponent && OverlappedComponent == TriggerBox)
	{
		TriggerBeginOverlap(OverlappedComponent, OtherActor, OtherComp, bFromSweep, SweepResult);
	}
}

void APBBoxTrigger::TriggerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, bool bFromSweep, const FHitResult & SweepResult)
{
}

void APBBoxTrigger::OnUnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (Role == ROLE_Authority && OverlappedComponent && OverlappedComponent == TriggerBox)
	{
		TriggerEndOverlap(OverlappedComponent, OtherActor, OtherComp);
	}
}

void APBBoxTrigger::TriggerEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherCompt)
{

}

