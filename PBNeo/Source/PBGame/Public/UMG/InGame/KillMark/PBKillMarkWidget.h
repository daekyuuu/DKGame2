// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UMG/PBSubWidget.h"
#include "FPBKillMarkConditionChecker.h"
#include "PBKillMarkWidget.generated.h"

/**
 * 
 */
UCLASS()
class PBGAME_API UPBKillMarkWidget : public UPBSubWidget
{
	GENERATED_BODY()

	PBGetterWidgetBlueprintProperty(UWidgetAnimation, KillMarkIn);

	PBGetterWidgetBlueprintVariable(UTextBlock, KillMessage);

public:
	UPBKillMarkWidget(const FObjectInitializer& ObjectInitializer);
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	virtual void OnAnimationFinished_Implementation(const UWidgetAnimation* Animation) override;

protected:
	
	//UFUNCTION(BlueprintNativeEvent, Category = "KillMark")
	void NotifyKillMark(const TArray<int32>& killMarkIds, EWeaponType WeapType);


	//UFUNCTION(BlueprintImplementableEvent, Category = "KillMark")
	void NotifyWarmupStarted();


	//UFUNCTION(BlueprintCallable, Category = "KillMark")
	void EnqueueKillMark(const TArray<int32>& killMarkIds, EWeaponType WeapType);

	//UFUNCTION(BlueprintCallable, Category = "KillMark")
	void DequeueKillMark();

	//UFUNCTION(BlueprintCallable, Category = "KillMark")
	void EmptyKillMarkQueue();

	//UFUNCTION(BlueprintCallable, Category = "KillMark")
	FText GetKillMarkText() const;

	void PlayAnimationNextKillMark();

	//UFUNCTION(BlueprintCallable, Category = "KillMark")
	void PlayKillMarkSound() const;


	/**
	* return true if KillMarkQueue have no element
	*/
	//UFUNCTION(BlueprintCallable, Category = "KillMark")
	bool IsTheKillMarkQueueEmpty() const;

protected:
	//UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "KillMark")
	float FirstAnimationDelayTime;
	bool bIsPlayingKillMark;

protected:
	TArray<KillMarkType> KillMarkQueue;
	
};
