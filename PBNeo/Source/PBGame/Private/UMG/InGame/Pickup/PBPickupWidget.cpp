// Fill out your copyright notice in the Description page of Project Settings.

#include "PBGame.h"
#include "PBPickupWidget.h"
#include "PBPickup.h"
#include "PBPickupIconWidget.h"
#include "CanvasPanelSlot.h"
#include "Kismet/KismetMathLibrary.h"

PBGetterWidgetBlueprintVariable_Implementation(UCanvasPanel, UPBPickupWidget, MainCanvas);



void UPBPickupWidget::NativeConstruct()
{
	Super::NativeConstruct();

	BindWidgetEvent(OnEnterOuterPickup, UPBPickupWidget::NotifyEnterOuterPickup);
	BindWidgetEvent(OnLeaveOuterPickup, UPBPickupWidget::NotifyLeaveOuterPickup);

	BindWidgetEvent(OnReadyPickup, UPBPickupWidget::NotifyReadyPickup);
	BindWidgetEvent(OnStartPickup, UPBPickupWidget::NotifyStartPickup);
	BindWidgetEvent(OnCancelPickup, UPBPickupWidget::NotifyCancelPickup);

	BindWidgetEvent(OnFinishPickup, UPBPickupWidget::NotifyFinishPickup);

	BindWidgetEvent(OnDeath, UPBPickupWidget::NotifyDeath);
	BindWidgetEvent(OnSpawn, UPBPickupWidget::NotifySpawn);

}

void UPBPickupWidget::NativeDestruct()
{
	UnbindWidgetEvent(OnEnterOuterPickup);
	UnbindWidgetEvent(OnLeaveOuterPickup);

	UnbindWidgetEvent(OnReadyPickup);
	UnbindWidgetEvent(OnStartPickup);
	UnbindWidgetEvent(OnCancelPickup);

	UnbindWidgetEvent(OnFinishPickup);

	UnbindWidgetEvent(OnDeath);
	UnbindWidgetEvent(OnSpawn);

	Reset();


	Super::NativeDestruct();

}

void UPBPickupWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	UpdatePickupIconsLocation();

	UpdateIconsVisibilityAccordingToRelativeLocation();
}

void UPBPickupWidget::Reset()
{
	for (auto Pair : PickupIcons)
	{
		APBPickup* Pickup = Pair.Key.Get();
		RemovePickupIcon(Pickup);
	}

	PickupIcons.Empty();
}

void UPBPickupWidget::NotifyEnterOuterPickup(class APBPickup* Pickup)
{
	BuildPickupIcon(Pickup);
}

void UPBPickupWidget::NotifyLeaveOuterPickup(class APBPickup* Pickup)
{
	RemovePickupIcon(Pickup);

}

void UPBPickupWidget::NotifyReadyPickup(class APBPickup* Pickup)
{
	// Set the pickup icon's state to ready
	if (Pickup)
	{
		for (auto Value : PickupIcons)
		{
			UPBPickupIconWidget* IconWidget = Value.Value;

			if (nullptr == IconWidget)
			{
				continue;
			}

			if (Value.Key == Pickup)
			{
				IconWidget->ShowPickupReadyUI(true);
			}
			else
			{
				IconWidget->ShowPickupReadyUI(false);
			}
		}
	}
	else
	{
		for (auto Value : PickupIcons)
		{
			auto IconWidget = Value.Value;
			if (IconWidget)
			{
				IconWidget->ShowPickupReadyUI(false);
			}
		}
	}

}

void UPBPickupWidget::NotifyStartPickup(class APBPickup* Pickup, float TimerDuration)
{
	auto IconWidget = GetIconWidgetFromContainer(Pickup);
	if (IconWidget)
	{
		IconWidget->StartGaugeFilling(TimerDuration);
	}
}

void UPBPickupWidget::NotifyCancelPickup(class APBPickup* Pickup)
{
	auto IconWidget = GetIconWidgetFromContainer(Pickup);
	if (IconWidget)
	{
		IconWidget->StopGaugeFilling();
	}
}


void UPBPickupWidget::NotifyFinishPickup(class APBPickup* Pickup)
{
	RemovePickupIcon(Pickup);
}

void UPBPickupWidget::NotifyDeath(class APBPlayerState* KillerPlayerState, class APBPlayerState* KilledPlayerState, const UDamageType* KillerDamageType, const FTakeDamageInfo& TakeDamageInfo)
{
	if (GetPBPlayerState() == KilledPlayerState)
	{
		Reset();

		Hide();
	}



}

void UPBPickupWidget::NotifySpawn(class APBPlayerState* SpawnedPlayerState)
{
	if (GetPBPlayerState() == SpawnedPlayerState)
	{
		Reset();

		Show();
	}
}

void UPBPickupWidget::BuildPickupIcon(class APBPickup* Pickup)
{
	if (nullptr == Pickup)
	{
		return;
	}

	// If the pickup is already added, ignore it
	if (PickupIcons.Contains(Pickup))
	{
		return;
	}

	auto PickupIconWidget = CreateWidget<UPBPickupIconWidget>(GetOwningPlayer(), PickupIconWidgetClass);
	if (PickupIconWidget)
	{
		PickupIconWidget->InitWidget(Pickup);

		// Attach the widget and set size as auto.
		GetBPV_MainCanvas()->AddChild(PickupIconWidget);
		auto TargetSlot = Cast<UCanvasPanelSlot>(PickupIconWidget->Slot);
		if (TargetSlot)
		{
			TargetSlot->SetAutoSize(true);
		}

		PickupIcons.Add(Pickup, PickupIconWidget);

	}


}

void UPBPickupWidget::RemovePickupIcon(class APBPickup* Pickup)
{
	// Remove pickup from the array, from the widget
	if (PickupIcons.Contains(Pickup))
	{
		PickupIcons[Pickup]->RemoveFromParent();
		PickupIcons.Remove(Pickup);

	}
}

void UPBPickupWidget::UpdatePickupIconsLocation()
{
	if (PickupIcons.Num() < 1)
	{
		return;
	}

	if (false == ensure(GetOwningCharacter()))
	{
		return;
	}

	for (auto Pair : PickupIcons)
	{
		auto Pickup = Pair.Key;
		auto Icon = Pair.Value;

		if (Pickup.IsValid() && Icon)
		{
			FVector WorldPickupLoc = Pickup->GetActorLocation();
			FVector WorldCharLoc = GetOwningCharacter()->GetActorLocation();

			FVector2D ScreenPickupLoc = FVector2D::ZeroVector;

			auto LookAtRotator = UKismetMathLibrary::FindLookAtRotation(WorldPickupLoc, WorldCharLoc);
			auto RotatedUpVector = LookAtRotator.RotateVector(Pickup->GetActorUpVector());

			float DistBetweenChar = FVector::Dist(WorldPickupLoc, GetOwningCharacter()->GetActorLocation());
			float DistanceScaledYOffset = ScreenYOffset * (FMath::Min(1.0f, DistBetweenChar / Pickup->OuterVolumeSize));

			// Add YOffset according to the distance.
			WorldPickupLoc += RotatedUpVector * DistanceScaledYOffset;

			bool Res = UGameplayStatics::ProjectWorldToScreen(GetOwningPlayer(), WorldPickupLoc, ScreenPickupLoc);
			if (Res)
			{
				auto TargetSlot = Cast<UCanvasPanelSlot>(Icon->Slot);

				FVector2D FinalLoc = FromViewportPixelToLocal(ScreenPickupLoc);
				TargetSlot->SetPosition(FinalLoc);
			}
		}

	}
}

void UPBPickupWidget::UpdateIconsVisibilityAccordingToRelativeLocation()
{

	for (auto Pair : PickupIcons)
	{
		auto Pickup = Pair.Key;
		auto Icon = Pair.Value;

		if (Pickup.IsValid() && Icon)
		{
			FVector WorldPickupLoc = Pickup->GetActorLocation();
			FVector WorldCharLoc = GetOwningCharacter()->GetActorLocation();

			FVector ToPickupDir = (WorldPickupLoc - WorldCharLoc).GetSafeNormal();
			FVector CharLook = GetOwningCharacter()->GetActorForwardVector().GetSafeNormal();

			bool IsThePickupInFront = (FVector::DotProduct(ToPickupDir, CharLook) > 0);
			if (IsThePickupInFront)
			{
				Icon->Show();
			}
			else
			{
				Icon->Hide();
			}
		}
	}

}

UPBPickupIconWidget* UPBPickupWidget::GetIconWidgetFromContainer(class APBPickup* Pickup)
{
	if (PickupIcons.Contains(Pickup))
	{
		return PickupIcons[Pickup];
	}

	return nullptr;
}
