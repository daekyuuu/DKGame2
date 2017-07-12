// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UMG/PBSubWidget.h"
#include "PBGameLogWidget.generated.h"

/**
 * 
 */

 /**
 *
 */
USTRUCT()
struct FKillLogAsset
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, Category = "Defaults")
	TSubclassOf<class APBWeapon> WeaponClass;


	UPROPERTY(EditDefaultsOnly, Category = "Defaults")
	UTexture* Icon;
};



UCLASS()
class PBGAME_API UPBGameLogWidget : public UPBSubWidget
{
	GENERATED_BODY()
	
public:
	UPBGameLogWidget(const FObjectInitializer& ObjectInitializer);
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
protected:

	UFUNCTION()
	void NotifyDeath(class APBPlayerState* KillerPlayerState, class APBPlayerState* VictimPlayerState, const UDamageType* KillerDamageType, const FTakeDamageInfo& TakeDamageInfo);

	void AddBattleLog(const FPBLogMessage& msg);

	UFUNCTION()
	void RemoveAndUpdateMessage(int32 index = 0);

	/**
	* Update the UI layout on BP
	*/
	UFUNCTION(BlueprintNativeEvent, Category = "GameLog")
	void UpdateMessages();

	UPROPERTY(BlueprintReadOnly, Category = "GameLog")
	TArray<FPBLogMessage> MsgArray;

private:
	class UTexture* GetWeaponIconById(int32 id);
	
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GameLog")
	int32 MaxGameLog;


private:
	UTexture2D* CacheForHint;

};
