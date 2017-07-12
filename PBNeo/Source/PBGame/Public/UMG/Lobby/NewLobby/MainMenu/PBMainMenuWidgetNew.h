// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UMG/PBMainWidget.h"
#include "PBMainMenuWidgetNew.generated.h"


USTRUCT(Blueprintable)
struct FMainMenuSubButtonData
{
	GENERATED_BODY()
		
public:

	// The target page to go when you press the button.
	UPROPERTY(EditAnywhere, Category = "Meta")
	EPageType TargetPage;

	// Target index of tab of page which you want to show up at first.
	UPROPERTY(EditAnywhere, Category = "Meta")
	int32 TargetTabIndex;

	UPROPERTY(EditAnywhere, Category = "Meta")
	FText ButtonText;


	FMainMenuSubButtonData()
	{
		TargetPage = EPageType::None;
		TargetTabIndex = 0;
		ButtonText = FText::FromString("ButtonText");
	}

};


USTRUCT(Blueprintable)
struct FMainMenuSubButtonDataSet
{
	GENERATED_BODY()

public:

	// The target page to go when you press the button.
	UPROPERTY(EditAnywhere, Category = "Meta")
		TArray<FMainMenuSubButtonData> DataArray;

};


/**
 * 
 */
UCLASS()
class PBGAME_API UPBMainMenuWidgetNew : public UPBLobbyWidget
{
	GENERATED_BODY()
public:
	PBGetterWidgetBlueprintVariable(UCanvasPanel, MainButtonCanvas);
	PBGetterWidgetBlueprintVariable(UCanvasPanel, SubButtonCanvas);
	PBGetterWidgetBlueprintVariable(UCanvasPanel, SubButtonSlots);


public:

	UPBMainMenuWidgetNew();
	
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual FReply NativeOnFocusReceived(const FGeometry& InGeometry, const FFocusEvent& InFocusEvent) override;

	void BindEvents();

	class UPBUserWidget* GetLastFocusedMainButton() const { return LastFocusedMainButton; }


	
private:

	void SetOwnerOfButtonsOfCanvas(class UCanvasPanel* CanvasPanel);

	void BindEventsOfButtonsOfCanvas(class UCanvasPanel* CanvasPanel);

	void BroadcastDepthChanged(int32 NewDepth);

	void SetCurrDepth(int32 NewDepth);

	UPBUserWidget* GetFirstButtonOfMenu();

	/**
	* Change the location of SubButtons according which button is currently focused.
	*/
	void ChangeLocationOfSubButtons(class UPBUserWidget* FocusedButton);

	bool HasSubButtons(class UPBUserWidget* Widget);

	void BuildSubButtons(class UPBUserWidget* Widget);

	void RequestToPushPage(class UPBUserWidget* Widget);

	bool IsTheMainButton(class UPBUserWidget* Widget);
	bool IsTheSubButton(class UPBUserWidget* Widget);

	void ShowSubButtons();
	void HideSubButtons();

	UFUNCTION()
		virtual void HandlePBButtonReleased(class UPBUserWidget* Widget);

	UFUNCTION()
		virtual void HandlePBButtonGotNaviInput(class UPBUserWidget* Widget, FKey PressedKey);

	UFUNCTION()
		virtual void HandlePBButtonFocused(class UPBUserWidget* Widget, bool bFocused);

	UFUNCTION()
		virtual void HandlePBButtonCanceled(UPBUserWidget* Widget);

protected:
	UPROPERTY()
	class UPBUserWidget* LastFocusedMainButton;

	UPROPERTY(EditDefaultsOnly, Category = "Button")
		TArray< FMainMenuSubButtonDataSet > SubButtonDataSetArray;

	UPROPERTY()
		int32 CurrDepth;

};
