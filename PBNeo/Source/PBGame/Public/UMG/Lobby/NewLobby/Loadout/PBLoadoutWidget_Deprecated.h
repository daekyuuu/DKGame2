// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UMG/Lobby/NewLobby/PBLobbyWidget.h"
#include "PBLoadoutWidget_Deprecated.generated.h"

/**
 * 
 */
UCLASS()
class PBGAME_API UPBLoadoutWidget_Deprecated : public UPBLobbyWidget
{
	GENERATED_BODY()


public:

	PBGetterWidgetBlueprintVariable(UPBLoadout_SoldierTab, SoldierTab);
	PBGetterWidgetBlueprintVariable(UPBLoadout_WeapTab, WeaponTab);
	PBGetterWidgetBlueprintVariable(UPBLoadout_WeapTab, SpecialTab);

public:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	void BindEvents();
	
private:

	UFUNCTION()
	virtual void HandlePBButtonCanceled(class UPBUserWidget* Widget);


protected:
	UFUNCTION(BlueprintCallable, Category = "Loadout")
		void OnSoldierIsSelected(int32 Id);

	UFUNCTION(BlueprintCallable, Category = "Loadout")
		void OnWeapIsSelected(int32 Id);

	/* ------------------------------------------------------------------------------- */
	// Request soldier list
	/* ------------------------------------------------------------------------------- */

	UFUNCTION(BlueprintCallable, Category = "Loadout")
		void RequestSoldierList(EPBTeamType Type);

	UFUNCTION(BlueprintImplementableEvent, Category = "Loadout")
		void NotifySoldierListIsRecevied(EPBTeamType Type, const TArray<FPBItemBaseInfo>& SoldierList);

	/* ------------------------------------------------------------------------------- */
	// Request weap list
	/* ------------------------------------------------------------------------------- */

	UFUNCTION(BlueprintCallable, Category = "Loadout")
		void RequestWeapList(int32 Type);

	UFUNCTION(BlueprintImplementableEvent, Category = "Loadout")
		void NotifyWeapListIsRecevied(EWeaponType Type, const TArray<FPBItemBaseInfo>& WeapList);
	
};
