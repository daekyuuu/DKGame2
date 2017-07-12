// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "SlateWrapperTypes.h"
#include "PBUMGUtils.generated.h"

/**
 *
 */
UCLASS(Blueprintable)
class PBGAME_API UPBUMGUtils : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:

	//////////////////////////////////////////////////////////////////////////
	// Focus and scroll

	/**
	* Navigate the focus inside scrollbox widget.
	*
	* @Param Player: Local player who has focus
	* @Param ScrollBox: Target Scrollbox
	* @Param bInverseDir: if true, navigate to backward.
	* @Param OffsetForFocus: When this function navigate the focus, if there's offset value then will jump the index as much as offset.
	* @Param OffsetForScroll: When this function Scroll to widget, if there's offset value then will jump the index as much as offset.
	*/
	UFUNCTION(BlueprintCallable, Category = "UMGUtil")
		static class UWidget* NavigateToNextInScrollBox(class ULocalPlayer* Player, class UScrollBox* ScrollBox, bool bInverseDir = false, int32 OffsetForFocus = 0, int32 OffsetForScroll = 0);

	UFUNCTION(BlueprintCallable, Category = "UMGUtil")
		static class UWidget* NavigateToNextInScrollBoxes(class ULocalPlayer* Player, const TArray<class UScrollBox*>& ScrollBoxArray, bool bInverseDir = false, int32 OffsetForFocus = 0, int32 OffsetForScroll = 0);

	UFUNCTION(BlueprintCallable, Category = "UMGUtil")
		static class UWidget* NavigateToNextInHorizontalBox(class ULocalPlayer* Player, class UHorizontalBox* HorizontalBox, bool bInverseDir = false, int32 OffsetForFocus = 0);

	UFUNCTION(BlueprintCallable, Category = "UMGUtil")
		static class UWidget* NavigateToNextInPanel(class ULocalPlayer* Player, class UPanelWidget* PanelWidget, bool bInverseDir = false, int32 OffsetForFocus = 0);

	UFUNCTION(BlueprintCallable, Category = "UMGUtil")
		static class UWidget* ScrollWidgetIntoViewWithOffset(class UScrollBox* ScrollBox ,UWidget* WidgetToFind, bool AnimateScroll = true, bool bInverseDir = false, int32 OffsetForScroll = 0);

	/**
	* Navigate focus from scrollBox To other scrollBox.
	* it consider layout and navigate to closest index of scroll.
	* @Param ScrollOffsetForOneStep: Scroll amount at one scroll
	* @Param OffsetForScroll: The value which you used in NavigateToNextInScrollBox()
	*/
	UFUNCTION(BlueprintCallable, Category = "UMGUtil")
		static class UWidget* NavigateToOtherScrollBox(class ULocalPlayer* Player, class UScrollBox* FromScrollBox, class UScrollBox* ToScrollBox, float ScrollOffsetForOneStep, int32 OffsetForFocus = 0);

	UFUNCTION(BlueprintCallable, Category = "UMGUtil")
		static class UWidget* NavigateToOtherPannelAtSameIndex(class ULocalPlayer* Player, class UPanelWidget* From, class UPanelWidget* To);

	//////////////////////////////////////////////////////////////////////////
	// Find, Get ... 

	UFUNCTION(BlueprintCallable, Category = "UMGUtil")
		static TArray<class UWidget*> GetChildrenOfPanel(class UPanelWidget* PanelWidget);

	template<typename T>
	static TArray<T*> GetChildrenOfPanelAs(class UPanelWidget* PanelWidget)
	{
		auto Children = GetChildrenOfPanel(PanelWidget);
		TArray<T*> OutArray;
		for (auto Child : Children)
		{
			auto ReturnTypeWiget = Cast<T>(Child);
			if (ReturnTypeWiget)
			{
				OutArray.Add(ReturnTypeWiget);
			}
		}

		return OutArray;
	}


	UFUNCTION(BlueprintCallable, Category = "UMGUtil")
		static class UPanelWidget* FindSamePannelInArray(const TArray<class UPanelWidget*>& PannelArray, class UPanelWidget* PanelWidget);

	UFUNCTION(BlueprintCallable, Category = "UMGUtil")
		static bool IsTheWidgetInThePanel(class UWidget* TargetWidget, const UPanelWidget* PanelWidget);

	UFUNCTION(BlueprintCallable, Category = "UMGUtil")
		static class UWidget* FindNextInHorizontalBox(class ULocalPlayer* Player, class UHorizontalBox* HorizontalBox, bool bInverseDir = false, int32 OffsetForFocus = 0);

	UFUNCTION(BlueprintCallable, Category = "UMGUtil")
		static class UPanelWidget* GetNextPannelInArray(const TArray<class UPanelWidget*>& PannelArray, class UPanelWidget* TargetPannel, bool bInverseDir = false);

	UFUNCTION(BlueprintCallable, Category = "UMGUtil")
		static class UWidget* GetNextWidgetInPanel(class UPanelWidget* Panel, class UWidget* CurrWidget, bool bInverseDir = false);

	UFUNCTION(BlueprintCallable, Category = "UMGUtil")
		static int32 GetFocusWidgetIndexInThePanel(class ULocalPlayer* Player, class UPanelWidget* Panel);

	//////////////////////////////////////////////////////////////////////////
	// ETC



	UFUNCTION(BlueprintCallable, Category = "UMGUtil")
		static void BlockUE4Navigation(class UWidget* Widget);

	UFUNCTION(BlueprintCallable, Category = "UMGUtil")
		static TArray<class UPanelWidget*> CastWidgetArrayToPanelArray(const TArray<class UWidget*>& WidgetArray);

	UFUNCTION(BlueprintCallable, Category = "UMGUtil")
		static FString ToTimeString(float TimeSeconds);

	UFUNCTION(BlueprintCallable, Category = "UMGUtil")
		static FVector2D Rotate2DVector(FVector2D VectorToRotate, float RotateByAngle);

	UFUNCTION(BlueprintCallable, Category = "UMGUtil")
		static FVector2D Clamp2DVector(FVector2D VectorToClamp, FVector2D MinVector, FVector2D MaxVector);

	//////////////////////////////////////////////////////////////////////////
	// Key Input

	UFUNCTION(BlueprintCallable, Category = "UMGUtil")
		static bool IsUp(const FKey& Key);

	UFUNCTION(BlueprintCallable, Category = "UMGUtil")
		static bool IsDown(const FKey& Key);

	UFUNCTION(BlueprintCallable, Category = "UMGUtil")
		static bool IsLeft(const FKey& Key);

	UFUNCTION(BlueprintCallable, Category = "UMGUtil")
		static bool IsRight(const FKey& Key);

	UFUNCTION(BlueprintCallable, Category = "UMGUtil")
		static bool IsL1(const FKey& Key);

	UFUNCTION(BlueprintCallable, Category = "UMGUtil")
		static bool IsR1(const FKey& Key);

	UFUNCTION(BlueprintCallable, Category = "UMGUtil")
		static bool IsEnter(const FKey& Key);

	UFUNCTION(BlueprintCallable, Category = "UMGUtil")
		static bool IsCancel(const FKey& Key);

	/* Returns true if A and B are equal (A == B) */
	UFUNCTION(BlueprintCallable, Category = "UMGUtil")
		static bool TestEqual(class UObject* A, class UObject* B);

	//////////////////////////////////////////////////////////////////////////
	// Brush

	UFUNCTION(BlueprintCallable, Category = "UMGUtil")
		static void SetOnlyBrushImage(class UImage* ImageWidget, class UObject* Texture);

};
