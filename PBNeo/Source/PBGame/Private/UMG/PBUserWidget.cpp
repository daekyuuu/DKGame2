// Fill out your copyright notice in the Description page of Project Settings.

#include "PBGame.h"
#include "PBUserWidget.h"
#include "PBMainWidget.h"
#include "PB3DWidgetRelay.h"
#include "Engine/UserInterfaceSettings.h"

#include "PBItemTable_Weap.h"
#include "PanelWidget.h"
#include "UserWidget.h"
#include "WidgetTree.h"
#include "Button.h"

void UPBUserWidget::DesignTimeConstruct_Implementation()
{

}

void UPBUserWidget::OnWidgetRebuilt()
{
	Super::OnWidgetRebuilt();
	if (IsDesignTime())
	{
		DesignTimeConstruct();
	}
}

void UPBUserWidget::SyncProperties_Implementation()
{

}

void UPBUserWidget::SynchronizeProperties()
{
	Super::SynchronizeProperties();
	SyncProperties();
}

UPBUserWidget::UPBUserWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	FocusTransferTarget = nullptr;
	InitFocusMoving();
	InitInputLocking();

	bEnableAutoDestroy = true;
}

void UPBUserWidget::RemoveFromParent()
{
	/*
		bEnabledAutoDestroy가 true이면 이 함수(UUserWidget::RemoveFromParent)는 호출되어도 기능을 수행하지 않습니다.
		그러므로 사용자가 원하는 시점에서 같은 효과를 원한다면 UUserWidget::RemoveFromViewport함수를 호출하면 됩니다.
		void UUserWidget::RemoveFromViewport()
		{
			RemoveFromParent();
		}
	*/
	if (bEnableAutoDestroy)
	{
		Super::RemoveFromParent();
	}
}

void UPBUserWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	UpdateGeometryCache(MyGeometry);
}

void UPBUserWidget::UpdateGeometryCache(const FGeometry& MyGeometry)
{
	GeometryCache = MyGeometry;
}

FVector2D UPBUserWidget::FromLocalToViewportLayout(FVector2D Input)
{
	FVector2D Absolute = GeometryCache.LocalToAbsolute(Input);
	if (GetWorld() && GetWorld()->IsGameWorld())
	{
		if (UGameViewportClient* ViewportClient = GetWorld()->GetGameViewport())
		{
			if (FViewport* Viewport = ViewportClient->Viewport)
			{
				FVector2D ViewportSize;
				ViewportClient->GetViewportSize(ViewportSize);
				FVector2D PixelPosition = Viewport->VirtualDesktopPixelToViewport(FIntPoint((int32)Absolute.X, (int32)Absolute.Y)) * ViewportSize;

				float CurrentViewportScale = GetDefault<UUserInterfaceSettings>()->GetDPIScaleBasedOnSize(FIntPoint(ViewportSize.X, ViewportSize.Y));
				FVector2D LayoutPosition = PixelPosition / CurrentViewportScale;

				return LayoutPosition;
			}
		}
	}
	return FVector2D();
}

FVector2D UPBUserWidget::FromLocalToViewportPixel(FVector2D Input)
{
	FVector2D Absolute = GeometryCache.LocalToAbsolute(Input);
	if (GetWorld() && GetWorld()->IsGameWorld())
	{
		if (UGameViewportClient* ViewportClient = GetWorld()->GetGameViewport())
		{
			if (FViewport* Viewport = ViewportClient->Viewport)
			{
				FVector2D ViewportSize;
				ViewportClient->GetViewportSize(ViewportSize);

				FVector2D PixelPosition = Viewport->VirtualDesktopPixelToViewport(FIntPoint((int32)Absolute.X, (int32)Absolute.Y)) * ViewportSize;
				
				return PixelPosition;
			}
		}
	}
	return FVector2D();
}

FVector2D UPBUserWidget::FromViewportPixelToLocal(FVector2D Input)
{
	if (GetWorld() && GetWorld()->IsGameWorld())
	{
		if (UGameViewportClient* ViewportClient = GetWorld()->GetGameViewport())
		{
			if (FViewport* Viewport = ViewportClient->Viewport)
			{
				FVector2D ViewportSize;
				ViewportClient->GetViewportSize(ViewportSize);

				FIntPoint Absolute = Viewport->ViewportToVirtualDesktopPixel(Input / ViewportSize);

				return GeometryCache.AbsoluteToLocal(Absolute);
			}
		}
	}
	return FVector2D();
}

FVector2D UPBUserWidget::GetCachedLocalSize()
{
	return GeometryCache.GetLocalSize();
}

FReply UPBUserWidget::NativeOnKeyUp(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	if (bInputLocked)
	{
		return FReply::Handled();
	}
	return Super::NativeOnKeyUp(InGeometry, InKeyEvent);
}

FReply UPBUserWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (bInputLocked)
	{
		return FReply::Handled();
	}
	return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
}

FReply UPBUserWidget::NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (bInputLocked)
	{
		return FReply::Handled();
	}
	return Super::NativeOnMouseButtonUp(InGeometry, InMouseEvent);
}


FReply UPBUserWidget::NativeOnMouseButtonDoubleClick(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (bInputLocked)
	{
		return FReply::Handled();
	}
	return Super::NativeOnMouseButtonDoubleClick(InGeometry, InMouseEvent);
}

void UPBUserWidget::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (bInputLocked)
	{
		return;
	}
	Super::NativeOnMouseEnter(InGeometry, InMouseEvent);
}

void UPBUserWidget::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	if (bInputLocked)
	{
		return;
	}
	Super::NativeOnMouseLeave(InMouseEvent);
}

void UPBUserWidget::SetLockInputForAllChildren(UWidget* Widget, bool ToLock)
{
	UPBUserWidget* PBUserWidget = Cast<UPBUserWidget>(Widget);
	if (PBUserWidget)
	{
		PBUserWidget->bInputLocked = ToLock;
	}

	UUserWidget* UserWidget = Cast<UUserWidget>(Widget);
	if (UserWidget && UserWidget->WidgetTree)
	{
		SetLockInputForAllChildren(UserWidget->WidgetTree->RootWidget, ToLock);
		return;
	}

	UPanelWidget* PBPanelWidget = Cast<UPanelWidget>(Widget);
	if (PBPanelWidget)
	{
		for (int idx = 0; idx < PBPanelWidget->GetChildrenCount(); ++idx)
		{
			SetLockInputForAllChildren(PBPanelWidget->GetChildAt(idx), ToLock);
		}
		return;
	}
}

bool UPBUserWidget::ButtonSubWidgetShouldBeEnabled() const
{
	return !bInputLocked && bIsEnabled;
}

FEventReply UPBUserWidget::TransferUserFocusThroughReply(
	const FFocusEvent& AcceptedFocusEvent, UPARAM(ref) FEventReply& Reply, UWidget* FocusWidget, bool bInAllUsers /*= false*/)
{
	if (FocusWidget)

	{
		TSharedPtr<SWidget> CapturingSlateWidget = FocusWidget->GetCachedWidget();
		if (CapturingSlateWidget.IsValid())
		{
			Reply.NativeReply = Reply.NativeReply.SetUserFocus(CapturingSlateWidget.ToSharedRef(), AcceptedFocusEvent.GetCause(), bInAllUsers);
		}
	}

	return Reply;
}

FReply UPBUserWidget::NativeOnFocusReceived(const FGeometry& InGeometry, const FFocusEvent& InFocusEvent)
{
	if (FocusTransferTarget)
	{
		// for debugging
		//PrintOnScreen(GetName(), 5.0f, FColor::White);

		// handled
		FEventReply E(true);

		// reply focus to target widget.
		return TransferUserFocusThroughReply(InFocusEvent, E, FocusTransferTarget).NativeReply;
	}
	else
	{
		return Super::NativeOnFocusReceived(InGeometry, InFocusEvent);
	}
}

void UPBUserWidget::NavigateUserFocus(ULocalPlayer* Player, UWidget* Widget)
{
	if (Widget)
	{
		return NavigateUserFocus(Player, Widget->TakeWidget());
	}
}

void UPBUserWidget::ClearUserFocus(ULocalPlayer* Player)
{
	if (Player)
	{
		FSlateApplication& SlateApp = FSlateApplication::Get();
		SlateApp.ClearUserFocus(Player->GetControllerId(), EFocusCause::SetDirectly);
	}
}

void UPBUserWidget::NavigateUserFocus(ULocalPlayer* Player, TSharedPtr<SWidget> Widget)
{
	if (Player && Widget.IsValid())
	{
		FSlateApplication& SlateApp = FSlateApplication::Get();
		SlateApp.SetUserFocus(Player->GetControllerId(), Widget, EFocusCause::SetDirectly);

	}
}

void UPBUserWidget::NavigateKeyboardFocus(UWidget* Widget)
{
	if (Widget)
	{
		return NavigateKeyboardFocus(Widget->TakeWidget());
	}
}

void UPBUserWidget::ClearKeyboardFocus()
{
	FSlateApplication& SlateApp = FSlateApplication::Get();
	SlateApp.ClearKeyboardFocus(EFocusCause::SetDirectly);
}

void UPBUserWidget::NavigateKeyboardFocus(TSharedPtr<SWidget> Widget)
{
	if (Widget.IsValid())
	{
		FSlateApplication& SlateApp = FSlateApplication::Get();
		SlateApp.SetKeyboardFocus(Widget, EFocusCause::SetDirectly);
	}
}

bool UPBUserWidget::ContainOwnerUserFocus()
{
	return ContainUserFocus(GetOwningLocalPlayer(), this);
}

bool UPBUserWidget::ContainUserFocus(ULocalPlayer* Player, UWidget* Widget)
{
	if (Player && Widget)
	{	
		if (ContainUserFocus(Player->GetControllerId(), Widget->TakeWidget().Get()))
		{
			return true;
		}

		for (TObjectIterator<UPB3DWidgetRelay> Itr; Itr; ++Itr)
		{
			UPB3DWidgetRelay* Relay = *Itr;
			if (Relay && Relay->GetWorld() && Relay->GetWorld()->IsGameWorld())
			{
				if (Relay->GetOwningLocalPlayer() == Player
					&&
					Relay->ContainVirtualUserFocus(Widget->TakeWidget().Get()))
				{
					return true;
				}
			}
		}
	}

	return false;
}

bool UPBUserWidget::ContainUserFocus(int ControllerIndex, SWidget& Widget)
{
	if (Widget.HasUserFocus(ControllerIndex))
	{
		return true;
	}

	if (Widget.GetChildren() == NULL)
	{
		return false;
	}

	for (int i = 0; i < Widget.GetChildren()->Num(); ++i)
	{
		if (ContainUserFocus(ControllerIndex, Widget.GetChildren()->GetChildAt(i).Get()))
		{
			return true;
		}
	}

	return false;
}

bool UPBUserWidget::ConTainKeyboardFocus(UWidget* Widget)
{
	if (Widget)
	{
		return ContainKeyboardFocus(Widget->TakeWidget().Get());
	}
	else
	{
		return false;
	}
}

bool UPBUserWidget::ContainKeyboardFocus(SWidget& Widget)
{
	if (Widget.HasKeyboardFocus())
	{
		return true;
	}

	if (Widget.GetChildren() == NULL)
	{
		return false;
	}

	for (int i = 0; i < Widget.GetChildren()->Num(); ++i)
	{
		if (ContainKeyboardFocus(Widget.GetChildren()->GetChildAt(i).Get()))
		{
			return true;
		}
	}

	return false;
}

FReply UPBUserWidget::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	if (bInputLocked)
		return FReply::Handled();

	FReply SuperReply = Super::NativeOnKeyDown(InGeometry, InKeyEvent);

	FSlateApplication& SlateApp = FSlateApplication::Get();
	EUINavigation NavDirection = SlateApp.GetNavigationDirectionFromKey(InKeyEvent);
	if (NavDirection == EUINavigation::Up && UpWidget.IsValid())
	{
		return FReply::Handled().SetUserFocus(UpWidget->TakeWidget(), EFocusCause::SetDirectly, false);
	}
	else if (NavDirection == EUINavigation::Down && DownWidget.IsValid())
	{
		return FReply::Handled().SetUserFocus(DownWidget->TakeWidget(), EFocusCause::SetDirectly, false);
	}
	else if (NavDirection == EUINavigation::Left && LeftWidget.IsValid())
	{
		return FReply::Handled().SetUserFocus(LeftWidget->TakeWidget(), EFocusCause::SetDirectly, false);
	}
	else if (NavDirection == EUINavigation::Right && RightWidget.IsValid())
	{
		return FReply::Handled().SetUserFocus(RightWidget->TakeWidget(), EFocusCause::SetDirectly, false);
	}

	return SuperReply;
}

void UPBUserWidget::SetWidgetNeighboursOverride(UWidget* Up, UWidget* Down, UWidget* Left, UWidget* Right)
{
	UpWidget = Up;
	DownWidget = Down;
	LeftWidget = Left;
	RightWidget = Right;
}

void UPBUserWidget::InitFocusMoving()
{
	UpWidget = nullptr;
	DownWidget = nullptr;
	LeftWidget = nullptr;
	RightWidget = nullptr;
}

void UPBUserWidget::InitInputLocking()
{
	bInputLocked = false;
}

TSharedPtr<FPBWidgetEventDispatcher> UPBUserWidget::GetMessageDispatcher() const
{
	APBPlayerController* pc = GetOwningPlayer() ? Cast<APBPlayerController>(GetOwningPlayer()) : nullptr;
	if (IsSafe(pc))
	{
		if (pc->WidgetEventDispatcher.IsValid())
		{
			return pc->WidgetEventDispatcher;
		}
	}

	return nullptr;
}

void UPBUserWidget::SetOwningPlayerController(APlayerController* Owner)
{
	if (Owner)
	{
		SetOwningLocalPlayer(Owner->GetLocalPlayer());
	}
}

UPBGameInstance* UPBUserWidget::GetPBGameInstance() const
{
	return GetWorld() ? Cast<UPBGameInstance>(GetWorld()->GetGameInstance()) : nullptr;
}

APBGameState* UPBUserWidget::GetPBGameState() const
{
	return GetWorld() ? Cast<APBGameState>(GetWorld()->GetGameState()) : nullptr;
}

APBPlayerState* UPBUserWidget::GetPBPlayerState() const
{
	return GetOwningPlayer() ? Cast<APBPlayerState>(GetOwningPlayer()->PlayerState) : nullptr;
}

class APBCharacter* UPBUserWidget::GetOwningCharacter() const
{
	return Cast<APBCharacter>(GetOwningPlayerPawn());
}

class APBPlayerController* UPBUserWidget::GetOwningPBPlayer() const
{
	return Cast<APBPlayerController>(GetOwningPlayer());
}

void UPBUserWidget::SetEnableAutoDestroy(bool bInEnableAutoDestroy)
{
	bEnableAutoDestroy = bInEnableAutoDestroy;
}

bool UPBUserWidget::IsEnabledAutoDestroy() const
{
	return bEnableAutoDestroy;
}


UPBTimerWidget::UPBTimerWidget(const FObjectInitializer &Initializer)
	: Super(Initializer)
{
	TimeStamp = FDateTime::UtcNow().ToUnixTimestamp();
	bTrigManually = true;
	LifeTimeInSeconds = 5.0f;
}

void UPBTimerWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (false == bTrigManually)
	{
		StartTimer();
	}
}

void UPBTimerWidget::EndTimer()
{
	OnEndTimer.Broadcast(this);
}

void UPBTimerWidget::StartTimer()
{
	auto GI = GetPBGameInstance();
	if (false == ensure(GI))
		return;

	auto &TM = GI->GetTimerManager();
	if (TM.IsTimerActive(DisposableTimerHandle))
		return;

	TM.SetTimer(DisposableTimerHandle, this, &UPBTimerWidget::EndTimer, LifeTimeInSeconds, false);
}

int64 UPBTimerWidget::GetTimeStamp() const
{
	return TimeStamp;
}

void UPBTimerWidget::SetTimeStamp(int64 InTimeStamp)
{
	TimeStamp = InTimeStamp;
}