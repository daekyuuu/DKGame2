// Fill out your copyright notice in the Description page of Project Settings.

#include "PBGame.h"
#include "PBTypes.h"
#include "PBGameplayStatics.h"
#include "PBGameInstance.h"
#include "Table/Item/PBItemTableManager.h"
#include "Table/Item/PBItemTable_Weap.h"
#include "Table/Item/PBItemTable_Char.h"
#include "Weapons/PBWeapon.h"
#include "Player/PBLocalPlayer.h"
#include "Player/PBPersistentUser.h"

// USoundNodeLocalPlayer 와 연계해서 주로 사용한다.
UAudioComponent* UPBGameplayStatics::SpawnSoundAtLocation(UObject* WorldContextObject, AActor* PawnOwner, class USoundBase* Sound, FVector Location, FRotator Rotation /*= FRotator::ZeroRotator*/, float VolumeMultiplier /*= 1.f*/, float PitchMultiplier /*= 1.f*/, float StartTime /*= 0.f*/, class USoundAttenuation* AttenuationSettings /*= nullptr*/, class USoundConcurrency* ConcurrencySettings /*= nullptr*/)
{
	if (!Sound || !GEngine || !GEngine->UseSound())
	{
		return nullptr;
	}

	UWorld* ThisWorld = GEngine->GetWorldFromContextObject(WorldContextObject);
	if (!ThisWorld || !ThisWorld->bAllowAudioPlayback || ThisWorld->GetNetMode() == NM_DedicatedServer)
	{
		return nullptr;
	}

	const bool bIsInGameWorld = ThisWorld->IsGameWorld();

	FAudioDevice::FCreateComponentParams Params(ThisWorld);
	Params.SetLocation(Location);
	Params.AttenuationSettings = AttenuationSettings;
	Params.ConcurrencySettings = ConcurrencySettings;

	UAudioComponent* AudioComponent = FAudioDevice::CreateComponent(Sound, Params);
	if (AudioComponent)
	{
		AudioComponent->SetWorldLocationAndRotation(Location, Rotation);
		AudioComponent->SetVolumeMultiplier(VolumeMultiplier);
		AudioComponent->SetPitchMultiplier(PitchMultiplier);
		AudioComponent->bAllowSpatialization = bIsInGameWorld;
		AudioComponent->bIsUISound = !bIsInGameWorld;
		AudioComponent->bAutoDestroy = true;
		AudioComponent->SubtitlePriority = DEFAULT_SUBTITLE_PRIORITY; // Fixme: pass in? Do we want that exposed to blueprints though?
		AudioComponent->Play(StartTime);
	}

	return AudioComponent;
}

void UPBGameplayStatics::ShowDebugSphere(UObject* WorldContextObject, const FVector& Center, float Radius, FColor const& Color, float Lifetime)
{
	UWorld* ThisWorld = GEngine->GetWorldFromContextObject(WorldContextObject);

	if (ThisWorld == nullptr || !ThisWorld->IsPlayInEditor() || ThisWorld->GetNetMode() == NM_DedicatedServer)
		return;

	// display sphere
	DrawDebugSphere(ThisWorld, Center, Radius, 30, Color, false, Lifetime);

	// display center point
	DrawDebugPoint(ThisWorld, Center, 5.f, Color, false, Lifetime);

	// display radius
	FVector TextLoc = Center;
	TextLoc.Z += 20.f;
	FString Text = FString::Printf(TEXT("%.2f (Radius)"), Radius);
	DrawDebugString(ThisWorld, TextLoc, Text, nullptr, Color, Lifetime, true);
}

void UPBGameplayStatics::ShowDebugWeaponTrace(UObject* WorldContextObject, const FVector& LineStart, const FVector& LineEnd, FColor const& Color, float Lifetime, float Tickness, int32 LineNumber)
{
	UWorld* ThisWorld = GEngine->GetWorldFromContextObject(WorldContextObject);

	if (ThisWorld == nullptr || !ThisWorld->IsPlayInEditor() || ThisWorld->GetNetMode() == NM_DedicatedServer)
		return;

	DrawDebugLine(ThisWorld, LineStart, LineEnd, Color, false, Lifetime, Tickness);

	// display center point
	DrawDebugPoint(ThisWorld, LineEnd, Tickness+2.f, Color, false, Lifetime);

	// display radius
	FVector TextLoc = LineEnd;
	float Lenth = FVector(LineEnd - LineStart).Size();
	FString Text = FString::Printf(TEXT("(%d)%.2f"), LineNumber, Lenth);
	DrawDebugString(ThisWorld, TextLoc, Text, nullptr, Color, Lifetime, true);
}


void UPBGameplayStatics::DisplayWeaponItemList(UObject* WorldContextObject)
{
	auto TM = GetItemTableManager(WorldContextObject);
	if (TM == nullptr) return;

	TArray<FPBWeaponTableData*> OutDataList;
	TM->GetTableDataList(EPBTableType::Weapon, OutDataList);

	for (auto WeapData : OutDataList)
	{
		if (WeapData && WeapData->BPClass)
		{
			auto Weapon = Cast<APBWeapon>(WeapData->BPClass->GetDefaultObject());
			if (Weapon)
			{
				PrintOnScreen(FString::Printf(TEXT("ItemID ( %d ) %s"), Weapon->GetItemID(), *WeapData->BPClass->GetName()), 30.f, FColor::Cyan);
			}
		}
	}
}

bool UPBGameplayStatics::ApplyRadialDamage(UObject* WorldContextObject, float BaseDamage, const FVector& Origin, float DamageRadius, TSubclassOf<UDamageType> DamageTypeClass, const TArray<AActor*>& IgnoreActors, TArray<FTakeDamageInfo>& OutDamagedActors, AActor* DamageCauser, AController* InstigatedByController, bool bDoFullDamage, ECollisionChannel DamagePreventionChannel)
{
	float DamageFalloff = bDoFullDamage ? 0.f : 1.f;
	return ApplyRadialDamageWithFalloff(WorldContextObject, BaseDamage, 0.f, Origin, 0.f, DamageRadius, DamageFalloff, DamageTypeClass, IgnoreActors, OutDamagedActors, DamageCauser, InstigatedByController, DamagePreventionChannel);
}

bool UPBGameplayStatics::ApplyRadialDamageWithFalloff(UObject* WorldContextObject, float BaseDamage, float MinimumDamage, const FVector& Origin, float DamageInnerRadius, float DamageOuterRadius, float DamageFalloff, TSubclassOf<class UDamageType> DamageTypeClass, const TArray<AActor*>& IgnoreActors, TArray<FTakeDamageInfo>& OutDamagedActors, AActor* DamageCauser, AController* InstigatedByController, ECollisionChannel DamagePreventionChannel)
{
	static FName NAME_ApplyRadialDamage = FName(TEXT("ApplyRadialDamage"));
	FCollisionQueryParams SphereParams(NAME_ApplyRadialDamage, false, DamageCauser);

	SphereParams.AddIgnoredActors(IgnoreActors);

	// query scene to see what we hit
	TArray<FOverlapResult> Overlaps;
	UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject);
	World->OverlapMultiByObjectType(Overlaps, Origin, FQuat::Identity, FCollisionObjectQueryParams(FCollisionObjectQueryParams::InitType::AllDynamicObjects), FCollisionShape::MakeSphere(DamageOuterRadius), SphereParams);

	// collate into per-actor list of hit components
	TMap<AActor*, TArray<FHitResult> > OverlapComponentMap;
	for (int32 Idx = 0; Idx<Overlaps.Num(); ++Idx)
	{
		FOverlapResult const& Overlap = Overlaps[Idx];
		AActor* const OverlapActor = Overlap.GetActor();

		if (OverlapActor &&
			OverlapActor->bCanBeDamaged &&
			OverlapActor != DamageCauser &&
			Overlap.Component.IsValid())
		{
			FHitResult Hit;
			if (DamagePreventionChannel == ECC_MAX || ComponentIsDamageableFrom(Overlap.Component.Get(), Origin, DamageCauser, IgnoreActors, DamagePreventionChannel, Hit))
			{
				TArray<FHitResult>& HitList = OverlapComponentMap.FindOrAdd(OverlapActor);
				HitList.Add(Hit);
			}
		}
	}

	// make sure we have a good damage type
	TSubclassOf<UDamageType> const ValidDamageTypeClass = DamageTypeClass ? DamageTypeClass : TSubclassOf<UDamageType>(UDamageType::StaticClass());

	bool bAppliedDamage = false;

	// call damage function on each affected actors
	for (TMap<AActor*, TArray<FHitResult> >::TIterator It(OverlapComponentMap); It; ++It)
	{
		AActor* const Victim = It.Key();

		TArray<FHitResult> const& ComponentHits = It.Value();

		FRadialDamageEvent DmgEvent;
		DmgEvent.DamageTypeClass = ValidDamageTypeClass;
		DmgEvent.ComponentHits = ComponentHits;
		DmgEvent.Origin = Origin;
		DmgEvent.Params = FRadialDamageParams(BaseDamage, MinimumDamage, DamageInnerRadius, DamageOuterRadius, DamageFalloff);

		float ActualDamage = Victim->TakeDamage(BaseDamage, DmgEvent, InstigatedByController, DamageCauser);

		// set victim actors and actual damage to out param
		FTakeDamageInfo TakeDamageInfo;
		TakeDamageInfo.Victim = Victim;
		TakeDamageInfo.ActualDamage = ActualDamage;
		OutDamagedActors.Add(FTakeDamageInfo(TakeDamageInfo));

		bAppliedDamage = true;
	}

	return bAppliedDamage;
}

/** @RETURN True if weapon trace from Origin hits component VictimComp.  OutHitResult will contain properties of the hit. */
bool UPBGameplayStatics::ComponentIsDamageableFrom(UPrimitiveComponent* VictimComp, FVector const& Origin, AActor const* IgnoredActor, const TArray<AActor*>& IgnoreActors, ECollisionChannel TraceChannel, FHitResult& OutHitResult)
{
	static FName NAME_ComponentIsVisibleFrom = FName(TEXT("ComponentIsVisibleFrom"));
	FCollisionQueryParams LineParams(NAME_ComponentIsVisibleFrom, true, IgnoredActor);
	LineParams.AddIgnoredActors(IgnoreActors);

	// Do a trace from origin to middle of box
	UWorld* const World = VictimComp->GetWorld();
	check(World);

	FVector const TraceEnd = VictimComp->Bounds.Origin;
	FVector TraceStart = Origin;
	if (Origin == TraceEnd)
	{
		// tiny nudge so LineTraceSingle doesn't early out with no hits
		TraceStart.Z += 0.01f;
	}
	bool const bHadBlockingHit = World->LineTraceSingleByChannel(OutHitResult, TraceStart, TraceEnd, TraceChannel, LineParams);
	//::DrawDebugLine(World, TraceStart, TraceEnd, FLinearColor::Red, true);

	// If there was a blocking hit, it will be the last one
	if (bHadBlockingHit)
	{
		if (OutHitResult.Component == VictimComp)
		{
			// if blocking hit was the victim component, it is visible
			return true;
		}
		else
		{
			// if we hit something else blocking, it's not
			UE_LOG(LogDamage, Log, TEXT("Radial Damage to %s blocked by %s (%s)"), *GetNameSafe(VictimComp), *GetNameSafe(OutHitResult.GetActor()), *GetNameSafe(OutHitResult.Component.Get()));
			return false;
		}
	}

	// didn't hit anything, assume nothing blocking the damage and victim is consequently visible
	// but since we don't have a hit result to pass back, construct a simple one, modeling the damage as having hit a point at the component's center.
	FVector const FakeHitLoc = VictimComp->GetComponentLocation();
	FVector const FakeHitNorm = (Origin - FakeHitLoc).GetSafeNormal();		// normal points back toward the epicenter
	OutHitResult = FHitResult(VictimComp->GetOwner(), VictimComp, FakeHitLoc, FakeHitNorm);
	return true;
}

float UPBGameplayStatics::InterpDelayToInterpSpeed(float Input)
{
	// Assume the game is 60fps.
	// Assume we are interpolating float value V to float value 0 at interpolation speed S using "FMath::FInterpTo(...) @ Engine version 4.14".
	// Then we have V/1000 = V * ( 1 - S/60)^(60 * TimeElapsed)

	// Let T be the earliest time when CurrentValue is less than V/1000 ("almost zero").
	// Then given every T there is a corresponding S.
	// This function returns S given T.

	if (Input <= 0.f)
	{
		// At interpolation speed 60 we have no delay under 60fps
		return 60.f;
	}
	else
	{
		// S = 60 * (1 - exp(log(1/1000)/60T)
		static float ExponentialConstant = FMath::Loge(1.f/1000.f)/60.f;
		return 60 * (1 - FMath::Exp(ExponentialConstant / Input));
	}
}

class UPBItemTableManager* UPBGameplayStatics::GetItemTableManager(UObject* WorldContextObject)
{
	UPBGameInstance* GI = Cast<UPBGameInstance>(GetGameInstance(WorldContextObject));
	return GI ? GI->GetItemTableManager() : nullptr;
}

class UPBGameTableManager* UPBGameplayStatics::GetGameTableManager(UObject* WorldContextObject)
{
	UPBGameInstance* GI = Cast<UPBGameInstance>(GetGameInstance(WorldContextObject));
	return GI ? GI->GetGameTableManager() : nullptr;
}

class UPBPacketManager* UPBGameplayStatics::GetPacketManager(UObject* WorldContextObject)
{
	UPBGameInstance* GI = Cast<UPBGameInstance>(GetGameInstance(WorldContextObject));
	return GI ? GI->GetPacketManager() : nullptr;
}

class UPBNetClientInterface* UPBGameplayStatics::GetNetClientInterface(UObject* WorldContextObject)
{
	UPBGameInstance* GI = Cast<UPBGameInstance>(GetGameInstance(WorldContextObject));
	return GI ? GI->GetNetClientInterface() : nullptr;
}

class UPBNetServerInterface* UPBGameplayStatics::GetNetServerInterface(UObject* WorldContextObject)
{
	UPBGameInstance* GI = Cast<UPBGameInstance>(GetGameInstance(WorldContextObject));
	return GI ? GI->GetNetServerInterface() : nullptr;
}

class UPBRoomUserInfoManager* UPBGameplayStatics::GetRoomUserInfoManager(UObject* WorldContextObject)
{
	UPBGameInstance* GI = Cast<UPBGameInstance>(GetGameInstance(WorldContextObject));
	return GI ? GI->GetRoomUserInfoManager() : nullptr;
}

FTransform UPBGameplayStatics::GetCorrectedTransformAccordingToAspectRatioAndFOV(UWorld* World, const APlayerController* PlayerController, const FTransform& Tr, const float ExpectingFOV)
{
	FTransform CorrectedTr = Tr;

	if (PlayerController && PlayerController->PlayerCameraManager && World && World->GetGameViewport())
	{
		FVector L = CorrectedTr.GetLocation();
		FVector S = CorrectedTr.GetScale3D();

		FVector2D ViewportSize;
		World->GetGameViewport()->GetViewportSize(ViewportSize);

		const float ExpectingAspectRatio = 1920.0f / 1080.0f;
		const float CurrAspectRatio = ViewportSize.X / ViewportSize.Y;

		const float ExpectingHalfFOV = FMath::DegreesToRadians(ExpectingFOV / 2.0f);
		const float CurrHalfFOV = FMath::DegreesToRadians(PlayerController->PlayerCameraManager->GetFOVAngle() / 2.0f);

		// @See: this formula is made for undoing "PerspectiveMatrix's transform" and thereafter performing the expectingAspectRatio's calculation
		L.Z *= (1.0f / CurrAspectRatio) * ExpectingAspectRatio;
		const float FOVConvertValue = FMath::Tan(CurrHalfFOV) / FMath::Tan(ExpectingHalfFOV);
		L.Z *= FOVConvertValue;
		L.Y *= FOVConvertValue;

		S *= FOVConvertValue;

		CorrectedTr.SetLocation(L);
		CorrectedTr.SetScale3D(S);
	}

	return CorrectedTr;
}

UPBPersistentUser* UPBGameplayStatics::GetPersistentUserInfo(APlayerController* Controller)
{
	if (Controller && Controller->GetLocalPlayer())
	{
		UPBLocalPlayer* LP = Cast<UPBLocalPlayer>(Controller->GetLocalPlayer());
		return LP ? LP->GetPersistentUser() : nullptr;
	}
	return nullptr;
}

UPBLocalPlayer* UPBGameplayStatics::GetPBLocalPlayer(APlayerController* Controller)
{
	return (Controller && Controller->GetLocalPlayer()) ? Cast<UPBLocalPlayer>(Controller->GetLocalPlayer()) : nullptr;
}

TArray<int32> UPBGameplayStatics::GetSavedWeapIds_Deprecated(class ULocalPlayer* LocalPlayer)
{
	TArray<int32> EmptyArray;

	if (LocalPlayer)
	{
		UPBLocalPlayer* PBLocalPlayer = Cast<UPBLocalPlayer>(LocalPlayer);
		if (PBLocalPlayer)
		{
			auto PersistUser = PBLocalPlayer->GetPersistentUser();
			if (PersistUser)
			{
				return PersistUser->GetBaseUserInfo().EquippedWeapItems;
			}
		}
	}

	return EmptyArray;
}

int32 UPBGameplayStatics::GetSavedCharId_Deprecated(class ULocalPlayer* LocalPlayer, EPBTeamType Type)
{
	if (LocalPlayer)
	{
		UPBLocalPlayer* PBLocalPlayer = Cast<UPBLocalPlayer>(LocalPlayer);
		if (PBLocalPlayer)
		{
			auto PersistUser = PBLocalPlayer->GetPersistentUser();
			if (PersistUser)
			{
				auto CharIds = PersistUser->GetBaseUserInfo().EquippedCharItems;
				if (CharIds.IsValidIndex((int32)Type))
				{
					return CharIds[(int32)Type];
				}
			}
		}
	}

	return 0;
}

EPBTeamType UPBGameplayStatics::GetTeamTypeFromCharId(UObject* WorldContextObject, int32 Id)
{
	if (WorldContextObject)
	{
		auto Mgr = UPBGameplayStatics::GetItemTableManager(WorldContextObject);
		if (Mgr)
		{
			FPBCharacterTableData* Data = Mgr->GetCharacterTableData(Id);
			if (Data)
			{
				return (EPBTeamType)Data->TeamEnum;
			}
		}
	}

	return EPBTeamType::Max;
}
