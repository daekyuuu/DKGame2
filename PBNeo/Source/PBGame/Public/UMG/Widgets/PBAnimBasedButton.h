// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "PBSubWidget_HavePBButton.h"
#include "PBAnimBasedButton.generated.h"

/**
 * 
 */
UCLASS()
class PBGAME_API UPBAnimBasedButton : public UPBSubWidget_HavePBButton
{
	GENERATED_BODY()

public:

	/**
	* NOTE:
	* ButtonHit is the invisible button to handle the mouse events.
	* In other hand, The focus events that come from gamepad will be handled by the widget itself overriding NativeOnFocusXXX() functions.
	* Since the focus event will be handled by only root widget, UButton can not be focusable
	* For the same reason, PBAnimBasedWidget itself is not clickable(ESlateVisibility::SelfHitTestInvisible). ButtonHit is only clickable.
	*/
	PBGetterWidgetBlueprintVariable(UButton, ButtonHit);

	PBGetterWidgetBlueprintVariable(UTextBlock, ButtonTitle);


public:

	UPBAnimBasedButton();

	virtual void Show() override;
	virtual void Hide() override;

	virtual void NotifyDepthChanged(int32 NewDepth) override;
	virtual void UpdateButtonLook();

	void SetOwningPBUserWidget(class UPBUserWidget* Widget) { OwningPBUserWidget = Widget; }

protected:

	// I've override it to block the ue4 default widget navigation system.
	virtual void OnWidgetRebuilt() override;

	virtual void DesignTimeConstruct_Implementation() override;

	void UpdateTitleText();

	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	virtual FReply NativeOnFocusReceived(const FGeometry& InGeometry, const FFocusEvent& InFocusEvent) override;
	virtual void NativeOnFocusLost(const FFocusEvent& InFocusEvent) override;

	virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;
	virtual FReply NativeOnKeyUp(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;
	
	UFUNCTION()
	void HandleButtonHitClicked();

	UFUNCTION()
	void HandleButtonHitHovered();

	/*
	* Play Button animation.
	* Unlike the UE4 PlayAnimation(), It store the last played animation and stop it before playing new animation.
	**/
	void PlayAnimationIfNeed();
	

protected:

	// The widget who containing this widget.
	UPROPERTY()
	class UPBUserWidget* OwningPBUserWidget;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Button")
		FText TitleText;
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Resource")
		USoundCue* PressedSound;

protected:
	UPROPERTY()
		class UWidgetAnimation* AnimToPlay;

	// My depth in the ui layout.(ex: In the MainMenu, every main buttons are depth 0 and every sub buttons are depth 1)
	UPROPERTY(EditAnywhere, Category = "Depth")
		int32 MyDepth;

	// Is this widget In the same depth with the current focused widget? 
	UPROPERTY()
		bool bInSameDepth;

};
