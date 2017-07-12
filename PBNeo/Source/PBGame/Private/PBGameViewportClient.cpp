// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "PBGame.h"
#include "PBGameViewportClient.h"
#include "SlateMaterialBrush.h"
#include "SSafeZone.h"
#include "SThrobber.h"
#include "MoviePlayer.h"

#include "PBGameInstance.h"
#include "PBDialogWidget.h"
#include "PBDialogConfirmWidget.h"
#include "PBDialogNoticeWidget.h"
#include "PBDialogReconnectWidget.h"
#include "PBLocalPlayer.h"
#include "Player/PBPlayerController.h"
#include "Player/PBCharacter.h"
#include "Weapons/PBWeapon.h"

void SPBLoadingScreen::Construct(const FArguments& InArgs)
{
	LoadingScreenBrush = MakeShareable(new FSlateDynamicImageBrush(InArgs._Texture.Get(), InArgs._ImageSize, TEXT("LoadingScreenBrush")));

	ChildSlot
		[
			SNew(SOverlay)
			+ SOverlay::Slot()
		.HAlign(HAlign_Fill)
		.VAlign(VAlign_Fill)
		[
			SNew(SImage)
			.Image(LoadingScreenBrush.Get())
		]
	+ SOverlay::Slot()
		.HAlign(HAlign_Fill)
		.VAlign(VAlign_Fill)
		[
			SNew(SSafeZone)
			.VAlign(VAlign_Bottom)
		.HAlign(HAlign_Right)
		.Padding(10.0f)
		.IsTitleSafe(true)
		[
			SNew(SThrobber)
			.Visibility(this, &SPBLoadingScreen::GetLoadIndicatorVisibility)
		]
		]
		];
}

UPBGameViewportClient::UPBGameViewportClient(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SetSuppressTransitionMessage(true);
}

void UPBGameViewportClient::NotifyPlayerAdded(int32 PlayerIndex, ULocalPlayer* AddedPlayer)
{
	Super::NotifyPlayerAdded(PlayerIndex, AddedPlayer);

 	UPBLocalPlayer* const PBLP = Cast<UPBLocalPlayer>(AddedPlayer);
 	if (PBLP)
 	{
 		PBLP->LoadPersistentUser();
 	}
}

static FVector2D PaniniProjection(FVector2D OM, float d, float s)
{
	float PaniniDirectionXZInvLength = 1.0f / FMath::Sqrt(1.0f + OM.X * OM.X);
	float SinPhi = OM.X * PaniniDirectionXZInvLength;
	float TanTheta = OM.Y * PaniniDirectionXZInvLength;
	float CosPhi = FMath::Sqrt(1.0f - SinPhi * SinPhi);
	float S = (d + 1.0f) / (d + CosPhi);

	return S * FVector2D(SinPhi, FMath::Lerp(TanTheta, TanTheta / CosPhi, s));
}

static FName NAME_d(TEXT("d"));
static FName NAME_s(TEXT("s"));
static FName NAME_FOVMulti(TEXT("FOV Multi"));
static FName NAME_Scale(TEXT("Scale"));

FVector UPBGameViewportClient::PaniniProjectLocation(const FSceneView* SceneView, const FVector& WorldLoc, UMaterialInterface* PaniniParamsMat /*= NULL*/) const
{
	float d = 1.0f;
	float s = 0.0f;
	float FOVMulti = 0.5f;
	float Scale = 1.0f;
	if (PaniniParamsMat != NULL)
	{
		PaniniParamsMat->GetScalarParameterValue(NAME_d, d);
		PaniniParamsMat->GetScalarParameterValue(NAME_s, s);
		PaniniParamsMat->GetScalarParameterValue(NAME_FOVMulti, FOVMulti);
		PaniniParamsMat->GetScalarParameterValue(NAME_Scale, Scale);
	}

	FVector ViewSpaceLoc = SceneView->ViewMatrices.GetViewMatrix().TransformPosition(WorldLoc);
	FVector2D PaniniResult = PaniniProjection(FVector2D(ViewSpaceLoc.X / ViewSpaceLoc.Z, ViewSpaceLoc.Y / ViewSpaceLoc.Z), d, s);

	FMatrix ClipToView = SceneView->ViewMatrices.GetInvProjectionMatrix();
	float ScreenSpaceScaleFactor = (ClipToView.M[0][0] / PaniniProjection(FVector2D(ClipToView.M[0][0], ClipToView.M[1][1]), d, 0.0f).X) * Scale;
	float FOVModifier = ((ClipToView.M[0][0] - 1.0f) * FOVMulti + 1.0f) * ScreenSpaceScaleFactor;

	PaniniResult *= (ViewSpaceLoc.Z * FOVModifier);

	return SceneView->ViewMatrices.GetInvViewMatrix().TransformPosition(FVector(PaniniResult.X, PaniniResult.Y, ViewSpaceLoc.Z));
}

FVector UPBGameViewportClient::PaniniProjectLocationForPlayer(ULocalPlayer* Player, const FVector& WorldLoc, UMaterialInterface* PaniniParamsMat /*= NULL*/) const
{
	if (GetWorld() == nullptr)
	{
		return WorldLoc;
	}

	FSceneViewFamilyContext ViewFamily(FSceneViewFamily::ConstructionValues(Viewport, GetWorld()->Scene, EngineShowFlags).SetRealtimeUpdate(true));

	FVector ViewLocation;
	FRotator ViewRotation;
	FSceneView* SceneView = Player->CalcSceneView(&ViewFamily, ViewLocation, ViewRotation, Viewport);
	return (SceneView != NULL) ? PaniniProjectLocation(SceneView, WorldLoc, PaniniParamsMat) : WorldLoc;
}

void UPBGameViewportClient::Draw(FViewport* InViewport, FCanvas* SceneCanvas)
{
	// apply panini projection to first person weapon

	//struct FSavedTransform
	//{
	//	USceneComponent* Component;
	//	FTransform Transform;
	//	FSavedTransform(USceneComponent* InComp, const FTransform& InTransform)
	//		: Component(InComp), Transform(InTransform)
	//	{}
	//};
	//TArray<FSavedTransform> SavedTransforms;

	//for (FLocalPlayerIterator It(GEngine, GetWorld()); It; ++It)
	//{
	//	APBCharacter* Character = It->PlayerController ? Cast<APBCharacter>(It->PlayerController->GetViewTarget()) : nullptr;
	//	if (Character && Character->IsFirstPerson())
	//	{	
	//		FSceneViewFamilyContext ViewFamily(FSceneViewFamily::ConstructionValues(InViewport, GetWorld()->Scene, EngineShowFlags).SetRealtimeUpdate(true));

	//		FVector ViewLocation;
	//		FRotator ViewRotation;
	//		FSceneView* SceneView = It->CalcSceneView(&ViewFamily, ViewLocation, ViewRotation, InViewport);
	//		if (SceneView == nullptr) continue;

	//		// Character 1p mesh with attached weapon mesh
	//		UMeshComponent* CharacterMesh = Character->GetMesh1P();
	//		if (CharacterMesh)
	//		{
	//			TArray<USceneComponent*> Children = CharacterMesh->GetAttachChildren(); // make a copy in case something below causes it to change
	//			for (USceneComponent* Attachment : Children)
	//			{
	//				// any additional weapon meshes are assumed to be projected in the shader if desired
	//				if (!Attachment->IsPendingKill() && !SavedTransforms.ContainsByPredicate([Attachment](const FSavedTransform& TestItem) { return TestItem.Component == Attachment; }))
	//				{
	//					FVector AdjustedLoc = PaniniProjectLocation(SceneView, Attachment->GetComponentLocation(), CharacterMesh->GetMaterial(0));

	//					new(SavedTransforms) FSavedTransform(Attachment, Attachment->GetComponentTransform());
	//					Attachment->SetWorldLocation(AdjustedLoc);
	//					// hacky solution to attached spline mesh beams that need to update their endpoint
	//					if (Attachment->GetOwner() != nullptr && Attachment->GetOwner() != Character)
	//					{
	//						Attachment->GetOwner()->Tick(0.0f);
	//					}
	//					if (!Attachment->IsPendingKill())
	//					{
	//						Attachment->DoDeferredRenderUpdates_Concurrent();
	//					}
	//				}
	//			}
	//		}
	//	}
	//}

	Super::Draw(InViewport, SceneCanvas);

	//// revert components to their normal location
	//for (const FSavedTransform& RestoreItem : SavedTransforms)
	//{
	//	RestoreItem.Component->SetWorldTransform(RestoreItem.Transform);
	//}
}

#if WITH_EDITOR
void UPBGameViewportClient::DrawTransition(UCanvas* Canvas)
{
	if (GetOuterUEngine() != NULL)
	{
		TEnumAsByte<enum ETransitionType> Type = GetOuterUEngine()->TransitionType;
		switch (Type)
		{
		case TT_Connecting:
			DrawTransitionMessage(Canvas, NSLOCTEXT("GameViewportClient", "ConnectingMessage", "CONNECTING").ToString());
			break;
		case TT_WaitingToConnect:
			DrawTransitionMessage(Canvas, NSLOCTEXT("GameViewportClient", "Waitingtoconnect", "Waiting to connect...").ToString());
			break;	
		}
	}
}
#endif //WITH_EDITOR

void UPBGameViewportClient::ShowDialog(TWeakObjectPtr<ULocalPlayer> PlayerOwner, EPBDialogType::Type DialogType, const FText& Message,
	const FText& Confirm, const FText& Cancel, const FOnClicked& OnConfirm, const FOnClicked& OnCancel)
{
	UE_LOG(LogPlayerManagement, Log, TEXT("UPBGameViewportClient::ShowDialog..."));

	auto GI = Cast<UPBGameInstance>(GetGameInstance());
	if (!GI)
	{
		return;
	}

	if (UMGDialogWidget)
	{
		return;	// Already showing a dialog box
	}
	if (DialogType == EPBDialogType::Type::ControllerDisconnected)
	{
		UPBDialogReconnectWidget* ReconnectionWidget;
		if (PlayerOwner.IsValid() && PlayerOwner->PlayerController)
		{
			ReconnectionWidget = CreateWidget<UPBDialogReconnectWidget>(PlayerOwner->PlayerController, GI->ReconnectDialogClass);
		}
		else
		{
			ReconnectionWidget = CreateWidget<UPBDialogReconnectWidget>(GI, GI->ReconnectDialogClass);
		}
		if (!ReconnectionWidget)
			return;

		ReconnectionWidget->Init(PlayerOwner.Get(), Message, OnConfirm);
		UMGDialogWidget = ReconnectionWidget;
	}
	else if (DialogType == EPBDialogType::Generic && Cancel.IsEmpty())
	{
		UPBDialogNoticeWidget* NoticeWidget;
		if (PlayerOwner.IsValid() && PlayerOwner->PlayerController)
		{
			NoticeWidget = CreateWidget<UPBDialogNoticeWidget>(PlayerOwner->PlayerController, GI->NoticeDialogClass);
		}
		else
		{
			NoticeWidget = CreateWidget<UPBDialogNoticeWidget>(GI, GI->NoticeDialogClass);
		}
		if (!NoticeWidget)
			return;

		NoticeWidget->Init(PlayerOwner.Get(), Message, Confirm, OnConfirm);
		UMGDialogWidget = NoticeWidget;
	}
	else if (DialogType == EPBDialogType::Generic)
	{
		UPBDialogConfirmWidget* ConfirmWidget;
		if (PlayerOwner.IsValid() && PlayerOwner->PlayerController)
		{
			ConfirmWidget = CreateWidget<UPBDialogConfirmWidget>(PlayerOwner->PlayerController, GI->ConfirmDialogClass);
		}
		else
		{
			ConfirmWidget = CreateWidget<UPBDialogConfirmWidget>(GI, GI->ConfirmDialogClass);
		}
		if (!ConfirmWidget)
			return;

		ConfirmWidget->Init(PlayerOwner.Get(), Message, Confirm, OnConfirm, Cancel, OnCancel);
		UMGDialogWidget = ConfirmWidget;
	}
	UMGDialogWidget->Show(1000);

	if (DialogType == EPBDialogType::Type::ControllerDisconnected)
	{
		// Reconnection Widget MUST have valid local player
		check(PlayerOwner.IsValid());

		auto PO = Cast<UPBLocalPlayer>(PlayerOwner.Get());
		if (PO)
		{
			OldUserFocusWidgets.Add(PO, PO->GetFousedWidgetBeforeControllerInvalidated());
		}
		// There is no valid player controller to set userfocus on when controller is disconnected
	}
	else if (DialogType == EPBDialogType::Generic)
	{
		if (PlayerOwner.IsValid())
		{
			auto OldUserFocusWidget = FSlateApplication::Get().GetUserFocusedWidget(PlayerOwner->GetControllerId());
			if (OldUserFocusWidget.IsValid())
			{
				OldUserFocusWidgets.Add(PlayerOwner.Get(), OldUserFocusWidget);
			}

			UPBUserWidget::NavigateUserFocus(PlayerOwner.Get(), UMGDialogWidget);

			if (PlayerOwner->GetPlayerController(GetWorld()))
			{
				FInputModeUIOnly InputMode;
				PlayerOwner->GetPlayerController(GetWorld())->SetInputMode(InputMode);
			}
		}
		else // No specific player is recepient, borrow everyone's userfocus
		{
			for (auto It = GEngine->GetLocalPlayerIterator(GetWorld()); It; ++It)
			{
				auto OldUserFocusWidget = FSlateApplication::Get().GetUserFocusedWidget((*It)->GetControllerId());
				if (OldUserFocusWidget.IsValid())
				{
					OldUserFocusWidgets.Add((*It), OldUserFocusWidget);
				}

				UPBUserWidget::NavigateUserFocus((*It), UMGDialogWidget);

				if ((*It)->GetPlayerController(GetWorld()))
				{
					FInputModeUIOnly InputMode;
					(*It)->GetPlayerController(GetWorld())->SetInputMode(InputMode);
				}
			}
		}
	}

	OldKeyboardFocusWidget = FSlateApplication::Get().GetKeyboardFocusedWidget();

	UPBUserWidget::NavigateKeyboardFocus(UMGDialogWidget);
	
	if (GetGameInstance() && GetGameInstance()->GetFirstLocalPlayerController())
	{
		FInputModeUIOnly InputMode;
		GetGameInstance()->GetFirstLocalPlayerController()->SetInputMode(InputMode);
	}
}

void UPBGameViewportClient::HideDialog()
{
	if (UMGDialogWidget)
	{
		// Make sure we do not return focus to old widget when we do not have it ourselves
		{
			for (auto& Entry : OldUserFocusWidgets)
			{
				if (Entry.Key.IsValid()
					&&
					Entry.Value.IsValid()
					&&
					!UPBUserWidget::ContainUserFocus(Entry.Key.Get(), UMGDialogWidget))
				{
					Entry.Value.Reset();
				}
			}

			if (OldKeyboardFocusWidget.IsValid()
				&&
				!UPBUserWidget::ConTainKeyboardFocus(UMGDialogWidget))
			{
				OldKeyboardFocusWidget.Reset();
			}
		}
		
		UMGDialogWidget->Hide();
		UMGDialogWidget = NULL;

		// Return focus to old widgets
		{
			for (auto& Entry : OldUserFocusWidgets)
			{
				if (Entry.Key.IsValid()
					&&
					Entry.Value.IsValid())
				{
					UPBUserWidget::NavigateUserFocus(Entry.Key.Get(), Entry.Value);
				}
			}
			OldUserFocusWidgets.Reset();
		}

		if (OldKeyboardFocusWidget.IsValid())
		{
			UPBUserWidget::NavigateKeyboardFocus(OldKeyboardFocusWidget);
			OldKeyboardFocusWidget.Reset();
		}
	}
}

bool UPBGameViewportClient::IsShowingDialog() const
{
	return UMGDialogWidget != NULL;
}

void UPBGameViewportClient::ShowLoadingScreen()
{
	if (LoadingScreenWidget.IsValid())
	{
		return;
	}

	auto GI = Cast<UPBGameInstance>(GetGameInstance());
	if (GI)
	{
		LoadingScreenTextureInst = GI->LoadingScreenTexture.Get();
		if (!LoadingScreenTextureInst)
		{
			LoadingScreenTextureInst = GI->LoadingScreenTexture.LoadSynchronous();
		}
		if (LoadingScreenTextureInst)
		{
			LoadingScreenWidget = SNew(SPBLoadingScreen)
				.Texture(LoadingScreenTextureInst)
				.ImageSize(FVector2D(1920, 1080));

			if (LoadingScreenWidget.IsValid())
			{
				AddViewportWidgetContent(LoadingScreenWidget.ToSharedRef(), 10000);
			}
		}	
	}
}

void UPBGameViewportClient::HideLoadingScreen()
{
	if (!LoadingScreenWidget.IsValid())
	{
		return;
	}

	RemoveViewportWidgetContent(LoadingScreenWidget.ToSharedRef());
	LoadingScreenWidget.Reset();
}

void UPBGameViewportClient::SetUpMoviePlayerLoadingScreen()
{
	auto GI = Cast<UPBGameInstance>(GetGameInstance());
	if (GI)
	{
		MoviePlayer_LoadingScreenTextureInst = GI->LoadingScreenTexture.Get();
		if (!MoviePlayer_LoadingScreenTextureInst)
		{
			MoviePlayer_LoadingScreenTextureInst = GI->LoadingScreenTexture.LoadSynchronous();
		}
		if (MoviePlayer_LoadingScreenTextureInst)
		{
			MoviePlayer_LoadingScreenWidget = SNew(SPBLoadingScreen)
				.Texture(MoviePlayer_LoadingScreenTextureInst)
				.ImageSize(FVector2D(1920, 1080));

			if (MoviePlayer_LoadingScreenWidget.IsValid())
			{
				FLoadingScreenAttributes LoadingScreenAttr;
				LoadingScreenAttr.bAutoCompleteWhenLoadingCompletes = true;
				LoadingScreenAttr.WidgetLoadingScreen = MoviePlayer_LoadingScreenWidget;
				GetMoviePlayer()->SetupLoadingScreen(LoadingScreenAttr);
			}
		}
	}
}