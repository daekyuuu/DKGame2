// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UMG/Widgets/PBSwitcherBasedButton.h"
#include "PBLoadout_WeapTypeBtn.generated.h"

/**
 * 
 */
UCLASS()
class PBGAME_API UPBLoadout_WeapTypeBtn : public UPBSwitcherBasedButton
{
	GENERATED_BODY()
	
public:
	
	PBGetterWidgetBlueprintVariable(UTextBlock, Text0);
	PBGetterWidgetBlueprintVariable(UTextBlock, Text1);
	PBGetterWidgetBlueprintVariable(UTextBlock, Text2);
	PBGetterWidgetBlueprintVariable(UTextBlock, Text3);


public:
	UPBLoadout_WeapTypeBtn();


	virtual void NativeConstruct() override;
	virtual void SynchronizeProperties() override;

	void SetButtonText(FText Text);
	void SetWeapTypeId(int32 NewId);

	int32 GetWeapTypeId() const;

private:
	void SyncText();


private:
	FText ButtonText;

	int32 WeapTypeId;

	
};
