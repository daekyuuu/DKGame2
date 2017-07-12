// Fill out your copyright notice in the Description page of Project Settings.

#include "PBGame.h"
#include "PBSceneCaptureComponent2D.h"


#define SF this->ShowFlags

void UPBSceneCaptureComponent2D::OnRegister()
{
	Super::OnRegister();
	
	this->ShowFlags.AntiAliasing = false;
	//this->ShowFlags.BSP = false;
	this->ShowFlags.Decals = false;
	this->ShowFlags.Fog = false;
	this->ShowFlags.Landscape = false;
	this->ShowFlags.Particles = false;
	this->ShowFlags.AtmosphericFog = false;
	//this->ShowFlags.InstancedFoliage = false;
	//this->ShowFlags.InstancedGrass = false;
	this->ShowFlags.Paper2DSprites = false;
	this->ShowFlags.Bloom = false;
	this->ShowFlags.EyeAdaptation = false;
	this->ShowFlags.MotionBlur = false;
	//this->ShowFlags.SkyLighting = false;
	this->ShowFlags.AmbientOcclusion = false;
	this->ShowFlags.DynamicShadows = false;
	this->ShowFlags.AmbientCubemap = false;
	//this->ShowFlags.DistanceFieldAO = false;
	//this->ShowFlags.DistanceFieldGI = false;
	//this->ShowFlags.LightFunctions = false;
	this->ShowFlags.ReflectionEnvironment = false;
	this->ShowFlags.ScreenSpaceAO = false;
	this->ShowFlags.TexturedLightProfiles = false;
	//this->CaptureSource = ESceneCaptureSource::SCS_FinalColorLDR;

	this->UpdateShowFlags();


}

void UPBSceneCaptureComponent2D::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	APlayerController* PC = Cast<APlayerController>(GetOwner());
	if (nullptr == PC)
	{
		return;
	}
	
	auto CameraManager = PC->PlayerCameraManager;
	if (nullptr == CameraManager)
	{
		return;
	}

	auto Pawn = PC->GetPawn();

	// Exclude the owning pawn and attached actors from the capture target.
	if (Pawn)
	{
		this->HideActorComponents(Pawn);

		TArray<AActor*> OutAttachedActors;
		Pawn->GetAttachedActors(OutAttachedActors);
		for (auto A : OutAttachedActors)
		{
			this->HideActorComponents(A);
		}
	}


	this->SetWorldLocation(CameraManager->GetCameraLocation());
	this->SetWorldRotation(CameraManager->GetCameraRotation());
	this->FOVAngle = CameraManager->GetFOVAngle();

	
}
