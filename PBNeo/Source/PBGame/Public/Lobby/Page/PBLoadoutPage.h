// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Lobby/Page/PBWidgetPage.h"
#include "PBItemInfo.h"
#include "PBLoadoutPage.generated.h"

/**
 * 
 */
UCLASS()
class PBGAME_API UPBLoadoutPage : public UPBWidgetPage
{
	GENERATED_BODY()


public:
	virtual void Init(class APlayerController* OwningPlayer, TSubclassOf<class UPBMainWidget> HUDWidgetClass) override;
	virtual void Enter() override;
	virtual void Exit() override;

	/* ------------------------------------------------------------------------------- */
	// Soldier is selected
	/* ------------------------------------------------------------------------------- */
public:
	// called when the soldier has selected in the lobby
	DECLARE_EVENT_OneParam(UPBLoadoutPage, FOnSoldierIsSelectedEvent, int32 /*ItemId*/ )
	FOnSoldierIsSelectedEvent SoldierSelectEvt;
	void OnSoldierIsSelected(int32 Id);

private:
	void SetCharacterIdToPersistanceUser(int32 Id);

	/* ------------------------------------------------------------------------------- */
	// Weapon is selected
	/* ------------------------------------------------------------------------------- */
public:
	DECLARE_EVENT_OneParam(UPBLoadoutPage, FOnWeapIsSelectedEvent, int32 /*ItemId*/)
	FOnWeapIsSelectedEvent WeapSelectEvt;
	void OnWeapIsSelected(int32 Id);
private:
	int32 GetWeapSlotFromId(int32 Id);
	void SetWeapIdToPersistanceUser(int32 Id);
	bool IsThereSameId(const TArray<int32>& IdArray, int32 CompareId);


	/* ------------------------------------------------------------------------------- */
	// Soldier list is requested.
	/* ------------------------------------------------------------------------------- */

public:
	DECLARE_EVENT_TwoParams(UPBLoadoutPage, FOnSoldierListIsReceivedEvent, EPBTeamType /*TeamType*/, const TArray<FPBItemBaseInfo>& /*SoldierList*/);
	FOnSoldierListIsReceivedEvent SoldierListIsReceivedEvt;
	void RequestSoldierList(EPBTeamType Type);
	void GetAllCharIds_Deprecated(EPBTeamType Type, TArray<int32>& OutCharIds);


private:
	void OnSoldierListIsReceived(EPBTeamType Type, const TArray<FPBItemBaseInfo>& SoldierList);


	/* ------------------------------------------------------------------------------- */
	// Weap list is requested.
	/* ------------------------------------------------------------------------------- */
public:
	DECLARE_EVENT_TwoParams(UPBLoadoutPage, FOnWeapListIsReceived, EWeaponType /*WeapType*/, const TArray<FPBItemBaseInfo>& /*WeapList*/);
	FOnWeapListIsReceived WeapListIsReceivedEvt;
	void RequestWeapList(EWeaponType Type);

private:
	void OnWeapListIsReceived(EWeaponType Type, const TArray<FPBItemBaseInfo>& WeapList);

	void GetAllWeapIds_Deprecated(EWeaponType Type, TArray<int32>& OutWeapIds);


	/* ------------------------------------------------------------------------------- */
	// NEW LOBBY(DEVELEOPMENT)
	/* ------------------------------------------------------------------------------- */
public:

	/**
	* THESE CODE MUST BE REPLACED TO A CODE, WHICH GET THE DATA FROM THE ALREADY CACHED FROM THE SERVER. 
	*/

	// seungyoon.ji
	// HACK: Just get the item id list which is set hardly.
	TArray<int32> GetPresetItemsAtSlot(int32 SlotIndex);
	
	// seungyoon.ji
	// HACK: Get the weapon list that has same type from item ID.
	TArray<int32> GetWeaponListHasSameTypeFromId(int32 ItemId);

	// seungyoon.ji
	// HACK: Get the weapon list that has same type
	TArray<int32> GetWeaponListHasSameType(EWeaponType Type);

	void GetWeaponTypeAndSlotFromId(int32 ItemId, EWeaponType& OutWeaponType, EWeaponSlot& OutWeaponSlot);


};
