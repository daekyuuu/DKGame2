// Fill out your copyright notice in the Description page of Project Settings.

#include "PBGame.h"
#include "PBCrossHairWidget.h"
#include "PBCharacter.h"
#include "PBWeapon.h"
#include "PBCrossHairImageWidget.h"
#include "WidgetLayoutLibrary.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/CanvasPanel.h"

PBGetterWidgetBlueprintVariable_Implementation(UCanvasPanel, UPBCrossHairWidget, Canvas);

UPBCrossHairWidget::UPBCrossHairWidget(const FObjectInitializer &ObjectInitializer)
	: Super(ObjectInitializer)
{
	CrossHairConeLength = 2.0f;
}

void UPBCrossHairWidget::NativeConstruct()
{
	Super::NativeConstruct();

	InitAimPoint();

	BindWidgetEvent(OnDeath, UPBCrossHairWidget::NotifyDeath);
	BindWidgetEvent(OnSpawn, UPBCrossHairWidget::NotifySpawn);
	BindWidgetEvent(OnZoom, UPBCrossHairWidget::NotifyZoom);
	BindWidgetEvent(OnWeaponChange, UPBCrossHairWidget::NotifyWeaponChange);

	UpdateCrossHairImage();
	ForceUpdateAimPointOnScreen();
}

void UPBCrossHairWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	UpdateAimPoint(InDeltaTime);

	if (nullptr == CrossHairImageWidgetRef)
		return;

	// Update Crosshair Size
	{
		float ExpansionSize = GetExpansionSize();
		float GapSize = GapAngleToCrossHairGapSize(ExpansionSize);
		CrossHairImageWidgetRef->SetSize(GapSize);
	}

	// Update Crosshair Position
	{
		bool bShouldFixCrossHairPosition = ShouldFixCrossHairPosition();
		if (bShouldFixCrossHairPosition)
		{
			FVector2D LocalPosition = FromViewportPixelToLocal(GetAimPointInViewport());
			SetCrossHairPosition(LocalPosition);
		}
	}

	// Update Crosshair Aiming at Enemy State
	{
		bool bIsAimingAtEmeny = GetAimingAtEnemy();
		CrossHairImageWidgetRef->SetAimingAtEnemy(bIsAimingAtEmeny);
	}
}

void UPBCrossHairWidget::NativeDestruct()
{
	RemoveCrossHairImage();

	UnbindWidgetEvent(OnDeath);
	UnbindWidgetEvent(OnSpawn);
	UnbindWidgetEvent(OnZoom);
	UnbindWidgetEvent(OnWeaponChange);
	
	Super::NativeDestruct();
}

APBCharacter* UPBCrossHairWidget::GetOwningCharacter()
{
	auto PC = Cast<APBPlayerController>(GetOwningPlayer());
	if (PC)
	{
		return Cast<APBCharacter>(PC->GetCharacter());
	}
	return nullptr;
}

void UPBCrossHairWidget::NotifySpawn(class APBPlayerState* SpawnedPlayerState)
{
	UpdateCrossHairImage();
	ForceUpdateAimPointOnScreen();
}

void UPBCrossHairWidget::NotifyZoom(bool NowZooming)
{
	UpdateCrossHairImage();
	ForceUpdateAimPointOnScreen();
}

void UPBCrossHairWidget::NotifyWeaponChange()
{
	UpdateCrossHairImage();
	ForceUpdateAimPointOnScreen();
}

void UPBCrossHairWidget::NotifyDeath(class APBPlayerState* KillerPlayerState, class APBPlayerState* VictimPlayerState, const UDamageType* KillerDamageType, const FTakeDamageInfo& TakeDamageInfo)
{
	APBCharacter *Viewer = GetViewer();
	if (nullptr == Viewer)
	{
		RemoveCrossHairImage();
		return;
	}

	if (VictimPlayerState && Viewer == VictimPlayerState->GetOwner())
	{
		RemoveCrossHairImage();
	}
}

void UPBCrossHairWidget::ForceUpdateAimPointOnScreen()
{
	auto OP = GetOwningPlayer();
	if (nullptr == OP)
		return;

	auto Viewer = GetViewer();
	if (nullptr != Viewer)
	{
		FVector WorldSpaceHitLocation = GetPredictedHitLocation();

		FVector2D OutScreenPosition;
		if (UPBGameplayStatics::ProjectWorldToScreen(OP, WorldSpaceHitLocation, OutScreenPosition))
		{
			InterpAimPointOnScreen = OutScreenPosition;
		}
	}
}

FVector2D UPBCrossHairWidget::GetAimPointInViewport()
{
	return InterpAimPointOnScreen;
}

void UPBCrossHairWidget::InitAimPoint()
{
	if (GEngine && GEngine->GameViewport && GEngine->GameViewport->Viewport)
	{
		const FVector2D ViewportSize = UWidgetLayoutLibrary::GetViewportSize(this);
		const FVector2D  ViewportCenter = FVector2D(ViewportSize.X / 2, ViewportSize.Y / 2);
		InterpAimPointOnScreen = ViewportCenter;
	}
}

void UPBCrossHairWidget::UpdateAimPoint(float InDeltaTime)
{
	auto OP = GetOwningPlayer();
	if (nullptr == OP)
		return;

	auto Viewer = GetViewer();
	if (nullptr != Viewer)
	{
		FVector WorldSpaceHitLocation = GetPredictedHitLocation();

		FVector2D OutScreenPosition;
		if (UPBGameplayStatics::ProjectWorldToScreen(OP, WorldSpaceHitLocation, OutScreenPosition))
		{
			InterpAimPointOnScreen = OutScreenPosition;//FMath::Vector2DInterpTo(InterpAimPointOnScreen, OutScreenPosition, InDeltaTime, AimPointInterpRate);
		}
	}
}

FVector UPBCrossHairWidget::GetPredictedHitLocation()
{
	auto OP = GetOwningPlayer();
	if (nullptr == OP)
		return FVector::ZeroVector;

	auto Viewer = GetViewer();
	if (nullptr != Viewer)
	{
		FRotator FireRotation = Viewer->GetFirstPersonViewTransform().Rotator();

		FVector PlayerCamLocation; 
		FRotator PlayerCamRotation;
		OP->GetPlayerViewPoint(PlayerCamLocation, PlayerCamRotation);

		return PlayerCamLocation + FireRotation.Vector() * FirePredictionScanDistance;
	}

	ensure(OP && "Oops! Why OwningCharacter had been to be nullptr.");

	return FVector::ZeroVector;
}

APBCharacter *UPBCrossHairWidget::GetViewer() const
{
	auto World = GetWorld();
	if (false == ensure(World))
		return false;

	auto PlayerCharacter = Cast<APBCharacter>(UGameplayStatics::GetPlayerCharacter(World, 0));
	bool bIsViewingOwnself = false;
	if (nullptr == PlayerCharacter)
		return nullptr;

	if (PlayerCharacter == GetOwningPlayerPawn() && PlayerCharacter->IsViewingOwnself())
	{
		return PlayerCharacter;
	}

	auto TargetPlayer = PlayerCharacter->GetTargetViewPlayer();
	if (TargetPlayer)
	{
		auto TC = TargetPlayer->GetOwnerCharacter();
		if (TC)
		{
			return TC;
		}
	}

	return nullptr;
}

bool UPBCrossHairWidget::GetViewerIsZooming() const
{
	auto Viewer = GetViewer();
	if (nullptr == Viewer)
		return false;

	bool bIsZooming = Viewer->IsZooming();
	return bIsZooming;
}

bool UPBCrossHairWidget::GetAimingAtEnemy()
{
	APBCharacter *VC = GetViewer();
	if (VC)
	{
		APBWeapon *Weapon = VC->GetCurrentWeapon();
		if (Weapon)
		{
			APBCharacter *ATC = VC->GetAmingTargetCache();
			if (ATC && ATC->IsAlive())
			{
				return VC->GetTeamType() != ATC->GetTeamType();
			}
		}
	}

	return false;
}

float UPBCrossHairWidget::GetExpansionSize()
{
	APBCharacter *VC = GetViewer();
	if (VC && VC->GetCurrentWeapon())
	{
		APBWeapon *Weapon = VC->GetCurrentWeapon();
		if (Weapon)
		{
			float ExpansionSize = Weapon->GetExpansionSize();
			return ExpansionSize;
		}
	}

	return 0.f;
}

// Crosshair Size
float UPBCrossHairWidget::GapAngleToCrossHairGapSize(float GapAngle)
{
	float TanGapAngle = FMath::Tan(PI / (180.f) * GapAngle);
	return TanGapAngle * CrossHairConeLength;
}

// Corsshair Style
void UPBCrossHairWidget::AddCrossHairImage(UClass *InCrossHairImageClass)
{
	auto World = GetWorld();
	if (false == ensure(World))
		return;

	auto OP = GetOwningPlayer();
	if (false == ensure(OP))
		return;

	if (nullptr == InCrossHairImageClass)
		return;

	CrossHairImageWidgetRef = CreateWidget<UPBCrossHairImageWidget>(OP, InCrossHairImageClass);
	auto NewWidget = GetBPV_Canvas()->AddChild(CrossHairImageWidgetRef);
	auto PanelSlot = Cast<UCanvasPanelSlot>(NewWidget);
	if (PanelSlot)
	{
		FAnchorData AnchorData;
		AnchorData.Offsets = FMargin(0, 0, 0, 0);
		AnchorData.Anchors = FAnchors(0, 0, 1.0f, 1.0f);
		AnchorData.Alignment = FVector2D(0.5f, 0.5f);
		PanelSlot->SetLayout(AnchorData);
	}
}
void UPBCrossHairWidget::RemoveCrossHairImage()
{
	if (nullptr == CrossHairImageWidgetRef)
		return;

	CrossHairImageWidgetRef->RemoveFromParent();
	CrossHairImageWidgetRef = nullptr;
}
void UPBCrossHairWidget::UpdateCrossHairImage()
{
	auto Viewer = GetViewer();
	if (Viewer != GetOwningPlayerPawn())
	{
		int i = 3;
	}

	RemoveCrossHairImage();

	bool bViewerIsZoomming = GetViewerIsZooming();
	if (bViewerIsZoomming)
	{
		AddCrossHairImage(GetZoomCrossHairImageWidgetClass());
	}
	else
	{
		AddCrossHairImage(GetCrossHairImageWidgetClass());
	}
}

// Crosshair Position
void UPBCrossHairWidget::SetCrossHairPosition(const FVector2D &InLocalPosition)
{
	FVector2D Offset = InLocalPosition - GetLocalCentre();
	auto Margin = FMargin(Offset.X, Offset.Y, Offset.X * -1, Offset.Y * -1);
	
	auto CanvasSlot = UWidgetLayoutLibrary::SlotAsCanvasSlot(CrossHairImageWidgetRef);
	if (nullptr == CanvasSlot)
		return;

	CanvasSlot->SetOffsets(Margin);
}
FVector2D UPBCrossHairWidget::GetLocalCentre()
{
	return GetCachedLocalSize() / 2.0f;
}
bool UPBCrossHairWidget::ShouldFixCrossHairPosition()
{
	if (GetViewerIsZooming())
	{
		return GetZoomCrossHairPositionFixed();
	}

	return GetCrossHairPositionFixed();
}


bool UPBCrossHairWidget::GetCrossHairPositionFixed()
{
	APBCharacter *VC = GetViewer();
	if (VC)
	{
		APBWeapon *Weapon = VC->GetCurrentWeapon();
		if (Weapon)
		{
			return Weapon->GetCrossHairPositionFixed();
		}
	}
	return true;
}

TSubclassOf<UPBCrossHairImageWidget> UPBCrossHairWidget::GetCrossHairImageWidgetClass()
{
	APBCharacter *VC = GetViewer();
	if (VC)
	{
		if (VC != GetOwningPlayerPawn())
		{
			int i = 3;
		}

		APBWeapon *Weapon = VC->GetCurrentWeapon();
		if (Weapon)
		{
			return Weapon->GetCrossHairImageWidgetClass();
		}
	}

	return TSubclassOf<UPBCrossHairImageWidget>();
}

bool UPBCrossHairWidget::GetZoomCrossHairPositionFixed()
{
	APBCharacter *VC = GetViewer();
	if (VC)
	{
		APBWeapon *Weapon = VC->GetCurrentWeapon();
		if (Weapon)
		{
			return Weapon->GetZoomCrossHairPositionFixed();
		}
	}

	return true;
}

TSubclassOf<UPBCrossHairImageWidget> UPBCrossHairWidget::GetZoomCrossHairImageWidgetClass()
{
	APBCharacter *VC = GetViewer();
	if (VC)
	{
		if (VC != GetOwningPlayerPawn())
		{
			int i = 3;
		}

		APBWeapon *Weapon = VC->GetCurrentWeapon();
		if (Weapon)
		{
			return Weapon->GetZoomCrossHairImageWidgetClass();
		}
	}
	
	return TSubclassOf<UPBCrossHairImageWidget>();
}