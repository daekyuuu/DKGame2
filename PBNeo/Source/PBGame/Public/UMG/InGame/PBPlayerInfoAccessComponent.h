// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Object.h"
#include "PBPlayerInfoAccessComponent.generated.h"

/**
 * 
 */
UCLASS()
class PBGAME_API UPBPlayerInfoAccessComponent : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY()
	class APlayerController* LocalPlayerController;

	UPROPERTY(BlueprintReadWrite, Category = "MemberInfo")
	class APBPlayerState* TargetPlayerState;

public:
	UFUNCTION(BlueprintCallable, category = "MemberInfo")
		FText GetUserName() const;

	UFUNCTION(BlueprintCallable, category = "MemberInfo")
		float GetHP() const;

	UFUNCTION(BlueprintCallable, category = "MemberInfo")
		FText GetKill() const;

	UFUNCTION(BlueprintCallable, category = "MemberInfo")
		FText GetKillAndDeath() const;

	UFUNCTION(BlueprintCallable, category = "MemberInfo")
		FText GetAssist() const;

	UFUNCTION(BlueprintCallable, category = "MemberInfo")
		FText GetDeath() const;
	
	UFUNCTION(BlueprintCallable, category = "MemberInfo")
		FText GetScore() const;	

	UFUNCTION(BlueprintCallable, category = "MemberInfo")
		bool IsItLocalPlayerInfo() const;

	UFUNCTION(BlueprintCallable, category = "MemberInfo")
		bool IsDie() const;



public:
	class APBPlayerState* GetLocalPlayerState() const;
	
	
};
