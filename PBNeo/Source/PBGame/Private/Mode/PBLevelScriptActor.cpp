// Fill out your copyright notice in the Description page of Project Settings.

#include "PBGame.h"
#include "PBLevelScriptActor.h"
#include "PBGameInstance.h"

APBLevelScriptActor::APBLevelScriptActor()
{
}

void APBLevelScriptActor::BeginPlay()
{
	Super::BeginPlay();
}

void APBLevelScriptActor::BeginDestroy()
{
	Super::BeginDestroy();
}

ULevelSequence *APBLevelScriptActor::GetLevelSequence(const FString &SequenceName)
{
	//for (FLevelSequenceElement const &CurSeq : LevelSequenceArray)
	//{
	//	if (CurSeq.Name == SequenceName)
	//		return CurSeq.LevelSequence;
	//}
	auto FoundObject = LevelSequenceMap.Find(SequenceName);
	if (nullptr == FoundObject)
		return nullptr;

	return *FoundObject;
}

class ULevelSequencePlayer *APBLevelScriptActor::CreateLevelSequencePlayer(const FString &SequenceName)
{
	ULevelSequence *LevelSequence = GetLevelSequence(SequenceName);
	if (false == ensureMsgf(nullptr != LevelSequence, *FString::Printf(TEXT("%s could not find from class PBLevelScriptActor::LevelSequenceMap, you have to add a reference to that Map."), *SequenceName)))
		return nullptr;

	FMovieSceneSequencePlaybackSettings LevelSequencePlaybackSettings;
	ULevelSequencePlayer *Player = ULevelSequencePlayer::CreateLevelSequencePlayer(this, LevelSequence, LevelSequencePlaybackSettings);

	return Player;
}

bool APBLevelScriptActor::MulticastPlayLevelSequence_Validate(const FString &SequenceName)
{
	return true;
}

void APBLevelScriptActor::MulticastPlayLevelSequence_Implementation(const FString &SequenceName)
{
	ULevelSequencePlayer *Player = CreateLevelSequencePlayer(SequenceName);
	
	if (Player)
	{
		Player->Play();
	}
}

void APBLevelScriptActor::DisplayResult() 
{
	OnDisplayResult.Broadcast(); 
}