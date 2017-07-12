// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine/LevelScriptActor.h"
#include "PBLevelScriptActor.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnLevelScriptActorEvent);

USTRUCT(BlueprintType)
struct FLevelSequenceElement
{
	GENERATED_BODY()

	//레벨시퀀스의 이름. 어셋 이름과 일치해야만 원하는 타이밍에 재생할 수 있다.
	UPROPERTY()
	FString Name;

	//분명 이게 어떤 씬인지 즉석해서 알 길이 없을 것이다. 귀찮아도 설명을 달아두면 추후에 도움이 될 것이다.
	UPROPERTY()
	FString Description;

	UPROPERTY()
	class ULevelSequence *LevelSequence;
};

/**
 * 
 */
UCLASS()
class PBGAME_API APBLevelScriptActor : public ALevelScriptActor
{
	GENERATED_BODY()

public:
	APBLevelScriptActor();

public:
	virtual void BeginPlay() override;
	virtual void BeginDestroy() override;

public:
	class ULevelSequence *GetLevelSequence(const FString &SequenceName);
	class ULevelSequencePlayer *CreateLevelSequencePlayer(const FString &SequenceName);

	UFUNCTION(Reliable, NetMulticast, WithValidation)
	void MulticastPlayLevelSequence(const FString &SequenceName);

	/* Functions for Cinematic Event Track (DO NOT RENAME OR REMOVE) */
	UFUNCTION()
	void DisplayResult();
	FOnLevelScriptActorEvent OnDisplayResult;

protected:
	//레벨에 대한 설명을 달아두면 추후에 이어서 작업하기 좋을 것이다.
	UPROPERTY(EditDefaultsOnly, Category = "Level")
	FString Description;
	//레벨시퀀스 맵. 여기에 등록해두고, 필요할 때마다 편하게 꺼내쓰는 용도.
	UPROPERTY(EditDefaultsOnly, Category = "Game|Cinematic")
	TMap<FString, class ULevelSequence *> LevelSequenceMap;
};
