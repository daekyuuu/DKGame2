// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UMG/Lobby/NewLobby/Loadout/PBSubWidget_HavePBButton.h"
#include "PBLobbyTabWidget.generated.h"

/**
 * 
 */
UCLASS()
class PBGAME_API UPBLobbyTabWidget : public UPBSubWidget_HavePBButton
{
	GENERATED_BODY()
	
public:
	PBGetterWidgetBlueprintVariable(UHorizontalBox, TabButtonHorizontalBox);
	PBGetterWidgetBlueprintVariable(UCanvasPanel, R1Canvas);

public:
	UPBLobbyTabWidget();
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	virtual FReply NativeOnFocusReceived(const FGeometry& InGeometry, const FFocusEvent& InFocusEvent) override;
	virtual void DesignTimeConstruct_Implementation() override;

public:

	/**
	* Rotate the tab. It will raise the event OnPBLobbyTabRotated.
	*/
	void RotateToRight();
	void RotateToLeft();

	/**
	* Set the active index directly instead of rotation using Index.
	*/
	void JumpTo(int32 Index);

private:
	void Rotate(bool bDirectionR);
	void JumpToInternal(int32 Index);

private:

	void BindEvents();
	void BuildSubButtons();
	void UpdateR1CanvasPosition();
	void ActivateTabButton(int32 ButtonIndex, bool bActive);

	UFUNCTION()
	virtual void HandlePBButtonFocused(class UPBUserWidget* Widget, bool bFocused) override;

	UFUNCTION()
	virtual void HandlePBButtonCanceled(class UPBUserWidget* Widget) override;

protected:
	UPROPERTY()
	int32 ActivatedButtonIndex;

	UPROPERTY(EditAnywhere, Category = "LobbyTab")
	TArray<FText> TabButtonTextArray;

	UPROPERTY(EditDefaultsOnly, Category = "LobbyTab")
	TSubclassOf<class UPBLobbyTabButton> TabButtonClass;
	
	
};
