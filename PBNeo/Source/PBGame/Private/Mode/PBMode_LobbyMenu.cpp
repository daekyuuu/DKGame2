// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "PBGame.h"
#include "Mode/PBMode_LobbyMenu.h"
#include "PBGameInstance.h"
#include "Online/PBGameSession.h"
#include "Player/PBPlayerController_Menu.h"
#include "PBLobbySpectatorPawn.h"

APBMode_LobbyMenu::APBMode_LobbyMenu(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	PlayerControllerClass = APBPlayerController_Menu::StaticClass();
	DefaultPawnClass = APawn::StaticClass(); // do not allow defaultpawn movement processing
	SpectatorClass = APBLobbySpectatorPawn::StaticClass();
}

//engine override
void APBMode_LobbyMenu::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);
}

//engine override
void APBMode_LobbyMenu::RestartPlayer(class AController* NewPlayer)
{

}

void APBMode_LobbyMenu::SetDefaultPawnClass(TSubclassOf<APawn> PawnClass)
{
	ActualDefaultPawnClass = PawnClass == nullptr ? DefaultPawnClass : PawnClass;
}

UClass* APBMode_LobbyMenu::GetDefaultPawnClassForController_Implementation(AController* InController)
{
	return ActualDefaultPawnClass;
}
//
//APawn* APBMode_LobbyMenu::SpawnDefaultPawnAtTransform_Implementation(AController* NewPlayer, const FTransform& SpawnTransform)
//{
//	UClass* PawnClass = GetDefaultPawnClassForController(NewPlayer);
//	return SpawnCachedPawn(PawnClass, SpawnTransform);
//}
//
//APawn* APBMode_LobbyMenu::SpawnCachedPawn(TSubclassOf<UClass> PawnClass, const FTransform& SpawnTransform)
//{
//	TArray<FSpawnedRoomCharacter> SpawnPawns;
//	CachedRoomCharacterMap.MultiFind(PawnClass, SpawnPawns);
//
//	// Find in the Cached 
//	for (auto CachedSpawnInfo : SpawnPawns)
//	{
//		if (CachedSpawnInfo.Character && !CachedSpawnInfo.bUsed)
//		{
//			CachedSpawnInfo.Character->SetActorTransform(SpawnTransform);
//			CachedSpawnInfo.bUsed = true;
//			return CachedSpawnInfo.Character;
//		}
//	}
//
//	FActorSpawnParameters SpawnInfo;
//	SpawnInfo.Instigator = Instigator;
//	SpawnInfo.ObjectFlags |= RF_Transient;	// We never want to save default player pawns into a map
//
//	APawn* ResultPawn = GetWorld()->SpawnActor<APawn>(PawnClass, SpawnTransform, SpawnInfo);
//	if (!ResultPawn)
//	{
//		UE_LOG(LogGameMode, Warning, TEXT("Couldn't spawn Pawn of type %s at %s"), *GetNameSafe(PawnClass), *SpawnTransform.ToHumanReadableString());
//	}
//	
//	FSpawnedRoomCharacter CacheSpawnInfo;
//	CacheSpawnInfo.Character = ResultPawn;
//	CacheSpawnInfo.bUsed = true;
//	CachedRoomCharacterMap.Add(PawnClass, CacheSpawnInfo);
//
//	return ResultPawn;
//}

TSubclassOf<AGameSession> APBMode_LobbyMenu::GetGameSessionClass() const
{
	return APBGameSession::StaticClass();
}
