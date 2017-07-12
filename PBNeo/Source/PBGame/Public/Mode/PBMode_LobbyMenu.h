// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "PBMode_LobbyMenu.generated.h"

UCLASS()
class APBMode_LobbyMenu : public AGameMode
{
	GENERATED_UCLASS_BODY()

public:
	/** Initialize the game. This is called before actors' PreInitializeComponents. */
	virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;
	
	/** Returns game session class to use */
	virtual TSubclassOf<AGameSession> GetGameSessionClass() const override;

	/** skip it, menu doesn't require player start or pawn */
	virtual void RestartPlayer(class AController* NewPlayer) override;

	void SetDefaultPawnClass(TSubclassOf<APawn> PawnClass);

	virtual UClass* GetDefaultPawnClassForController_Implementation(AController* InController) override;

	/** The default pawn class used by players. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Classes)
	TSubclassOf<APawn> ActualDefaultPawnClass;

	//virtual APawn* SpawnDefaultPawnAtTransform_Implementation(AController* NewPlayer, const FTransform& SpawnTransform) override;

	//UFUNCTION()
	//APawn* SpawnCachedPawn(TSubclassOf<UClass> PawnClass, const FTransform& SpawnTransform);

	//struct FSpawnedRoomCharacter
	//{
	//	APawn* Character;
	//	bool bUsed;		

	//	FSpawnedRoomCharacter()
	//	{
	//		bUsed = false;
	//		Character = nullptr;
	//	}
	//};

	//TMultiMap<UClass*, FSpawnedRoomCharacter> CachedRoomCharacterMap;
};
