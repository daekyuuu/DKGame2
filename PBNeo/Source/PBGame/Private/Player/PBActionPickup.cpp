// Fill out your copyright notice in the Description page of Project Settings.

#include "PBGame.h"
#include "PBActionPickup.h"
#include "PBPickup.h"
#include "PBPlayerController.h"
#include "FPBWidgetEventDispatcher.h"
#include "PBPickupWeapon.h"
#include "PBWeapon.h"
#include "PBGameplayStatics.h"
#include "PBItemTableManager.h"

// Sets default values for this component's properties
UPBActionPickup::UPBActionPickup()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
	ButtonPressDurationForPickup = 0.5f;
}


// Called when the game starts
void UPBActionPickup::BeginPlay()
{
	Super::BeginPlay();

	Pickupable = nullptr;
	
}


// Called every frame
void UPBActionPickup::TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction )
{
	Super::TickComponent( DeltaTime, TickType, ThisTickFunction );

	if (GetOwner()->Role == ROLE_Authority)
	{
		// It can be nullptr( If there's no nearest pickup)
		auto CalculatedPickup = CalculateNearestPickup();

		if (CalculatedPickup != Pickupable)
		{
			// 1. Stop the current pickupable process
			StopPickupProcess();

			// 2. Switch the pickupable to the new one
			Pickupable = CalculatedPickup;

			// 3. Notify event that pickupable has changed
			ClientHandlePickupableChanged(Pickupable);
		}
	}

	

}

void UPBActionPickup::SpawnPickupWeapon(class APBWeapon* Weap)
{
	auto Character = Cast<APBCharacter>(GetOuter());
	if (nullptr == Character)
	{
		return;
	}

	if (nullptr == Weap)
	{
		return;
	}


	APBGameMode* GameMode = GetWorld() ? GetWorld()->GetAuthGameMode<APBGameMode>() : nullptr;
	if (GameMode)
	{
		if (Weap->GetCurrentAmmo() > 0)
		{
			APBPickup* PickupItem = GameMode->NewPickupWeap(Weap, Character->GetTransform());
		}
	}

}

class APBPickup* UPBActionPickup::CalculateNearestPickup()
{
	if (PickupCandidates.Num() < 1)
	{
		return nullptr;
	}

	auto Character = Cast<APBCharacter>(GetOuter());
	if (nullptr == Character)
	{
		return nullptr;
	}

	TPair<APBPickup*, float> NearestPickup;
	NearestPickup.Key = nullptr;
	NearestPickup.Value = 0.0f;

	// Get the smallest angle pickup between eye direction and to weapon direction.
	for (auto PickupCandidate : PickupCandidates)
	{
		FVector EyeLocation;
		FRotator EyeRotation;
		Character->GetActorEyesViewPoint(EyeLocation, EyeRotation);

		auto EyeDir = EyeRotation.Vector().GetSafeNormal();
		auto ToPickupDir = (PickupCandidate->GetActorLocation() - EyeLocation).GetSafeNormal();

		// AngleCos -> [0,1]
		float AngleCos = FVector::DotProduct(EyeDir, ToPickupDir);

		if (AngleCos > NearestPickup.Value)
		{
			NearestPickup.Key = PickupCandidate;
			NearestPickup.Value = AngleCos;
		}

	}

	return NearestPickup.Key;

}

void UPBActionPickup::StartPickupProcess()
{
	if (Pickupable)
	{
		SetPickupTimer();
		ClientHandlePickupButtonPressed(Pickupable, ButtonPressDurationForPickup);
	}
}

void UPBActionPickup::StopPickupProcess()
{
	if (Pickupable)
	{
		ClearPickupTimer();
		ClientHandlePickupButtonReleased(Pickupable);
	}
}


void UPBActionPickup::ServerStartPickupProcess_Implementation()
{
	StartPickupProcess();
}


void UPBActionPickup::ServerStopPickupProcess_Implementation()
{
	StopPickupProcess();
}

bool UPBActionPickup::ServerStartPickupProcess_Validate()
{
	return true;
}


bool UPBActionPickup::ServerStopPickupProcess_Validate()
{
	return true;
}


void UPBActionPickup::SetPickupTimer()
{
	auto World = GetWorld();
	if (World)
	{
		World->GetTimerManager().SetTimer(TimerHandle_Pickup, this, &UPBActionPickup::PerformPickup, ButtonPressDurationForPickup);
	}
}

void UPBActionPickup::ClearPickupTimer()
{
	if (GetWorld())
	{
		GetWorld()->GetTimerManager().ClearTimer(TimerHandle_Pickup);
	}
}

void UPBActionPickup::NotifyOuterBeginOverlap(class APBPickup* Pickup)
{
	// To Prevent the case that the PlayerController is not possessed to the character yet.
	GetWorld()->GetTimerManager().SetTimerForNextTick(
	[Pickup, this]()
	{
		ClientHandleOuterBeginOverlap(Pickup);
	}
	);
}

void UPBActionPickup::NotifyOuterEndOverlap(class APBPickup* Pickup)
{
	GetWorld()->GetTimerManager().SetTimerForNextTick(
	[Pickup, this]()
	{
		ClientHandleOuterEndOverlap(Pickup);
	}
	);
}

void UPBActionPickup::NotifyInnerBeginOverlap(class APBPickup* Pickup)
{
	GetWorld()->GetTimerManager().SetTimerForNextTick(
	[Pickup, this]()
	{
		if (Pickup)
		{
			PickupCandidates.AddUnique(Pickup);
		}
	}
	);

}

void UPBActionPickup::NotifyInnerEndOverlap(class APBPickup* Pickup)
{
	GetWorld()->GetTimerManager().SetTimerForNextTick(
	[Pickup, this]()
	{
		if (Pickup && PickupCandidates.Contains(Pickup))
		{
			PickupCandidates.Remove(Pickup);
		}
	}
	);

}

void UPBActionPickup::NotifyPickupButtonPressed()
{
	// Set the pickup timer and send the button pressed event to the the widget
	ServerStartPickupProcess();

}

void UPBActionPickup::NotifyPickupButtonReleased()
{
	// clear the pickup timer and send the button released event to the widget
	ServerStopPickupProcess();
	
}



void UPBActionPickup::PerformPickup()
{
	if (nullptr == Pickupable)
	{
		return;
	}

	if (false == PickupCandidates.Contains(Pickupable))
	{
		return;
	}


	auto Character = Cast<APBCharacter>(GetOwner());
	if (nullptr == Character)
	{
		return;
	}


	if (false == Pickupable->CanPickup(Character))
	{
		return;
	}


	// Drop the current weapon that placed at the pickup's slot
	auto PickupWeapon = Cast<APBPickupWeapon>(Pickupable);
	if (PickupWeapon)
	{
		auto WeapType = PickupWeapon->GetWeaponType();
		if (WeapType)
		{
			auto WeapCDO = Cast<APBWeapon>(WeapType->GetDefaultObject());
			if (WeapCDO)
			{
				auto TM = UPBGameplayStatics::GetItemTableManager(this);
				if (TM)
				{
					auto TableData = TM->GetWeaponTableData(WeapCDO->GetItemID());
					if (TableData)
					{
						EWeaponSlot WeapSlot = (EWeaponSlot)TableData->WeaponSlotEnum;
						auto CurrWeaponAtSlot = Character->GetWeapon(WeapSlot, 0);
						if (CurrWeaponAtSlot)
						{
							SpawnPickupWeapon(CurrWeaponAtSlot);
						}
					}
				}
			}
		}
	}

	// Internally, a new weapon that just has picked up will replace the already existing weapon.
	// It means that I don't need to worry about previous weapon management.
	Pickupable->PickupOnTouch(Character);


	// Clean up pickup variables..
	PickupCandidates.Remove(Pickupable);
	Pickupable = nullptr;


	// notify finish pickup event to the widget
	ClientHandleFinishPickup(Pickupable);




}


TSharedPtr<FPBWidgetEventDispatcher> UPBActionPickup::GetWidgetDispatcher()
{
	auto Character = Cast<APBCharacter>(GetOuter());
	if (nullptr == Character)
	{
		return TSharedPtr<FPBWidgetEventDispatcher>(nullptr);
	}

	auto PC = Cast<APBPlayerController>(Character->GetController());
	if (nullptr == PC)
	{
		return TSharedPtr<FPBWidgetEventDispatcher>(nullptr);
	}

	return PC->WidgetEventDispatcher;
}

void UPBActionPickup::ClientHandleOuterBeginOverlap_Implementation(class APBPickup* Pickup)
{
	if (GetWidgetDispatcher().IsValid())
	{
		GetWidgetDispatcher()->OnEnterOuterPickup.Broadcast(Pickup);
	}
}

void UPBActionPickup::ClientHandleOuterEndOverlap_Implementation(class APBPickup* Pickup)
{
	if (GetWidgetDispatcher().IsValid())
	{
		GetWidgetDispatcher()->OnLeaveOuterPickup.Broadcast(Pickup);
	}
}


void UPBActionPickup::ClientHandleFinishPickup_Implementation(class APBPickup* Pickup)
{
	if (GetWidgetDispatcher().IsValid())
	{
		GetWidgetDispatcher()->OnFinishPickup.Broadcast(Pickup);
	}
}


void UPBActionPickup::ClientHandlePickupableChanged_Implementation(class APBPickup* Pickup)
{
	if (GetWidgetDispatcher().IsValid())
	{
		GetWidgetDispatcher()->OnReadyPickup.Broadcast(Pickup);
	}
}

void UPBActionPickup::ClientHandlePickupButtonPressed_Implementation(class APBPickup* Pickup, float TimerDuration)
{
	if (GetWidgetDispatcher().IsValid())
	{
		GetWidgetDispatcher()->OnStartPickup.Broadcast(Pickup, TimerDuration);
	}
}

void UPBActionPickup::ClientHandlePickupButtonReleased_Implementation(class APBPickup* Pickup)
{
	if (GetWidgetDispatcher().IsValid())
	{
		GetWidgetDispatcher()->OnCancelPickup.Broadcast(Pickup);
	}
}


