// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UMG/PBSubWidget.h"
#include "PBButtonSkin.generated.h"

/**
 *  To be deleted!!! It is just here because some legacy ui subclasses it!!!
 */
UCLASS()
class PBGAME_API UPBButtonSkin : public UPBSubWidget
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = ButtonSkin)
	void EnterNormalState();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = ButtonSkin)
	void EnterFocusedState();
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = ButtonSkin)
	void EnterClickedState();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = ButtonSkin)
	void SetTitle(const FText& Input);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = ButtonSkin)
	void SetTitleSize(int32 Size);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = ButtonSkin)
	void SetSubtitle(const FText& Input);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = ButtonSkin)
	void SetSubtitleSize(int32 Size);	
};
