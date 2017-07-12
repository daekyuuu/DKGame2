// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UMG/PBSubWidget.h"
#include "PBSubWidget_HavePBButton.generated.h"

/**
 * 
 */
UCLASS()
class PBGAME_API UPBSubWidget_HavePBButton : public UPBSubWidget
{
	GENERATED_BODY()
	

public:

	UPROPERTY(BlueprintAssignable, Category = "Event")
		FOnPBButtonFocusedEvent OnPBButtonFocused;

	UPROPERTY(BlueprintAssignable, Category = "Event")
		FOnPBButtonReleasedEvent OnPBButtonReleased;

	// Called when the cancel key is released.
	UPROPERTY(BlueprintAssignable, Category = "Event")
		FOnPBButtonCanceledEvent OnPBButtonCanceled;

	// Called when the navigation key(left, right, up, down, L1, R1) is released.
	UPROPERTY(BlueprintAssignable, Category = "Event")
		FOnPBButtonGotNaviInputEvent OnPBButtonGotNaviInput;

	// Only use if there's lobby tab widget
	UPROPERTY(BlueprintAssignable, Category = "Event")
		FOnPBLobbyTabChanged OnPBLobbyTabChanged;

	virtual void NotifyDepthChanged(int32 NewDepth) { ensure(false); }
	virtual void NotifyNewFocusToSibling(UPBUserWidget* NewFocusedWidget) { ensure(false); }

public:
	UPBSubWidget_HavePBButton();
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual FReply NativeOnFocusReceived(const FGeometry& InGeometry, const FFocusEvent& InFocusEvent) override;
	
protected:
	virtual void BindEvents();

private:
	UFUNCTION()
		virtual void HandlePBButtonReleased(class UPBUserWidget* Widget){}

	UFUNCTION()
		virtual void HandlePBButtonGotNaviInput(class UPBUserWidget* Widget, FKey PressedKey){}

	UFUNCTION()
		virtual void HandlePBButtonFocused(class UPBUserWidget* Widget, bool bFocused){}

	UFUNCTION()
		virtual void HandlePBButtonCanceled(class UPBUserWidget* Widget){}

protected:

	template <typename TWidgetHaveHandler>
	void BindPBButtonEvents(TWidgetHaveHandler* WidgetHaveHandler, class UPBSubWidget_HavePBButton* WidgetHaveEvents)
	{
		if (WidgetHaveEvents && WidgetHaveHandler)
		{
			WidgetHaveEvents->OnPBButtonCanceled.AddUniqueDynamic(WidgetHaveHandler,		&TWidgetHaveHandler::HandlePBButtonCanceled);
			WidgetHaveEvents->OnPBButtonFocused.AddUniqueDynamic(WidgetHaveHandler,			&TWidgetHaveHandler::HandlePBButtonFocused);
			WidgetHaveEvents->OnPBButtonGotNaviInput.AddUniqueDynamic(WidgetHaveHandler,	&TWidgetHaveHandler::HandlePBButtonGotNaviInput);
			WidgetHaveEvents->OnPBButtonReleased.AddUniqueDynamic(WidgetHaveHandler,		&TWidgetHaveHandler::HandlePBButtonReleased);
		}
	}

	
};
