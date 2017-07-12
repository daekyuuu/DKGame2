// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UMG/PBSubWidget.h"
#include "PBItemInfo.h"
#include "PBSwitcherBasedButton.h"
#include "PBLoadout_Weap.generated.h"


USTRUCT(Blueprintable)
struct FPBResource_Loadout_Weap
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadOnly)
	bool bSucceedToSet;

	UPROPERTY(BlueprintReadOnly)
	FString ItemName;

	UPROPERTY(BlueprintReadOnly)
	UTexture2D* ItemIcon;

	FPBResource_Loadout_Weap()
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
class PBGAME_API UPBLoadout_Weap : public UPBSwitcherBasedButton
{
	GENERATED_BODY()
public:

	PBGetterWidgetBlueprintVariable(UTextBlock, WeapName0);
	PBGetterWidgetBlueprintVariable(UTextBlock, WeapName1);
	PBGetterWidgetBlueprintVariable(UTextBlock, WeapName2);
	PBGetterWidgetBlueprintVariable(UTextBlock, WeapName3);

	PBGetterWidgetBlueprintVariable(UImage, WeapImg0);
	PBGetterWidgetBlueprintVariable(UImage, WeapImg1);
	PBGetterWidgetBlueprintVariable(UImage, WeapImg2);
	PBGetterWidgetBlueprintVariable(UImage, WeapImg3);

public:
	UPBLoadout_Weap();

	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual void UpdateButtonLook() override;
	virtual FReply NativeOnFocusReceived(const FGeometry& InGeometry, const FFocusEvent& InFocusEvent) override;
	virtual void NotifyDepthChanged(int32 NewDepth) override;


	bool IsNavigatable() const;

	void AssignItemInfo(const FPBItemBaseInfo& Info);

	UFUNCTION(BlueprintCallable, Category = "WeapBtn")
	int32 GetItemId() const;

private:
	void OnItemInfoAssigned(const FPBItemBaseInfo& Info);

	FPBResource_Loadout_Weap GetResourcesFromId(int32 id);

	void BuildPropertyArray();

private:
	int32 ItemId;

	UPROPERTY()
	TArray<class UImage*> WeapImgArray;

	UPROPERTY()
	TArray<class UTextBlock*> WeapNameArray;

};
