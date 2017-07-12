// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "PBGame.h"
#include "Player/PBPlayerController_Menu.h"
#include "PBCharacterScene.h"
#include "PBCheatManager.h"
#include "PBSceneCaptureComponent2D.h"

APBPlayerController_Menu::APBPlayerController_Menu(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	CheatClass = UPBCheatManager::StaticClass();

	PBSceneCaptureComp2D = CreateDefaultSubobject<UPBSceneCaptureComponent2D>("PPSceneCaptureComp2D");
	if (PBSceneCaptureComp2D)
	{
		PBSceneCaptureComp2D->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
	}
}

void APBPlayerController_Menu::PostInitializeComponents() 
{
	Super::PostInitializeComponents();

	EnableCheats();
	bShowMouseCursor = true;

	if (PBSceneCaptureComp2D)
	{
		if (IsLocalPlayerController() && RenderTargetForBlur)
		{
			PBSceneCaptureComp2D->TextureTarget = RenderTargetForBlur;
			PBSceneCaptureComp2D->Activate();
		}
		else
		{
			PBSceneCaptureComp2D->Deactivate();
		}
	}


}
