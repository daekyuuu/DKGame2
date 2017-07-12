// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UMG/PBUserWidget.h"
#include "GameFramework/Actor.h"
#include "PB3DWidgetRelay.generated.h"

UCLASS()
class AWidgetInteractor : public AActor
{
	GENERATED_BODY()

	AWidgetInteractor();

public:
	UPROPERTY()
	class UWidgetInteractionComponent* InteractionComp;
};

UCLASS()
class PBGAME_API UPB3DWidgetRelay : public UPBUserWidget
{
	GENERATED_BODY()

	UPB3DWidgetRelay(const FObjectInitializer& ObjectInitializer);
	
	void NativeConstruct() override;

	void NativeDestruct() override;

	/* ------------------------------------------------------------------------------- */
	// Manages Interaction Actor
	/* ------------------------------------------------------------------------------- */

public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Interaction, meta = (ClampMin = "0", UIMin = "0"))
	float MaxInteractionDistance;

protected: // For modules on top

	UWidgetInteractionComponent* GetWidgetIntereacter();

protected:

	void InitWidgetInteractorSys();

	void SpawnWidgetInteractor();

	void DestroyWidgetInteractor();

	UPROPERTY()
	AWidgetInteractor* WidgetInteractor;

	/* ------------------------------------------------------------------------------- */
	// Relaying Interactions
	/* ------------------------------------------------------------------------------- */

protected:

	FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;

	FReply NativeOnKeyUp(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;

	FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

	FReply NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

	void NativeOnFocusLost(const FFocusEvent& InFocusEvent) override;

	/* ------------------------------------------------------------------------------- */
	// Checking Virtual Focus
	/* ------------------------------------------------------------------------------- */

public:

	UFUNCTION(BlueprintCallable, Category = WidgetFocus)
	bool ContainVirtualUserFocus(UWidget* Widget);

	bool ContainVirtualUserFocus(SWidget& Widget);

	/* ------------------------------------------------------------------------------- */
	// End
	/* ------------------------------------------------------------------------------- */
	
};
