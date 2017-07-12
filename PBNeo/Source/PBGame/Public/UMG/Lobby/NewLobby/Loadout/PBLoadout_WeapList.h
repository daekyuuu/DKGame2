// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UMG/PBSubWidget.h"
#include "PBLoadout_WeapList.generated.h"

/**
 * 
 */
UCLASS()
class PBGAME_API UPBLoadout_WeapList : public UPBSubWidget_HavePBButton
{
	GENERATED_BODY()


public:
	PBGetterWidgetBlueprintVariable(UScrollBox, RootScrollBox);
	

public:
	UPBLoadout_WeapList();
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;


public:

	UFUNCTION(BlueprintCallable, Category = "Build")
	void BuildWeapList(const TArray<FPBItemBaseInfo>& WeapInfos, TSubclassOf<class UPBLoadout_WeapVector4> WeapVector4Class);

	bool IsNavigatable();

	virtual void NotifyDepthChanged(int32 NewDepth) override;
	virtual void NotifyNewFocusToSibling(UPBUserWidget* NewFocusedWidget) override;
	
private:
	void BuildPBWidgets(const TArray<FPBItemBaseInfo>& WeapInfos, TSubclassOf<class UPBLoadout_WeapVector4> WeapVector4Class);

	virtual FReply NativeOnFocusReceived(const FGeometry& InGeometry, const FFocusEvent& InFocusEvent) override;
	void SetFocusTransferTarget(UPBUserWidget* Widget);
	void SetLastFocusedWeapId(UPBUserWidget* Widget);

	void NavigateToNext(bool bInverseDir);
	void NavigateToUp(bool bInverseDir);

	class UPBLoadout_Weap* FindWeapFromId(int32 Id);
	TArray<UPBLoadout_WeapVector4*> GetRows();

public:
	virtual void BindEvents() override;

	UFUNCTION()
		virtual void HandlePBButtonReleased(UPBUserWidget* Widget) override;

	UFUNCTION()
		virtual void HandlePBButtonCanceled(UPBUserWidget* Widget) override;

	UFUNCTION()
		virtual void HandlePBButtonFocused(UPBUserWidget* Widget, bool bFocused) override;

	UFUNCTION()
		virtual void HandlePBButtonGotNaviInput(UPBUserWidget* Widget, FKey PressedKey) override;



private:

	UPROPERTY()
	class UPBLoadout_WeapVector4* RowContainingFocus;

	int32 LastFocusedWeapId;
	
};
