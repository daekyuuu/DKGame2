// Fill out your copyright notice in the Description page of Project Settings.

#include "PBGame.h"
#include "PBBombSiteIndicatorWidget.h"
#include "PBTrigger_BombSite.h"
#include "PBCharacter.h"
#include "PBBombSiteIndicatorImageWidget.h"
#include "CanvasPanelSlot.h"
#include "WidgetLayoutLibrary.h"
#include "Components/CanvasPanel.h"

PBGetterWidgetBlueprintVariable_Implementation(class UCanvasPanel, UPBBombSiteIndicatorWidget, Canvas);

void UPBBombSiteIndicatorWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// getting cache
	GetBPV_Canvas();

	InitIndicators();

	AngleAsDegree = 0.0f;

	OffsetContractScreenBorder = 0.1f;
	//IsAlive = GetOwningCharacter() != nullptr ? GetOwningCharacter()->IsAlive() : false;

	//BindWidgetEvent(OnDeath, UPBBombSiteIndicatorWidget::NotifyDeath);
	BindWidgetEvent(OnSpawn, UPBBombSiteIndicatorWidget::NotifySpawn);
	BindWidgetEvent(OnZoom, UPBBombSiteIndicatorWidget::NotifyZoom);
	BindWidgetEvent(OnBombPlanted, UPBBombSiteIndicatorWidget::NotifyBombPlanted);
	BindWidgetEvent(OnBombExploded, UPBBombSiteIndicatorWidget::NotifyBombExploded);
	BindWidgetEvent(OnBombDefused, UPBBombSiteIndicatorWidget::NotifyBombDefused);
}

void UPBBombSiteIndicatorWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	TickIndicators(InDeltaTime);
}

void UPBBombSiteIndicatorWidget::NativeDestruct()
{
	//UnbindWidgetEvent(OnDeath);
	UnbindWidgetEvent(OnSpawn);
	UnbindWidgetEvent(OnZoom);
	UnbindWidgetEvent(OnBombPlanted);
	UnbindWidgetEvent(OnBombExploded);
	UnbindWidgetEvent(OnBombDefused);

	Super::NativeDestruct();
}

TArray<FPBBombSiteIndicatorData> UPBBombSiteIndicatorWidget::GetBombSiteData()
{
	return BombSiteData;
}

void UPBBombSiteIndicatorWidget::UpdateBombSiteData()
{
	if (nullptr == GetWorld())
		return;

	RemoveBombSiteData();

	//if (IsAlive)
	{
		AddBombSiteData();

		UpdateAlertModeIndicatorImageWidget();
	}
}

void UPBBombSiteIndicatorWidget::AddBombSiteData()
{
	if (nullptr == GetWorld())
		return;

	for (TActorIterator<APBTrigger_BombSite> Itr(GetWorld()); Itr; ++Itr)
	{
		APBTrigger_BombSite *CurTrigger = *Itr;
		if (!CurTrigger->IsPendingKill())
		{
			FPBBombSiteIndicatorData Data;
			Data.TriggerBombSite = CurTrigger;
			Data.IndicatorImageWidget = CreateWidget<UPBBombSiteIndicatorImageWidget>(GetOwningPlayer(), Data.TriggerBombSite->GetIndicatorImageWidgetClass());
			Data.IndicatorImageWidget->SetVisibility(ESlateVisibility::Visible);
			BombSiteData.Add(Data);

			//InitImageWidget(Data.TriggerBombSite->GetSiteSign(), Data.IndicatorImageWidget);
			{
				GetBPV_Canvas()->AddChildToCanvas(Data.IndicatorImageWidget);
				FString PointName = EPBBombSiteSignAsString(Data.TriggerBombSite->GetSiteSign());
				Data.IndicatorImageWidget->GetBPV_PointName()->SetText(FText::FromString(PointName));
			}
		}
	}
}

void UPBBombSiteIndicatorWidget::RemoveBombSiteData()
{
	if (nullptr == GetWorld())
		return;

	for (FPBBombSiteIndicatorData &CurData : BombSiteData)
	{
		FPBBombSiteIndicatorData &Data = CurData;
		Data.IndicatorImageWidget->SetVisibility(ESlateVisibility::Hidden);
		Data.IndicatorImageWidget->RemoveFromParent();
		Data.IndicatorImageWidget = nullptr;
	}

	BombSiteData.Reset();
}

bool UPBBombSiteIndicatorWidget::OwningPlayerIsDied(APlayerState* VictimPlayer)
{
	return GetPBPlayerState() == VictimPlayer;
}

APBCharacter* UPBBombSiteIndicatorWidget::GetOwningCharacter()
{
	auto PC = Cast<APBPlayerController>(GetOwningPlayer());
	if (PC)
	{
		return Cast<APBCharacter>(PC->GetCharacter());
	}
	return nullptr;
}

void UPBBombSiteIndicatorWidget::NotifySpawn(class APBPlayerState* SpawnedPlayerState)
{
	if (nullptr == GetWorld())
		return;

	if (nullptr == SpawnedPlayerState)
		return;

	if (nullptr == GetPBPlayerState())
		return;

	if (GetPBPlayerState() != SpawnedPlayerState)
		return;
	
	//IsAlive = true;

	UpdateBombSiteData();
}

void UPBBombSiteIndicatorWidget::NotifyZoom(bool NowZooming)
{
	IsZooming = NowZooming;
}

//void UPBBombSiteIndicatorWidget::NotifyDeath(class APBPlayerState* KillerPlayerState, class APBPlayerState* VictimPlayerState, const UDamageType* KillerDamageType, const FTakeDamageInfo& TakeDamageInfo)
//{
//	if (OwningPlayerIsDied(VictimPlayerState) == false)
//		return;
//
//	//IsAlive = false;
//
//	RemoveBombSiteData();
//	StopAnimationOfIndicatorWidgets();
//}

void UPBBombSiteIndicatorWidget::NotifyBombPlanted()
{
	if (nullptr == GetWorld())
		return;

	if (GetOwningPlayer() == nullptr || GetOwningCharacter() == nullptr)
		return;

	UpdateAlertModeIndicatorImageWidget();
}

void UPBBombSiteIndicatorWidget::NotifyBombExploded()
{
	if (nullptr == GetWorld())
		return;

	if (GetOwningPlayer() == nullptr || GetOwningCharacter() == nullptr)
		return;

	StopAnimationOfIndicatorWidgets();
}

void UPBBombSiteIndicatorWidget::NotifyBombDefused()
{
	if (nullptr == GetWorld())
		return;

	if (GetOwningPlayer() == nullptr || GetOwningCharacter() == nullptr)
		return;

	StopAnimationOfIndicatorWidgets();
}

void UPBBombSiteIndicatorWidget::UpdateAlertModeIndicatorImageWidget()
{
	if (nullptr == GetWorld())
		return;

	for (auto CurData : BombSiteData)
	{
		APBTrigger_BombSite *CurBombSite = CurData.TriggerBombSite;
		if (nullptr == CurBombSite)
			continue;

		UPBBombSiteIndicatorImageWidget *IndicatorImageWidget = CurData.IndicatorImageWidget;
		if (nullptr == IndicatorImageWidget)
			continue;

		if (CurBombSite && CurBombSite->IsPlantedBomb())
		{
			//BP_AlertModeIndicatorImageWidget(BombSite, IndicatorImageWidget);
			// Alert Planted Bomb
			if (false == IndicatorImageWidget->IsPlayingAnimation())
			{
				if (IndicatorImageWidget->GetBPP_TargetOn())
				{
					IndicatorImageWidget->PlayAnimation(IndicatorImageWidget->GetBPP_TargetOn(), 0, 0 /* Loop Inifinitly */ );
				}
			}

			return;
		}
	}
}

void UPBBombSiteIndicatorWidget::StopAnimationOfIndicatorWidgets()
{
	if (nullptr == GetWorld())
		return;

	for (auto CurData : BombSiteData)
	{
		APBTrigger_BombSite *CurBombSite = CurData.TriggerBombSite;
		if (nullptr == CurBombSite)
			continue;

		UPBBombSiteIndicatorImageWidget *IndicatorImageWidget = CurData.IndicatorImageWidget;
		if (nullptr == IndicatorImageWidget)
			continue;

		//BP_StopAnimationOfIndicatorWidgets(IndicatorImageWidget);
		// Stop Alert Animation
		{
			if (IndicatorImageWidget->GetBPP_TargetOn())
			{
				IndicatorImageWidget->StopAnimation(IndicatorImageWidget->GetBPP_TargetOn());
			}
		}
	}
}

void UPBBombSiteIndicatorWidget::InitIndicators()
{
	AddBombSiteData();
}

// Calculate on plane of X,Y-axis in 3D Space.
float GetAngleForWidgetRenderer(
	const FVector &CrossHairWorldLocation
	, const FVector &OwningCharacterForward
	, const FVector &BombSiteLocation
	, bool L, bool T, bool R, bool B)
{
	FVector ToBombSiteLocation = BombSiteLocation - CrossHairWorldLocation;
	ToBombSiteLocation.Normalize();

	float ResultAngle = ToBombSiteLocation | OwningCharacterForward;
	FVector CrossToBombSiteWithForward = FVector::CrossProduct(ToBombSiteLocation, OwningCharacterForward);
	float DotProductWithUp = FVector::UpVector | CrossToBombSiteWithForward;

	if (DotProductWithUp <= 0)
	{
		ResultAngle = -ResultAngle;
	}

	if (L)
	{
		ResultAngle = -(FMath::Fmod(ResultAngle + (HALF_PI * 0.5f), PI * 2));
	}
	else if (R)
	{
		ResultAngle = -(FMath::Fmod(ResultAngle - (HALF_PI * 0.5f), PI * 2));
	}
	else if (T)
	{
		ResultAngle = -(FMath::Fmod(ResultAngle, PI * 2));
	}
	else if (B)
	{
		ResultAngle = FMath::Fmod(ResultAngle, PI * 2);
	}

	return FMath::RadiansToDegrees(ResultAngle);
}

void UPBBombSiteIndicatorWidget::TickIndicators(float InDeltaTime)
{
	if (nullptr == GetWorld())
		return;

	auto OP = GetOwningPlayer();
	auto OC = GetOwningCharacter();
	if (nullptr == OP || nullptr == OC)
		return;
	
	auto CurrentLevel = OC->GetLevel();
	if (CurrentLevel && CurrentLevel->IsPendingKillOrUnreachable())
		return;

	FVector OwningCharacterLocation = OC->GetActorLocation();
	FVector OwningCharacterForward = OC->GetActorForwardVector();
	FVector2D ViewportSize = UWidgetLayoutLibrary::GetViewportSize(this);
	FVector2D LocalSizeOfViewport = FromViewportPixelToLocal(ViewportSize);
	FVector2D LocalCenterOfViewport = LocalSizeOfViewport * 0.5f;
	FVector DeprojectedCenterOfViewportWorldPosition;
	FVector DeprojectedCenterOfViewportWorldDirection;
	bool DeprojectedCenterPositionResult = UGameplayStatics::DeprojectScreenToWorld(OP, LocalCenterOfViewport, DeprojectedCenterOfViewportWorldPosition, DeprojectedCenterOfViewportWorldDirection);

	for (auto CurData : BombSiteData)
	{
		APBTrigger_BombSite *CurBombSite = CurData.TriggerBombSite;
		if (nullptr == CurBombSite)
			continue;

		UPBBombSiteIndicatorImageWidget *IndicatorImageWidget = CurData.IndicatorImageWidget;
		FVector WorldSpaceTriggerLocation = CurBombSite->GetActorLocation();

		FVector2D OutScreenPosition;
		if (false == UPBGameplayStatics::ProjectWorldToScreen(OP, WorldSpaceTriggerLocation, OutScreenPosition))
		{
			FVector ReflectedTriggerLocation = (WorldSpaceTriggerLocation - DeprojectedCenterOfViewportWorldPosition).MirrorByVector(FVector::UpVector)
				+ DeprojectedCenterOfViewportWorldPosition;

			if (false == UPBGameplayStatics::ProjectWorldToScreen(OP, WorldSpaceTriggerLocation, OutScreenPosition))
			{
				continue;
			}
		}
		
		UCanvasPanelSlot *CanvasPanelSlot = UWidgetLayoutLibrary::SlotAsCanvasSlot(IndicatorImageWidget);
		if (nullptr == CanvasPanelSlot)
			continue;

		FVector2D WidgetCanvasSize = IndicatorImageWidget->GetDesiredSize();
		FVector2D LocalWidgetCanvasSize = FromViewportPixelToLocal(WidgetCanvasSize);
		FVector2D LocalPoint = FromViewportPixelToLocal(OutScreenPosition);
		

		float OffsetByLocalViewportHeight = LocalSizeOfViewport.Y * OffsetContractScreenBorder;
		bool IndicatorIsOutOfScreenToTop = false;
		bool IndicatorIsOutOfScreenToLeft = false;
		bool IndicatorIsOutOfScreenToRight = false;
		bool IndicatorIsOutOfScreenToBottom = false;
		float RightBound = LocalSizeOfViewport.X - (LocalWidgetCanvasSize.X * 0.5f) - OffsetByLocalViewportHeight;
		float LeftBound = OffsetByLocalViewportHeight;//LocalCanvasSize.X * 0.5f;
		float TopBound = OffsetByLocalViewportHeight;//LocalCanvasSize.Y * 0.5f;
		float BottomBound = LocalSizeOfViewport.Y - (LocalWidgetCanvasSize.Y * 0.5f) - OffsetByLocalViewportHeight;
		if (LocalPoint.X >= RightBound)
		{
			LocalPoint.X = RightBound;
			IndicatorIsOutOfScreenToRight = true;
		}
		else if (LocalPoint.X <= LeftBound)
		{
			LocalPoint.X = LeftBound;
			IndicatorIsOutOfScreenToLeft = true;
		}

		if (LocalPoint.Y >= BottomBound)
		{
			LocalPoint.Y = BottomBound;
			IndicatorIsOutOfScreenToBottom = true;
		}
		else if (LocalPoint.Y <= TopBound)
		{
			LocalPoint.Y = TopBound;
			IndicatorIsOutOfScreenToTop = true;
		}

		if (DeprojectedCenterPositionResult)
		{
			AngleAsDegree = GetAngleForWidgetRenderer(
				DeprojectedCenterOfViewportWorldPosition
				, OwningCharacterForward
				, WorldSpaceTriggerLocation
				, IndicatorIsOutOfScreenToLeft
				, IndicatorIsOutOfScreenToTop
				, IndicatorIsOutOfScreenToRight
				, IndicatorIsOutOfScreenToBottom);
		}

		CanvasPanelSlot->SetSize(WidgetCanvasSize);
		CanvasPanelSlot->SetPosition(LocalPoint);

		bool IsOutOfScreen = IndicatorIsOutOfScreenToLeft
			|| IndicatorIsOutOfScreenToTop
			|| IndicatorIsOutOfScreenToRight
			|| IndicatorIsOutOfScreenToBottom;

		//BP_UpdateIndicatorImageWidget(CurBombSite, IndicatorImageWidget, IsOutOfScreen, AngleAsDegree);
		{
			// Update DistanceText
			auto DistanceBorder = IndicatorImageWidget->GetBPV_DistanceBorder();
			auto DistanceText = IndicatorImageWidget->GetBPV_DistanceText();
			int DistanceToTrigger = FMath::TruncToInt(FVector::Dist(OwningCharacterLocation, WorldSpaceTriggerLocation)) / 100;
			if (DistanceToTrigger > 1)
			{
				DistanceBorder->SetVisibility(ESlateVisibility::Visible);
				DistanceText->SetText(FText::FromString(FString::FromInt(DistanceToTrigger) + TEXT("m")));
			}
			else
			{
				DistanceBorder->SetVisibility(ESlateVisibility::Hidden);
			}

			// Rotate DirectionIndicator
			auto DistanceIndicator = IndicatorImageWidget->GetBPV_DirectionIndicator();
			if (IsOutOfScreen)
			{
				DistanceIndicator->SetVisibility(ESlateVisibility::Visible);
				DistanceIndicator->SetRenderAngle(AngleAsDegree);
			}
			else
			{
				DistanceIndicator->SetVisibility(ESlateVisibility::Hidden);
			}
		}
	}
}
