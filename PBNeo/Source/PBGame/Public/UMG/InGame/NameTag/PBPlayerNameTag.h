// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UMG/PBSubWidget.h"
#include "PBPlayerNameTag.generated.h"
class APBCharacter;

/**
 * 
 */
UCLASS()
class PBGAME_API UPBPlayerNameTag : public UPBSubWidget
{
	GENERATED_BODY()
	
public:
	UPBPlayerNameTag(const FObjectInitializer& ObjectInitializer);

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	UFUNCTION(BlueprintCallable, Category = "NameTag")
	FText GetUserName() const;

	// Set Owner character of this widget.
	void SetOwnerCharacter(APBCharacter* PBCharacter){ OwnerCharacter = PBCharacter; }

	void Update();

protected:
	UFUNCTION(BlueprintCallable, Category = "NameTag")
	EPBTeamType GetOwnerCharacterTeamType() const;

	class APBCharacter* GetLocalCharacter() const;
	bool IsTheOwnerCharacterLocal();

	UFUNCTION(BlueprintCallable, Category = "NameTag")
	bool IsItSameTeamWithLocalAndOwnerCharacter();

	UFUNCTION(BlueprintCallable, Category = "NameTag")
	int32 GetDistanceFromLocalPlayer();

	UFUNCTION(BlueprintCallable, Category = "NameTag")
	float GetHealthPercentage() const;

	void UpdateVisiblity();
private:
	class APBCharacter* OwnerCharacter;

};
