// Fill out your copyright notice in the Description page of Project Settings.

#include "PBGame.h"
#include "Widget.h"
#include "WidgetNavigation.h"
#include "PBUMGUtils.h"
#include "PBUserWidget.h"
#include "PanelWidget.h"
#include "PanelWidget.h"
#include "HorizontalBox.h"
#include "ScrollBox.h"
#include "Reply.h"
#include "Image.h"
#include "WidgetBlueprintLibrary.h"

class UWidget* UPBUMGUtils::NavigateToNextInScrollBox(class ULocalPlayer* Player, class UScrollBox* ScrollBox, bool bInverseDir, int32 OffsetForFocus, int32 OffsetForScroll)
{
	if (Player && ScrollBox)
	{
		int32 Size = ScrollBox->GetChildrenCount();

		for (int32 i = 0; i < Size; i++)
		{
			// find the current focused widget
			auto Child = ScrollBox->GetChildAt(i);
			if (Child)
			{
				if (UPBUserWidget::ContainUserFocus(Player, Child))
				{

					int32 FocusDestIndex = bInverseDir ? (i - (1 + OffsetForFocus)) : (i + (1 + OffsetForFocus));
					int32 ScrollDestIndex = bInverseDir ? (i - (1 + OffsetForScroll)) : (i + (1 + OffsetForScroll));

					if (FocusDestIndex < Size &&  FocusDestIndex >= 0 &&
						ScrollDestIndex < Size &&  ScrollDestIndex >= 0)
					{
						UWidget* FocusDestWidget = ScrollBox->GetChildAt(FocusDestIndex);
						UWidget* ScrollDestWidget = ScrollBox->GetChildAt(ScrollDestIndex);

						if (FocusDestWidget && ScrollDestWidget)
						{
							// scroll to next widget
							ScrollBox->ScrollWidgetIntoView(ScrollDestWidget);
							// navigate to next widget
							UPBUserWidget::NavigateUserFocus(Player, FocusDestWidget);

							return FocusDestWidget;
						}
					}
				}

			}
		}

	}

	return nullptr;
}

class UWidget* UPBUMGUtils::NavigateToNextInScrollBoxes(class ULocalPlayer* Player, const TArray<class UScrollBox*>& ScrollBoxArray, bool bInverseDir /*= false*/, int32 OffsetForFocus /*= 0*/, int32 OffsetForScroll /*= 0*/)
{
	for (auto Box : ScrollBoxArray)
	{
		UWidget* DestWidget = NavigateToNextInScrollBox(Player, Box, bInverseDir, OffsetForFocus, OffsetForScroll);
		if (DestWidget)
		{
			return DestWidget;
		}

	}

	return nullptr;
}

class UWidget* UPBUMGUtils::NavigateToNextInHorizontalBox(class ULocalPlayer* Player, class UHorizontalBox* HorizontalBox, bool bInverseDir /*= false*/, int32 OffsetForFocus /*= 0*/)
{
	if (Player && HorizontalBox)
	{
		int32 Size = HorizontalBox->GetChildrenCount();

		for (int32 i = 0; i < Size; i++)
		{
			// find the current focused widget
			auto Child = HorizontalBox->GetChildAt(i);
			if (Child)
			{
				if (UPBUserWidget::ContainUserFocus(Player, Child))
				{
					int32 FocusDestIndex = bInverseDir ? (i - (1 + OffsetForFocus)) : (i + (1 + OffsetForFocus));

					if (FocusDestIndex < Size &&  FocusDestIndex >= 0)
					{
						UWidget* FocusDestWidget = HorizontalBox->GetChildAt(FocusDestIndex);
						if (FocusDestWidget)
						{
							UPBUserWidget::NavigateUserFocus(Player, FocusDestWidget);
							return FocusDestWidget;
						}

					}
				}
			}
		}
	}

	return nullptr;
}

class UWidget* UPBUMGUtils::NavigateToNextInPanel(class ULocalPlayer* Player, class UPanelWidget* PanelWidget, bool bInverseDir /*= false*/, int32 OffsetForFocus /*= 0*/)
{
	if (Player && PanelWidget)
	{
		int32 Size = PanelWidget->GetChildrenCount();

		for (int32 i = 0; i < Size; i++)
		{
			// find the current focused widget
			auto Child = PanelWidget->GetChildAt(i);
			if (Child)
			{
				if (UPBUserWidget::ContainUserFocus(Player, Child))
				{
					int32 FocusDestIndex = bInverseDir ? (i - (1 + OffsetForFocus)) : (i + (1 + OffsetForFocus));

					if (FocusDestIndex < Size &&  FocusDestIndex >= 0)
					{
						UWidget* FocusDestWidget = PanelWidget->GetChildAt(FocusDestIndex);
						if (FocusDestWidget)
						{
							// You can not navigate to invisible widget.
							if (FocusDestWidget->GetVisibility() != ESlateVisibility::Collapsed && FocusDestWidget->GetVisibility() != ESlateVisibility::Hidden)
							{
								UPBUserWidget::NavigateUserFocus(Player, FocusDestWidget);
								return FocusDestWidget;
							}
					
						}

					}
				}
			}
		}
	}

	return nullptr;
}

class UWidget* UPBUMGUtils::ScrollWidgetIntoViewWithOffset(class UScrollBox* ScrollBox, UWidget* WidgetToFind, bool AnimateScroll /*= true*/, bool bInverseDir, int32 OffsetForScroll /*= 0*/)
{
	if (ScrollBox && WidgetToFind)
	{
		int32 Size = ScrollBox->GetChildrenCount();
		for (int32 i = 0; i < Size; ++i)
		{
			auto Child = ScrollBox->GetChildAt(i);
			if (Child == WidgetToFind)
			{
				int32 ScrollWidgetIndex = bInverseDir ? (i + OffsetForScroll) : (i - OffsetForScroll);
				if (ScrollWidgetIndex >= 0 && ScrollWidgetIndex < Size)
				{
					auto TargetWidget = ScrollBox->GetChildAt(ScrollWidgetIndex);
					ScrollBox->ScrollWidgetIntoView(TargetWidget);

					return TargetWidget;
				}

			}
		}
	}

	return nullptr;
}

class UWidget* UPBUMGUtils::NavigateToOtherScrollBox(class ULocalPlayer* Player, class UScrollBox* FromScrollBox, class UScrollBox* ToScrollBox, float ScrollOffsetForOneStep, int32 OffsetForFocus)
{
	if (FMath::IsNearlyZero(ScrollOffsetForOneStep))
	{
		return nullptr;
	}


	if (Player && FromScrollBox && ToScrollBox)
	{
		int32 Size = FromScrollBox->GetChildrenCount();

		for (int32 i = 0; i < Size; i++)
		{
			// find the current focused widget
			auto Child = FromScrollBox->GetChildAt(i);
			if (Child)
			{
				if (UPBUserWidget::ContainUserFocus(Player, Child))
				{
					// actual index of this widget
					int32 CurrIndex = i;
					float FromScrollOffset = FromScrollBox->GetScrollOffset();

					// Calculate scroll amount of FromScrollBox
					int32 ScrollCnt = (int32)((FromScrollOffset / ScrollOffsetForOneStep) + 0.5f);

					// Index when you see the widget regardless of scrolling.
					int32 IndexInView = CurrIndex - ScrollCnt*(1 + OffsetForFocus);

					// Calculate scroll amount of ToScrollBox
					float ToScrollBoxOffset = ToScrollBox->GetScrollOffset();
					ScrollCnt = (int32)((ToScrollBoxOffset / ScrollOffsetForOneStep) + 0.5f);

					// Final index of ToScrollBox for navigation. 
					int32 ToScrollBoxIndex = IndexInView + ScrollCnt*(1 + OffsetForFocus);

					UWidget* DestWidget = ToScrollBox->GetChildAt(ToScrollBoxIndex);
					if (DestWidget)
					{
						// Navigate focus
						UPBUserWidget::NavigateUserFocus(Player, DestWidget);
						return DestWidget;

						// And don't need scrolling in the process. If need then there's something wrong...
					}

					

				}
			}
		}
	}

	return nullptr;
}

class UWidget* UPBUMGUtils::NavigateToOtherPannelAtSameIndex(class ULocalPlayer* Player, class UPanelWidget* From, class UPanelWidget* To)
{
	if (Player && From && To)
	{
		int32 Size = From->GetChildrenCount();
		for (int32 i = 0; i < Size; ++i)
		{
			auto Child = From->GetChildAt(i);
			if (Child)
			{
				if (UPBUserWidget::ContainUserFocus(Player, Child))
				{
					UWidget* DestWidget = To->GetChildAt(i);
					if (DestWidget)
					{
						// Navigate focus
						UPBUserWidget::NavigateUserFocus(Player, DestWidget);
						return DestWidget;

					}
				}
			}
		}
	}

	return nullptr;
}

class TArray<class UWidget*> UPBUMGUtils::GetChildrenOfPanel(class UPanelWidget* PanelWidget)
{
	TArray<class UWidget*> WidgetArray;

	if (PanelWidget)
	{
		int32 Size = PanelWidget->GetChildrenCount();
		for (int i = 0; i < Size; ++i)
		{
			WidgetArray.Add(PanelWidget->GetChildAt(i));
		}
	}

	return WidgetArray;
}

class UPanelWidget* UPBUMGUtils::FindSamePannelInArray(const TArray<class UPanelWidget*>& PannelArray, class UPanelWidget* PanelWidget)
{
	if (PanelWidget)
	{
		for (UPanelWidget* Pan: PannelArray)
		{
			if (Pan == PanelWidget)
			{
				return Pan;
			}
		}
	}

	return nullptr;
}

bool UPBUMGUtils::IsTheWidgetInThePanel(class UWidget* TargetWidget, const UPanelWidget* PanelWidget)
{
	if (TargetWidget && PanelWidget)
	{
		int32 Size = PanelWidget->GetChildrenCount();
		for (int32 i = 0; i < Size; ++i)
		{
			if (TargetWidget == PanelWidget->GetChildAt(i))
			{
				return true;
			}
		}
	}


	return false;
}

class UWidget* UPBUMGUtils::FindNextInHorizontalBox(class ULocalPlayer* Player, class UHorizontalBox* HorizontalBox, bool bInverseDir, int32 OffsetForFocus)
{
	if (Player && HorizontalBox)
	{
		int32 Size = HorizontalBox->GetChildrenCount();

		for (int32 i = 0; i < Size; i++)
		{
			// find the current focused widget
			auto Child = HorizontalBox->GetChildAt(i);
			if (Child)
			{
				if (UPBUserWidget::ContainUserFocus(Player, Child))
				{
					int32 FocusDestIndex = bInverseDir ? (i - (1 + OffsetForFocus)) : (i + (1 + OffsetForFocus));

					if (FocusDestIndex < Size &&  FocusDestIndex >= 0)
					{
						UWidget* FocusDestWidget = HorizontalBox->GetChildAt(FocusDestIndex);
						if (FocusDestWidget)
						{
							return FocusDestWidget;
						}

					}
				}
			}
		}
	}

	return nullptr;
}

class UPanelWidget* UPBUMGUtils::GetNextPannelInArray(const TArray<class UPanelWidget*>& PannelArray, class UPanelWidget* TargetPannel, bool bInverseDir /*= false*/)
{
	if (TargetPannel)
	{
		for (int32 i = 0 ; i < PannelArray.Num() ; ++i)
		{
			if (PannelArray[i] == TargetPannel)
			{
				int32 IndexOfPannelWillReturn = bInverseDir ? (i - 1) : (i + 1);
				if (IndexOfPannelWillReturn >= 0 && IndexOfPannelWillReturn < PannelArray.Num())
				{
					return PannelArray[IndexOfPannelWillReturn];
				}
			}
		}
	}

	return nullptr;
}


void UPBUMGUtils::BlockUE4Navigation(class UWidget* Widget)
{
	if (Widget && Widget->Navigation)
	{
		Widget->Navigation->Up.Rule = EUINavigationRule::Stop;
		Widget->Navigation->Down.Rule = EUINavigationRule::Stop;
		Widget->Navigation->Left.Rule = EUINavigationRule::Stop;
		Widget->Navigation->Right.Rule = EUINavigationRule::Stop;
		Widget->Navigation->Next.Rule = EUINavigationRule::Stop;
		Widget->Navigation->Previous.Rule = EUINavigationRule::Stop;

	}
}

TArray<class UPanelWidget*> UPBUMGUtils::CastWidgetArrayToPanelArray(const TArray<class UWidget*>& WidgetArray)
{
	TArray<UPanelWidget*> PanelArray;
	for (auto W : WidgetArray)
	{
		UPanelWidget* P = Cast<UPanelWidget>(W);
	}

	return PanelArray;
}

FString UPBUMGUtils::ToTimeString(float TimeSeconds)
{
	// only minutes and seconds are relevant
	const int32 TotalSeconds = FMath::Max(0, FMath::TruncToInt(TimeSeconds) % 3600);
	const int32 NumMinutes = TotalSeconds / 60;
	const int32 NumSeconds = TotalSeconds % 60;

	const FString TimeDesc = FString::Printf(TEXT("%02d:%02d"), NumMinutes, NumSeconds);
	return TimeDesc;
}

FVector2D UPBUMGUtils::Rotate2DVector(FVector2D VectorToRotate, float RotateByAngle)
{
	float c = FMath::Cos(FMath::DegreesToRadians(RotateByAngle));
	float s = FMath::Sin(FMath::DegreesToRadians(RotateByAngle));

	return FVector2D(c*VectorToRotate.X - s*VectorToRotate.Y, s*VectorToRotate.X + c*VectorToRotate.Y);
}

FVector2D UPBUMGUtils::Clamp2DVector(FVector2D VectorToClamp, FVector2D MinVector, FVector2D MaxVector)
{
	return FVector2D(FMath::Clamp(VectorToClamp.X, MinVector.X, MaxVector.X), FMath::Clamp(VectorToClamp.Y, MinVector.Y, MaxVector.Y));
}

bool UPBUMGUtils::IsUp(const FKey& Key)
{
	return (Key == EKeys::Up) || (Key == EKeys::Gamepad_DPad_Up) || (Key == EKeys::Gamepad_LeftStick_Up);
}

bool UPBUMGUtils::IsDown(const FKey& Key)
{
	return (Key == EKeys::Down) || (Key == EKeys::Gamepad_DPad_Down) || (Key == EKeys::Gamepad_LeftStick_Down);
}

bool UPBUMGUtils::IsLeft(const FKey& Key)
{
	return (Key == EKeys::Left) || (Key == EKeys::Gamepad_DPad_Left) || (Key == EKeys::Gamepad_LeftStick_Left);

}

bool UPBUMGUtils::IsRight(const FKey& Key)
{
	return (Key == EKeys::Right) || (Key == EKeys::Gamepad_DPad_Right) || (Key == EKeys::Gamepad_LeftStick_Right);

}

bool UPBUMGUtils::IsL1(const FKey& Key)
{
	return (Key == EKeys::Gamepad_LeftShoulder);
}

bool UPBUMGUtils::IsR1(const FKey& Key)
{
	return (Key == EKeys::Gamepad_RightShoulder);

}

bool UPBUMGUtils::IsEnter(const FKey& Key)
{
	return (Key == EKeys::Enter) || (Key == EKeys::Gamepad_FaceButton_Bottom);
}

bool UPBUMGUtils::IsCancel(const FKey& Key)
{
	return (Key == EKeys::Escape) || (Key == EKeys::Gamepad_FaceButton_Right);

}

bool UPBUMGUtils::TestEqual(class UObject* A, class UObject* B)
{
	if (A && B)
	{
		return A == B;
	}



	return false;
}

void UPBUMGUtils::SetOnlyBrushImage(class UImage* ImageWidget, class UObject* Texture)
{
	if (ImageWidget && Texture)
	{
		FSlateBrush InBrush = ImageWidget->Brush;
		InBrush.SetResourceObject(Texture);
		ImageWidget->SetBrush(InBrush);
	}
}

class UWidget* UPBUMGUtils::GetNextWidgetInPanel(class UPanelWidget* Panel, class UWidget* CurrWidget, bool bInverseDir)
{
	if (Panel && CurrWidget)
	{
		int32 Size = Panel->GetChildrenCount();
		for (int i = 0; i < Size; ++i)
		{
			auto Child = Panel->GetChildAt(i);
			if (Child)
			{
				if (Child == CurrWidget)
				{
					int32 TargetIndex = bInverseDir ? i - 1 : i + 1;

					if (TargetIndex >= 0 && TargetIndex < Size)
					{
						return Panel->GetChildAt(TargetIndex);
					}
				}
			}
		}
	}

	return nullptr;
}

int32 UPBUMGUtils::GetFocusWidgetIndexInThePanel(class ULocalPlayer* Player, class UPanelWidget* Panel)
{
	if (Panel)
	{
		int32 Size = Panel->GetChildrenCount();
		for (int32 i = 0; i < Size; ++i)
		{
			if (UPBUserWidget::ContainUserFocus(Player, Panel->GetChildAt(i)))
			{
				return i;
			}
		}
	}

	return -1;
}
