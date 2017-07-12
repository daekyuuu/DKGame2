// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UMG/PBMainWidget.h"
#include "PBMultiplayPage.h"

#include "PBMultiplayWidget.generated.h"

/**
 * 
 */
UCLASS()
class PBGAME_API UPBMultiplayWidget : public UPBLobbyWidget
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, Category = "Multiplay")
	void OnModeChanged(int32 Index);
	
	UFUNCTION(BlueprintCallable, Category = "Multiplay")
	void OnMemberChanged(int32 Index);

	UFUNCTION(BlueprintCallable, Category = "Multiplay")
	void OnTimeChanged(int32 Index);

	UFUNCTION(BlueprintCallable, Category = "Multiplay")
	void OnRoundChanged(int32 Index);
	
	UFUNCTION(BlueprintCallable, Category = "Multiplay")
	EPBAsyncResult OnCreateRoom();

	UFUNCTION(BlueprintCallable, Category = "Multiplay")
	EPBAsyncResult OnSearchServerToJoin();

	UFUNCTION(BlueprintCallable, Category = "Multiplay")
	EPBAsyncResult OnConnectToServer(const struct FPBRoomEntry& ServerEntry);

};
