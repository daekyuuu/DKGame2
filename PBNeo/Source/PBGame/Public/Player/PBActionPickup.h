// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/ActorComponent.h"
#include "PBActionPickup.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PBGAME_API UPBActionPickup : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UPBActionPickup();

	// Called when the game starts
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction ) override;

	// Called on server only
	void SpawnPickupWeapon(class APBWeapon* Weap);

public:

	/**
	* Called only on server
	*/
	void NotifyOuterBeginOverlap(class APBPickup* Pickup);
	void NotifyOuterEndOverlap(class APBPickup* Pickup);
	void NotifyInnerBeginOverlap(class APBPickup* Pickup);
	void NotifyInnerEndOverlap(class APBPickup* Pickup);

	/**
	* called on local
	*/
	void NotifyPickupButtonPressed();
	void NotifyPickupButtonReleased();


protected:
	UFUNCTION(reliable, client)
	void ClientHandleOuterBeginOverlap(class APBPickup* Pickup);

	UFUNCTION(reliable, client)
	void ClientHandleOuterEndOverlap(class APBPickup* Pickup);

	UFUNCTION(reliable, client)
	void ClientHandlePickupableChanged(class APBPickup* Pickup);

	UFUNCTION(reliable, client)
	void ClientHandlePickupButtonPressed(class APBPickup* Pickup, float TimerDuration);

	UFUNCTION(reliable, client)
	void ClientHandlePickupButtonReleased(class APBPickup* Pickup);

	UFUNCTION(reliable, client)
	void ClientHandleFinishPickup(class APBPickup* Pickup);

private:

	UFUNCTION(reliable, Server, WithValidation)
		void ServerStartPickupProcess();

	UFUNCTION(reliable, Server, WithValidation)
		void ServerStopPickupProcess();

	UFUNCTION()
		void StartPickupProcess();

	UFUNCTION()
		void StopPickupProcess();

private:

	/* ------------------------------------------------------------------------------- */
	// Variables only for server
	/* ------------------------------------------------------------------------------- */

	// If the character is entered to the inner volume of pickup, then the pickup will be a candidate of pickupable.
	UPROPERTY()
		TArray<class APBPickup*> PickupCandidates;

	// It can be picked up because it's the nearest pickup among the candidate.
	UPROPERTY()
		class APBPickup* Pickupable;

	UPROPERTY()
		FTimerHandle TimerHandle_Pickup;

	UPROPERTY()
		float ButtonPressDurationForPickup;


private:

	// Calculate and return the nearest pickup among the candidates.
	class APBPickup* CalculateNearestPickup();

	void SetPickupTimer();

	void ClearPickupTimer();

	// After all of pickup process, it will be called to perform a real pickup!
	void PerformPickup();

private:

	TSharedPtr<FPBWidgetEventDispatcher> GetWidgetDispatcher();
		
	
};
