// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "PBTypes.h"
#include "PBGameViewportClient.generated.h"

class UWidget;
class UPBDialogWidget;
class SPBConfirmationDialog;

class SPBLoadingScreen : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SPBLoadingScreen) {}

	SLATE_ARGUMENT(TWeakObjectPtr<UTexture2D>, Texture)

	SLATE_ARGUMENT(FVector2D, ImageSize)

	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);

private:
	EVisibility GetLoadIndicatorVisibility() const
	{
		return EVisibility::Visible;
	}

	/** loading screen image brush */
	TSharedPtr<struct FSlateDynamicImageBrush> LoadingScreenBrush;
};

UCLASS(Within=Engine, transient, config=Engine)
class UPBGameViewportClient : public UGameViewportClient
{
	GENERATED_UCLASS_BODY()

public:
 	void NotifyPlayerAdded( int32 PlayerIndex, ULocalPlayer* AddedPlayer ) override;

	/** panini project the given location using the player's view
	* PaniniParamsMat will be used to grab parameters for the projection if available, otherwise reasonable default values are used
	*/
	FVector PaniniProjectLocation(const FSceneView* SceneView, const FVector& WorldLoc, UMaterialInterface* PaniniParamsMat = NULL) const;
	/** calls PaniniProjectLocation() with a SceneView representing the player's view (slower, don't use if SceneView is already available) */
	FVector PaniniProjectLocationForPlayer(ULocalPlayer* Player, const FVector& WorldLoc, UMaterialInterface* PaniniParamsMat = NULL) const;

	virtual void Draw(FViewport* Viewport, FCanvas* SceneCanvas) override;

#if WITH_EDITOR
	void DrawTransition(class UCanvas* Canvas) override;
#endif //WITH_EDITOR

	/* ------------------------------------------------------------------------ */
	// Shwoing/Hiding Dialog
	/* ------------------------------------------------------------------------ */

public:
	void ShowDialog(TWeakObjectPtr<ULocalPlayer> PlayerOwner, EPBDialogType::Type DialogType, 
		const FText& Message, const FText& Confirm, const FText& Cancel, const FOnClicked& OnConfirm, const FOnClicked& OnCancel);
	void HideDialog();

	bool IsShowingDialog() const;

protected:
	TMap<TWeakObjectPtr<ULocalPlayer>, TSharedPtr<SWidget>> OldUserFocusWidgets;

	TSharedPtr<SWidget> OldKeyboardFocusWidget;

	UPROPERTY()
	UPBDialogWidget* UMGDialogWidget;

	UPROPERTY()
	UTexture2D* LoadingScreenTextureInst;

	TSharedPtr<SPBLoadingScreen> LoadingScreenWidget;

	/* ------------------------------------------------------------------------ */
	// Shwoing/Hiding Loading Screen
	/* ------------------------------------------------------------------------ */

public:
	void ShowLoadingScreen();
	void HideLoadingScreen();

public:
	void SetUpMoviePlayerLoadingScreen();
private:
	UPROPERTY()
	UTexture2D* MoviePlayer_LoadingScreenTextureInst;

	TSharedPtr<SPBLoadingScreen> MoviePlayer_LoadingScreenWidget;

	/* ------------------------------------------------------------------------ */
	// End
	/* ------------------------------------------------------------------------ */
};