// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

//#include "UObject/NoExportTypes.h"
#include "Object.h"
#include "PBLobbyTourGuide.generated.h"

/**
 * This class help to tour around in the lobby level.
 * In the lobby level, PlayerController's camera should be move from position to position and from rotation to rotation.
 * The client can not dare to stroll around the place if there's no permission of tour guide.
 * 
 */


UCLASS()
class PBGAME_API UPBLobbyTourGuide : public UObject, public FTickableGameObject
{
	GENERATED_BODY()

public:

	UPBLobbyTourGuide();
	void Init(class APlayerController* Controller, class UPBLobby* LobbyInst);

	void Reset();

protected:

	virtual void Tick(float DeltaTime) override;
	virtual bool IsTickable() const override;
	virtual bool IsTickableInEditor() const override;
	virtual bool IsTickableWhenPaused() const override;
	virtual TStatId GetStatId() const override;
	virtual UWorld* GetWorld() const override;


public:
	/**
	* Start the cam tour from the CurrCamLocation to the TargetCamLocation with interpolation
	* @Param InCurrCamName		The Current cam name of lobby level(See: FLobbyCamName)
	* @Param InTargetCamName	The Current cam name of lobby level(See: FLobbyCamName)
	* 
	*/
	void StartTourFromTo(const FName& InCurrCamName, const FName& InTargetCamName);

	/**
	* Teleport cam position to the target cam
	* @Param InTargetCamName The Current cam name of lobby level(See: FLobbyCamName)
	*/
	void TeleportTo(const FName& InTargetCamName);

private:

	void CacheCamsFromLevel(class UWorld* World);
	void CachePostProcessVolumeFromLevel(class UWorld* World);
	
	/** 
	* Interpolate the player cam's transform, fov, post-process settings and lights brightness. It's called in tick(). 
	*/
	void UpdateLobbyTour(float DeltaTime);
	void UpdateLobbyTourWithCurve(float DeltaTime, class UCurveFloat* InterpCurve);

	void StopTour();


	/* ------------------------------------------------------------------------------- */
	// Lights
	/* ------------------------------------------------------------------------------- */
private:
	void UpdateBrightnessOfLights(float Rate);

	/**
	* Turn off the every lights dependent on the camera, in the lobby level.
	*/
	void TurnOffEveryLights();

	/**
	* Turn on the lights dependent on the camera and set the brightness rate.
	* @Param BrightnessRate The rate of brightness of lights. [0,1]
	*/
	void TurnOnLightsOfCam(const FName& CamName, float BrightnessRate);

	bool DoINeedLightInterp(const FName& InCurrCamName, const FName& InTargetCamName);


	/* ------------------------------------------------------------------------------- */
	// Post-Process effect
	/* ------------------------------------------------------------------------------- */
	void UpdatePostProcessBlend(float Rate);
	void SetPPVSettingsFromTargetCam();
	void SetupPostProcessBlend();

private:

	class APBLobbyLevelScriptActor* GetScriptActor() const;


private:
	
	bool bIsPlaying;

	FName TargetCamName;
	FName CurrCamName;

	float ElapsedTime;

	
	// post process volume for blend
	UPROPERTY()
	class APostProcessVolume* PPV;

	UPROPERTY()
	class APlayerController* OwningPlayer;

	UPROPERTY()
	class UPBLobby* OwningLobby;

	UPROPERTY()
	TMap<FName, class ACameraActor*> CamOfEachPage;
	
	
};
