// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "PBTypes.h"
#include "PBPlayerController_Menu.generated.h"

UCLASS()
class APBPlayerController_Menu : public APlayerController
{
	GENERATED_UCLASS_BODY()

public:

	/** After game is initialized */
	virtual void PostInitializeComponents() override;

	/* ------------------------------------------------------------------------------- */
	// Scene Capture
	/* ------------------------------------------------------------------------------- */
protected:
	UPROPERTY(EditAnywhere, Category = "Blur")
		class UPBSceneCaptureComponent2D* PBSceneCaptureComp2D;

	UPROPERTY(EditAnywhere, Category = "SceneCapture")
		UTextureRenderTarget2D* RenderTargetForBlur;

};

