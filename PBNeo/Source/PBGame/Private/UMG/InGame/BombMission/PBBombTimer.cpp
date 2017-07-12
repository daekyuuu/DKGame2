// Fill out your copyright notice in the Description page of Project Settings.

#include "PBGame.h"
#include "PBBombTimer.h"
#include "UMG/PBUMGUtils.h"
#include "Components/TextBlock.h"
#include "Weapons/Projectile/PBProj_Bomb.h"

PBGetterWidgetBlueprintVariable_Implementation(UTextBlock, UPBBombTimer, CountDownTimer);

UPBBombTimer::UPBBombTimer()
{
	CountdownLevel1AudioComponent = nullptr;
	CountdownLevel2AudioComponent = nullptr;
	CountdownLevel3AudioComponent = nullptr;
	CountdownLevel4AudioComponent = nullptr;
	CountdownLevel5AudioComponent = nullptr;
}

void UPBBombTimer::NativeConstruct()
{
	Super::NativeConstruct();

	SetVisibility(ESlateVisibility::Hidden);

	BindWidgetEvent(OnBombPlanted, UPBBombTimer::NotifyBombPlanted);
	BindWidgetEvent(OnSyncBombRemainingTime, UPBBombTimer::NotifySyncBombRemainingTime);
	BindWidgetEvent(OnBombDefused, UPBBombTimer::NotifyBombDefused);
	BindWidgetEvent(OnBombExploded, UPBBombTimer::NotifyBombExploded);
	BindWidgetEvent(OnBombFrozen, UPBBombTimer::NotifyBombFrozen);

	TimeToExplosion = 0.0f;
}

void UPBBombTimer::NativeDestruct()
{
	UnbindWidgetEvent(OnBombPlanted);
	UnbindWidgetEvent(OnSyncBombRemainingTime);
	UnbindWidgetEvent(OnBombDefused);
	UnbindWidgetEvent(OnBombExploded);
	UnbindWidgetEvent(OnBombFrozen);

	Super::NativeDestruct();
}

void UPBBombTimer::NotifyBombPlanted()
{
	SetVisibility(ESlateVisibility::Visible);

	auto World = GetWorld();
	if (GetWorld())
		return;

	//@note: Bomb이 월드에 하나만 존재한다는 가정. 그 이상 존재 할 경우 구현방법을 바꾸어야한다.
	for (TActorIterator<APBProj_Bomb> Itr(World); Itr; ++Itr)
	{
		auto Bomb = *Itr;
		if (Bomb)
		{
			FProjectileData ProjData = Bomb->GetProjectileData();
			TimeToExplosion = ProjData.ProjectileLife;
		}
	}

	ensure(TimeToExplosion != 0.0f);

	PrevCoundDownLevel = -1;
}

void UPBBombTimer::NotifySyncBombRemainingTime(float RemainingTime)
{
	GetBPV_CountDownTimer()->SetText(FText::FromString(UPBUMGUtils::ToTimeString(RemainingTime)));

	if (TimeToExplosion > 0 && RemainingTime > 0)
	{
		float PercentageUntilExplosion = 1.0f - (RemainingTime / TimeToExplosion);
		int32 CountDownLevel = (int32)(PercentageUntilExplosion / 2.0f * 10.0f) + 1;
		if (PrevCoundDownLevel != CountDownLevel && CountDownLevel < 5)
		{
			StopAllSound();

			if (CountDownLevel <= 1)
			{
				CountdownLevel1AudioComponent = UGameplayStatics::SpawnSound2D(GetWorld(), CountdownLevel1Sound);
			}
			else if (CountDownLevel == 2)
			{
				CountdownLevel2AudioComponent = UGameplayStatics::SpawnSound2D(GetWorld(), CountdownLevel2Sound);
			}
			else if (CountDownLevel == 3)
			{
				CountdownLevel3AudioComponent = UGameplayStatics::SpawnSound2D(GetWorld(), CountdownLevel3Sound);
			}
			else if (CountDownLevel == 4)
			{
				//@note: Level 4는 Loop가 아니므로, 재생이 완료되면 바로 다음 카운트 레벨로 넘어갈 필요가 있다.
				CountdownLevel4AudioComponent = UGameplayStatics::SpawnSound2D(GetWorld(), CountdownLevel4Sound);

				CountdownLevel4AudioComponent->OnAudioFinishedNative.AddUObject(this, &UPBBombTimer::OnCountdownLevel5);
			}

			PrevCoundDownLevel = CountDownLevel;
		}
	}
}

void UPBBombTimer::OnCountdownLevel5(UAudioComponent *HasFinishedAudioComponent)
{
	CountdownLevel5AudioComponent = UGameplayStatics::SpawnSound2D(GetWorld(), CountdownLevel5Sound);
}

void UPBBombTimer::NotifyBombDefused()
{
	StopAllSound();

	StopBombTimer();
}

void UPBBombTimer::NotifyBombExploded()
{
	StopAllSound();

	StopBombTimer();
}

void UPBBombTimer::NotifyBombFrozen()
{
	StopAllSound();

	StopBombTimer();
}

void UPBBombTimer::StopBombTimer()
{
	TimeToExplosion = 0.0f;
	PrevCoundDownLevel = -1;

	StopAllSound();

	SetVisibility(ESlateVisibility::Hidden);

	auto World = GetWorld();
	if (World)
	{
		auto GS = World->GetGameState<APBGameState>();
		if (GS)
		{
			GS->ServerSetBombRemainingTime(0);
		}
	}
}

UAudioComponent *Spawner(UWorld *World, USoundCue *Sound)
{
	if (nullptr == World)
		return nullptr;

	if (Sound)
	{
		auto AudioComponent = UGameplayStatics::SpawnSound2D(World, Sound);
		AudioComponent->bAutoDestroy = false;
		AudioComponent->bStopWhenOwnerDestroyed = false;
		AudioComponent->bAutoActivate = false;
		return AudioComponent;
	}

	return nullptr;
}

void Stopper(UAudioComponent *AudioComponent)
{
	if (AudioComponent 
		&& AudioComponent->IsValidLowLevel() 
		&& AudioComponent->IsPlaying() 
		&& false == AudioComponent->IsPendingKillOrUnreachable())
	{
		AudioComponent->Stop();
		AudioComponent->DestroyComponent();
		AudioComponent = nullptr;
	}
}

void UPBBombTimer::StopAllSound()
{
	Stopper(CountdownLevel1AudioComponent);
	Stopper(CountdownLevel2AudioComponent);
	Stopper(CountdownLevel3AudioComponent);
	Stopper(CountdownLevel4AudioComponent);
	Stopper(CountdownLevel5AudioComponent);
}