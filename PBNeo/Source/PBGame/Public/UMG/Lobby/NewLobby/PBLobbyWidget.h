// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UMG/PBMainWidget.h"
#include "PBLobby.h"
#include "PBLobbyWidget.generated.h"

/**
 * 
 */
UCLASS()
class PBGAME_API UPBLobbyWidget : public UPBMainWidget
{
	GENERATED_BODY()

public:
	UPBLobbyWidget();

public:
	void SetOwningPage(class UPBWidgetPage* InOwningPage) { OwningPage = InOwningPage; }

	template<typename T>
	T* GetOwningPage() { return Cast<T>(OwningPage); }

	UFUNCTION(BlueprintNativeEvent, Category = "Lobby")
	void OnWidgetShown();

	UFUNCTION(BlueprintNativeEvent, Category = "Lobby")
	void NotifyTabIndex(int32 Index);

protected:
	UFUNCTION(BlueprintCallable, Category = "Lobby")
	void PopPage();

	UFUNCTION(BlueprintCallable, Category = "Lobby")
	void PushPage(EPageType PageType);
	
protected:
	UPROPERTY(BlueprintReadOnly, Category = "Page")
	class UPBWidgetPage* OwningPage;

};
