// Fill out your copyright notice in the Description page of Project Settings.

#include "PBGame.h"
#include "PBMiniMapCanvas.h"


// Sets default values
APBMiniMapCanvas::APBMiniMapCanvas()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	SetActorHiddenInGame(true);

	MiniMapExtent = CreateDefaultSubobject<UBoxComponent>(TEXT("FirstPersonCamera"));
	SetRootComponent(MiniMapExtent);
	MiniMapExtent->SetBoxExtent(FVector(100, 100, 0), false);
	MiniMapExtent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	MiniMapExtent->SetIsReplicated(false);
}

FVector2D APBMiniMapCanvas::GetExtent()
{
	FVector ScaledExtent3D = MiniMapExtent->GetScaledBoxExtent();
	return FVector2D(ScaledExtent3D.X, ScaledExtent3D.Y);
}
