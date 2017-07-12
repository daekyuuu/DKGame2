// Fill out your copyright notice in the Description page of Project Settings.

#include "PBGame.h"
#include "PBLobbyTourGuide.h"
#include "PBLobbySpectatorPawn.h"
#include "PBLobby.h"
#include "Curves/CurveFloat.h"
#include "PBLobbyLevelScriptActor.h"

UPBLobbyTourGuide::UPBLobbyTourGuide()
{
	StopTour();

}

void UPBLobbyTourGuide::Init(class APlayerController* Controller, class UPBLobby* LobbyInst)
{
	Reset();

	if (nullptr == Controller)
	{
		return;
	}

	OwningPlayer = Controller;

	if (nullptr == LobbyInst)
	{
		return;
	}
	OwningLobby = LobbyInst;

	auto World = OwningPlayer->GetWorld();

	if (nullptr == World)
	{
		return;
	}

	bIsPlaying = false;

	CacheCamsFromLevel(World);
	CachePostProcessVolumeFromLevel(World);

	StartTourFromTo(FLobbyCamName::None, FLobbyCamName::MainMenu);
}


void UPBLobbyTourGuide::Reset()
{
	StopTour();

	PPV = nullptr;
	OwningPlayer = nullptr;
	OwningLobby = nullptr;
	CamOfEachPage.Empty();
}

void UPBLobbyTourGuide::Tick(float DeltaTime)
{
	if (nullptr == OwningLobby)
	{
		return;
	}

	if (false == bIsPlaying)
	{
		return;
	}

	if (OwningLobby->InterpCurve && CurrCamName != FLobbyCamName::None)
	{
		UpdateLobbyTourWithCurve(DeltaTime, OwningLobby->InterpCurve);
	}
	// If the Current cam is not specified, then use the default interpolation function.
	else
	{
		UpdateLobbyTour(DeltaTime);
	}
}

bool UPBLobbyTourGuide::IsTickable() const
{
	return true;
}

bool UPBLobbyTourGuide::IsTickableInEditor() const
{
	return false;
}

bool UPBLobbyTourGuide::IsTickableWhenPaused() const
{
	return false;
}

TStatId UPBLobbyTourGuide::GetStatId() const
{
	return TStatId();
}

UWorld* UPBLobbyTourGuide::GetWorld() const
{
	if (OwningPlayer)
	{
		return OwningPlayer->GetWorld();
	}

	return nullptr;
}


void UPBLobbyTourGuide::StartTourFromTo(const FName& InCurrCamName, const FName& InTargetCamName)
{
	bIsPlaying = true;
	CurrCamName = InCurrCamName;
	TargetCamName = InTargetCamName;
	ElapsedTime = 0.0f;

	TurnOffEveryLights();

	if (DoINeedLightInterp(CurrCamName, TargetCamName))
	{
		SetupPostProcessBlend();

		TurnOnLightsOfCam(CurrCamName, 1.0f);
		TurnOnLightsOfCam(TargetCamName, 0.0f);
	}
	else
	{
		TurnOnLightsOfCam(CurrCamName, 0.0f);
		TurnOnLightsOfCam(TargetCamName, 1.0f);
	}
}


void UPBLobbyTourGuide::TeleportTo(const FName& InTargetCamName)
{

	TargetCamName = InTargetCamName;
	
	if (nullptr == OwningPlayer)
	{
		return;
	}

	auto Pawn = Cast<APBLobbySpectatorPawn>(OwningPlayer->GetSpectatorPawn());
	if (nullptr == Pawn)
	{
		return;
	}

	auto TargetCam = CamOfEachPage.Contains(TargetCamName) ? CamOfEachPage[TargetCamName] : nullptr;
	if (nullptr == TargetCam)
	{
		return;
	}

	/**
	* Set the camera position and light settings immediately.
	*/

	TurnOffEveryLights();
	TurnOnLightsOfCam(TargetCamName, 1.0f);

	Pawn->GetCameraComponent()->PostProcessSettings = TargetCam->GetCameraComponent()->PostProcessSettings;
	Pawn->GetCameraComponent()->SetPostProcessBlendWeight(1.0f);
	Pawn->SetActorRotation(TargetCam->GetActorRotation());
	Pawn->SetActorLocation(TargetCam->GetActorLocation());

	OwningPlayer->PlayerCameraManager->SetFOV(TargetCam->GetCameraComponent()->FieldOfView);

	StopTour();


}

void UPBLobbyTourGuide::CacheCamsFromLevel(class UWorld* World)
{
	if (nullptr == World)
	{
		return;
	}

	CamOfEachPage.Empty();

	for (TActorIterator<ACameraActor> Itr(World); Itr; ++Itr)
	{
		ACameraActor* Cam = *Itr;
		if (Cam)
		{
			if (true == Cam->ActorHasTag(FLobbyCamName::MainMenu))
			{
				CamOfEachPage.Add(FLobbyCamName::MainMenu, Cam);
			}
			else if (true == Cam->ActorHasTag(FLobbyCamName::Loadout))
			{
				CamOfEachPage.Add(FLobbyCamName::Loadout, Cam);
			}
			else if (true == Cam->ActorHasTag(FLobbyCamName::Multiplay))
			{
				CamOfEachPage.Add(FLobbyCamName::Multiplay, Cam);
			}
			else if (true == Cam->ActorHasTag(FLobbyCamName::CharSelect_Main))
			{
				CamOfEachPage.Add(FLobbyCamName::CharSelect_Main, Cam);
			}
			else if (true == Cam->ActorHasTag(FLobbyCamName::CharSelect_Left))
			{
				CamOfEachPage.Add(FLobbyCamName::CharSelect_Left, Cam);
			}
			else if (true == Cam->ActorHasTag(FLobbyCamName::CharSelect_Right))
			{
				CamOfEachPage.Add(FLobbyCamName::CharSelect_Right, Cam);
			}
			else if (true == Cam->ActorHasTag(FLobbyCamName::LoadingScreen))
			{
				CamOfEachPage.Add(FLobbyCamName::LoadingScreen, Cam);
			}
		}
	}
}

void UPBLobbyTourGuide::CachePostProcessVolumeFromLevel(class UWorld* World)
{
	if (nullptr == World)
	{
		return;
	}
	
	// I assume that there's only one post-process volume for blend in the level
	for (TActorIterator<APostProcessVolume> Itr(World); Itr; ++Itr)
	{
		if (Itr->ActorHasTag("PostProcessVolumeForBlend") && Itr->IsValidLowLevel())
		{
			PPV = *Itr;
		}
	}
}

void UPBLobbyTourGuide::UpdateLobbyTour(float DeltaTime)
{

	if (nullptr == OwningPlayer)
	{
		StopTour();
		return;
	}

	auto Pawn = Cast<APBLobbySpectatorPawn>(OwningPlayer->GetSpectatorPawn());
	if (nullptr == Pawn)
	{
		StopTour();
		return;
	}

	if (false == CamOfEachPage.Contains(TargetCamName))
	{
		StopTour();
		return;
	}

	auto TargetCam = CamOfEachPage[TargetCamName];
	if (nullptr == TargetCam)
	{
		StopTour();
		return;
	}

	float const InterpSpeed = OwningLobby->DefaultTourSpeed;


	// Interpolate Rotation
	auto FromRot = Pawn->GetActorRotation();
	auto ToRot = TargetCam->GetActorRotation();
	auto ResRot = FMath::RInterpTo(FromRot, ToRot, DeltaTime, InterpSpeed);
	Pawn->SetActorRotation(ResRot);

	// Interpolate location
	auto FromLoc = Pawn->GetActorLocation();
	auto ToLoc = TargetCam->GetActorLocation();
	auto ResLoc = FMath::VInterpTo(FromLoc, ToLoc, DeltaTime, InterpSpeed);
	Pawn->SetActorLocation(ResLoc);

	// Interpolate FOV
	auto FromFOV = OwningPlayer->PlayerCameraManager->GetFOVAngle();
	auto ToFOV = TargetCam->GetCameraComponent()->FieldOfView;
	auto ResFOV = FMath::FInterpTo(FromFOV, ToFOV, DeltaTime, InterpSpeed);
	OwningPlayer->PlayerCameraManager->SetFOV(ResFOV);

	// If the interpolation process are ended, then set the settings as the target cam's settings.
	auto Dist = ToLoc - FromLoc;
	const float SmallNumber = KINDA_SMALL_NUMBER* 10.0f;
	if (Dist.SizeSquared() < SmallNumber)
	{
		Pawn->GetCameraComponent()->PostProcessSettings = TargetCam->GetCameraComponent()->PostProcessSettings;
		Pawn->SetActorRotation(ToRot);
		Pawn->SetActorLocation(ToLoc);
		OwningPlayer->PlayerCameraManager->SetFOV(ToFOV);

		TurnOffEveryLights();
		TurnOnLightsOfCam(TargetCamName, 1.0f);

		StopTour();
	}


}

void UPBLobbyTourGuide::UpdateLobbyTourWithCurve(float DeltaTime, class UCurveFloat* InterpCurve)
{
	if (nullptr == InterpCurve)
	{
		StopTour();
		return;
	}

	if (nullptr == OwningPlayer)
	{
		StopTour();
		return;
	}

	auto Pawn = Cast<APBLobbySpectatorPawn>(OwningPlayer->GetSpectatorPawn());
	if (nullptr == Pawn)
	{
		StopTour();
		return;
	}


	auto TargetCam = CamOfEachPage.Contains(TargetCamName) ? CamOfEachPage[TargetCamName] : nullptr;
	if (nullptr == TargetCam)
	{
		StopTour();
		return;
	}

	auto OriginCam = CamOfEachPage.Contains(CurrCamName) ? CamOfEachPage[CurrCamName] : nullptr;
	if (nullptr == OriginCam)
	{
		StopTour();
		return;
	}

	float TStart{}, TEnd{};
	InterpCurve->GetTimeRange(TStart, TEnd);
	const float Duration = FMath::Max(TEnd - TStart, 0.0f);

	if ( Duration > ElapsedTime)
	{
		// Progress at current time in the curve [0, 1]
		float ProgressRate = InterpCurve->GetFloatValue(ElapsedTime);

		// Interpolate Rotation
		auto OriginRot = OriginCam->GetActorRotation();
		auto TargetRot = TargetCam->GetActorRotation();
		auto ResRot = FMath::LerpRange(OriginRot, TargetRot, ProgressRate);
		Pawn->SetActorRotation(ResRot);

		// Interpolate Location
		auto OriginLoc = OriginCam->GetActorLocation();
		auto TargetLoc = TargetCam->GetActorLocation();
		auto ResLoc = FMath::Lerp(OriginLoc, TargetLoc, ProgressRate);
		Pawn->SetActorLocation(ResLoc);

		// Interpolate FOV
		auto OriginFOV = OriginCam->GetCameraComponent()->FieldOfView;
		auto TargetFOV = TargetCam->GetCameraComponent()->FieldOfView;
		auto ResFOV = FMath::Lerp(OriginFOV, TargetFOV, ProgressRate);
		OwningPlayer->PlayerCameraManager->SetFOV(ResFOV);

		ElapsedTime += DeltaTime;

		UpdateBrightnessOfLights(ProgressRate);

		UpdatePostProcessBlend(ProgressRate);


	}

	else
	{
		// If the interpolation process are ended, then set the settings as the target cam's settings.
		Pawn->GetCameraComponent()->PostProcessSettings = TargetCam->GetCameraComponent()->PostProcessSettings;
		Pawn->GetCameraComponent()->SetPostProcessBlendWeight(1.0f);
		Pawn->SetActorRotation(TargetCam->GetActorRotation());
		Pawn->SetActorLocation(TargetCam->GetActorLocation());
		
		OwningPlayer->PlayerCameraManager->SetFOV(TargetCam->GetCameraComponent()->FieldOfView);

		StopTour();

	}



}


void UPBLobbyTourGuide::StopTour()
{
	bIsPlaying = false;
	CurrCamName = FLobbyCamName::None;
	TargetCamName = FLobbyCamName::None;
	ElapsedTime = 0.0f;

}

void UPBLobbyTourGuide::UpdateBrightnessOfLights(float Rate)
{
	float const ClampedRate = FMath::Clamp(Rate, 0.0f, 1.0f);

	auto ScriptActor = GetScriptActor();
	if (ScriptActor)
	{
		if (DoINeedLightInterp(CurrCamName, TargetCamName))
		{
			ScriptActor->SetBrightnessRateOfLights(CurrCamName, 1.0f - ClampedRate);
			ScriptActor->SetBrightnessRateOfLights(TargetCamName, ClampedRate);
		}
	}
}

void UPBLobbyTourGuide::TurnOffEveryLights()
{
	auto World = GetWorld();
	if (World)
	{
		auto ScriptActor = Cast<APBLobbyLevelScriptActor>(World->GetLevelScriptActor());
		if (ScriptActor)
		{
			ScriptActor->TurnOffEveryLights();
		}
	}
}


void UPBLobbyTourGuide::TurnOnLightsOfCam(const FName& CamName, float BrightnessRate)
{
	auto ScriptActor = GetScriptActor();
	if (ScriptActor)
	{
		ScriptActor->TurnOnAndSetBrightnessRateOfLights(CamName, BrightnessRate);
	}
}

bool UPBLobbyTourGuide::DoINeedLightInterp(const FName& InCurrCamName, const FName& InTargetCamName)
{
	if (InTargetCamName != FLobbyCamName::None)
	{
		// If the lobby use the same cam, then we don't need light interpolation
		if (InCurrCamName != InTargetCamName)
		{
			return true;
		}
	}

	return false;
}

void UPBLobbyTourGuide::UpdatePostProcessBlend(float Rate)
{
	float const ClampedRate = FMath::Clamp(Rate, 0.0f, 1.0f);

	if (nullptr == PPV)
	{
		return;
	}

	auto Pawn = Cast<APBLobbySpectatorPawn>(OwningPlayer->GetSpectatorPawn());
	if (nullptr ==  Pawn)
	{
		return;
	}

	if (false == DoINeedLightInterp(CurrCamName, TargetCamName))
	{
		return;
	}

	PPV->BlendWeight = ClampedRate;
	Pawn->GetCameraComponent()->SetPostProcessBlendWeight(1 - ClampedRate);

}

void UPBLobbyTourGuide::SetPPVSettingsFromTargetCam()
{
	if (nullptr == PPV)
	{
		return;
	}

	auto TargetCam = CamOfEachPage.Contains(TargetCamName) ? CamOfEachPage[TargetCamName] : nullptr;
	if (nullptr == TargetCam)
	{
		return;
	}

	PPV->Settings = TargetCam->GetCameraComponent()->PostProcessSettings;

}

void UPBLobbyTourGuide::SetupPostProcessBlend()
{
	if (PPV)
	{
		SetPPVSettingsFromTargetCam();

		PPV->BlendWeight = 0.0f;
	}
}


class APBLobbyLevelScriptActor* UPBLobbyTourGuide::GetScriptActor() const
{
	auto World = GetWorld();
	if (World)
	{
		return Cast<APBLobbyLevelScriptActor>(World->GetLevelScriptActor());
	}

	return nullptr;
}

