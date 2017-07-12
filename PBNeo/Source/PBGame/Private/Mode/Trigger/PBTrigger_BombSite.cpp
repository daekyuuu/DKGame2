// Fill out your copyright notice in the Description page of Project Settings.

#include "PBGame.h"
#include "PBTrigger_BombSite.h"

FString EPBBombSiteSignAsString(EPBBombSiteSign BombSiteSign)
{
	switch (BombSiteSign)
	{
	case EPBBombSiteSign::A: return TEXT("A");
	case EPBBombSiteSign::B: return TEXT("B");
	}

	return FString();
}

APBTrigger_BombSite::APBTrigger_BombSite()
{
	//@note: for networking
	{
		bReplicates = true;
		bAlwaysRelevant = true;
	}

	bIsPlantedBomb = false;
	bToBeHidden = false;
}

void APBTrigger_BombSite::Reset()
{
	bToBeHidden = false;
	bIsPlantedBomb = false;
	SetActorHiddenInGame(false);
}

void APBTrigger_BombSite::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(APBTrigger_BombSite, bToBeHidden);
	DOREPLIFETIME(APBTrigger_BombSite, bIsPlantedBomb);
}

void APBTrigger_BombSite::TriggerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, bool bFromSweep, const FHitResult & SweepResult)
{
	APBCharacter* C = Cast<APBCharacter>(OtherActor);
	if (C)
	{
		C->NotifyEnterBombSite(this);
	}
}

void APBTrigger_BombSite::TriggerEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherCompt)
{
	APBCharacter* C = Cast<APBCharacter>(OtherActor);
	if (C)
	{
		C->NotifyLeaveBombSite();
	}
}

void APBTrigger_BombSite::NotifyBombPlanted()
{
	bToBeHidden = true;
	OnRep_ToBeHidden();
}

TSubclassOf<UPBBombSiteIndicatorImageWidget> APBTrigger_BombSite::GetIndicatorImageWidgetClass() const
{
	return BombSiteConfig.IndicatorImageWidgetClass;
}

EPBBombSiteSign APBTrigger_BombSite::GetSiteSign() const
{
	return BombSiteConfig.SiteSign;
}

void APBTrigger_BombSite::OnRep_ToBeHidden()
{
	SetActorHiddenInGame(bToBeHidden);
}

bool APBTrigger_BombSite::ServerSetIsPlantedBomb_Validate(bool bPlantedBomb)
{
	return true;
}

void APBTrigger_BombSite::ServerSetIsPlantedBomb_Implementation(bool bPlantedBomb)
{
	bIsPlantedBomb = bPlantedBomb;
	OnRep_bIsPlantedBomb();
}

bool APBTrigger_BombSite::IsPlantedBomb() const
{
	return bIsPlantedBomb;
}

void APBTrigger_BombSite::OnRep_bIsPlantedBomb()
{
	auto World = GetWorld();
	if (nullptr == World)
		return;

	if (bIsPlantedBomb)
	{
		for (auto It = World->GetPlayerControllerIterator(); It; ++It)
		{
			APBPlayerController* PC = Cast<APBPlayerController>(*It);
			if (PC && PC->IsLocalController())
			{
				PC->WidgetEventDispatcher->OnBombPlanted.Broadcast();
			}
		}
	}
}