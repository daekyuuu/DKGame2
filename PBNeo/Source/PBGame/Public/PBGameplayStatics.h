// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Kismet/GameplayStatics.h"
#include "PBGameplayStatics.generated.h"

//namespace PBGameDebug
//{
//	USTRUCT()
//	struct Sphere
//	{
//		FString HalfRange;
//	};
//}


/**
 * 
 */
UCLASS()
class PBGAME_API UPBGameplayStatics : public UGameplayStatics
{
	GENERATED_BODY()

public:

	// Owner 를 전달해서 USoundNodeLocalPlayer in SoundCue 와 연계해서 사용한다.
	UFUNCTION(BlueprintCallable, Category = "Audio", meta = (WorldContext = "WorldContextObject", AdvancedDisplay = "3", UnsafeDuringActorConstruction = "true", Keywords = "play"))
	static class UAudioComponent* SpawnSoundAtLocation(UObject* WorldContextObject, AActor* PawnOwner, class USoundBase* Sound, FVector Location, FRotator Rotation = FRotator::ZeroRotator, float VolumeMultiplier = 1.f, float PitchMultiplier = 1.f, float StartTime = 0.f, class USoundAttenuation* AttenuationSettings = nullptr, class USoundConcurrency* ConcurrencySettings = nullptr);
	
	static void ShowDebugSphere(UObject* WorldContextObject, const FVector& Center, float Radius, FColor const& Color, float Lifetime);
	static void ShowDebugWeaponTrace(UObject* WorldContextObject, const FVector& LineStart, const FVector& LineEnd, FColor const& Color, float Lifetime, float Tickness = 1.0f, int32 LineNumber=0);
	static void DisplayWeaponItemList(UObject* WorldContextObject);

	// reimplementation of UGameplayStatics. 
	// @param OutDamagedActors is added
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Game|Damage", meta = (WorldContext = "WorldContextObject", AutoCreateRefTerm = "IgnoreActors"))
	static bool ApplyRadialDamage(UObject* WorldContextObject, float BaseDamage, const FVector& Origin, float DamageRadius, TSubclassOf<class UDamageType> DamageTypeClass,
		const TArray<AActor*>& IgnoreActors, TArray<FTakeDamageInfo>& OutDamagedActors, AActor* DamageCauser = NULL, AController* InstigatedByController = NULL, bool bDoFullDamage = false, ECollisionChannel DamagePreventionChannel = ECC_Visibility);

	// reimplementation of UGameplayStatics. 
	// @param OutDamagedActors is added
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Game|Damage", meta = (WorldContext = "WorldContextObject", AutoCreateRefTerm = "IgnoreActors"))
	static bool ApplyRadialDamageWithFalloff(UObject* WorldContextObject, float BaseDamage, float MinimumDamage, const FVector& Origin, float DamageInnerRadius, 
		float DamageOuterRadius, float DamageFalloff, TSubclassOf<class UDamageType> DamageTypeClass, const TArray<AActor*>& IgnoreActors, TArray<FTakeDamageInfo>& OutDamagedActors, AActor* DamageCauser = NULL,
		AController* InstigatedByController = NULL, ECollisionChannel DamagePreventionChannel = ECC_Visibility);

	static bool ComponentIsDamageableFrom(UPrimitiveComponent* VictimComp, FVector const& Origin, AActor const* IgnoredActor, const TArray<AActor*>& IgnoreActors, ECollisionChannel TraceChannel, FHitResult& OutHitResult);

	static float InterpDelayToInterpSpeed(float Input);

	static class UPBItemTableManager* GetItemTableManager(UObject* WorldContextObject);
	static class UPBGameTableManager* GetGameTableManager(UObject* WorldContextObject);
	static class UPBPacketManager* GetPacketManager(UObject* WorldContextObject);
	static class UPBNetClientInterface* GetNetClientInterface(UObject* WorldContextObject);
	static class UPBNetServerInterface* GetNetServerInterface(UObject* WorldContextObject);
	static class UPBRoomUserInfoManager* GetRoomUserInfoManager(UObject* WorldContextObject);

	static FTransform GetCorrectedTransformAccordingToAspectRatioAndFOV(UWorld* World, const APlayerController* PlayerController, const FTransform& Tr, const float ExpectingFOV);

	static class UPBPersistentUser* GetPersistentUserInfo(APlayerController* Controller);
	static class UPBLocalPlayer* GetPBLocalPlayer(APlayerController* Controller);

	static TArray<int32> GetSavedWeapIds_Deprecated(class ULocalPlayer* LocalPlayer);
	static int32 GetSavedCharId_Deprecated(class ULocalPlayer* LocalPlayer, EPBTeamType Type);

	static EPBTeamType GetTeamTypeFromCharId(UObject* WorldContextObject, int32 Id);

};
