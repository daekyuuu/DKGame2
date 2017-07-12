// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "PBSwitcherBasedButton.h"

#include "PBItemInfo.h"
#include "PBLoadout_SoldierBtn.generated.h"

USTRUCT(Blueprintable)
struct FPBResource_Loadout_Soldier
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadOnly)
		bool bSucceedToSet;

	UPROPERTY(BlueprintReadOnly)
		FString ItemName;

	UPROPERTY(BlueprintReadOnly)
		UTexture2D* ItemIcon;

	FPBResource_Loadout_Soldier()
		:bSucceedToSet(false),
		ItemName(""),
		ItemIcon(nullptr)
	{

	}
};

/**
 *
 */
UCLASS()
class PBGAME_API UPBLoadout_SoldierBtn : public UPBSwitcherBasedButton
{
	GENERATED_BODY()

public:

	PBGetterWidgetBlueprintVariable(UTextBlock, CharName0);
	PBGetterWidgetBlueprintVariable(UTextBlock, CharName1);
	PBGetterWidgetBlueprintVariable(UTextBlock, CharName2);
	PBGetterWidgetBlueprintVariable(UTextBlock, CharName3);

	PBGetterWidgetBlueprintVariable(UImage, CharImg0);
	PBGetterWidgetBlueprintVariable(UImage, CharImg1);
	PBGetterWidgetBlueprintVariable(UImage, CharImg2);
	PBGetterWidgetBlueprintVariable(UImage, CharImg3);

public:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	void AssignItemInfo(const FPBItemBaseInfo& InItemInfo);

	UFUNCTION(BlueprintCallable, Category = "Loadout")
		int32 GetItemId() const;


private:
	void ApplyItemInfo();

	void BuildPropertyArray();

	UFUNCTION(BlueprintCallable, Category = "Loadout")
		FPBResource_Loadout_Soldier GetResourcesFromId(int32 Id);

private:
		FPBItemBaseInfo ItemInfo;

	UPROPERTY()
		TArray<class UImage*> CharImgArray;

	UPROPERTY()
		TArray<class UTextBlock*> CharNameArray;

};
