// Fill out your copyright notice in the Description page of Project Settings.

#include "PBGame.h"
#include "PBGameLogWidget.h"
#include "PBPlayerState.h"
#include "PBDamageType.h"
#include "PBWeapon.h"
#include "PBBulletBase.h"

#include "PBGameInstance.h"
#include "PBItemTableManager.h"
#include "PBItemTable_Weap.h"
#include "PBDamageType.h"

UPBGameLogWidget::UPBGameLogWidget(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	MaxGameLog = 5;
	
}

void UPBGameLogWidget::NativeConstruct()
{
	Super::NativeConstruct();

	BindWidgetEvent(OnDeath, UPBGameLogWidget::NotifyDeath);
}

void UPBGameLogWidget::NativeDestruct()
{
	UnbindWidgetEvent(OnDeath);

	Super::NativeDestruct();
}

void UPBGameLogWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
}


void UPBGameLogWidget::NotifyDeath(class APBPlayerState* KillerPlayerState, class APBPlayerState* VictimPlayerState, const UDamageType* KillerDamageType, const FTakeDamageInfo& TakeDamageInfo)
{
	if (KillerPlayerState && VictimPlayerState && KillerPlayerState->GetOwnerCharacter())
	{ 
		FPBLogMessage msg;
		msg.VerbIcon = nullptr;
		msg.bIsFilled = true;
		msg.KillerPlayerState = KillerPlayerState;
		msg.VictimPlayerState = VictimPlayerState;

		if (TakeDamageInfo.bIsHeadShot)
		{
			msg.VerbString = "HeadShot";
		}

		auto w = GetWeaponIconById(TakeDamageInfo.ItemId);
		if (w)
		{
			msg.VerbIcon = w;
		}

		// add and update UI
		AddBattleLog(msg);
		UpdateMessages();


		// remove oldest message after few seconds
		if (GetOwningPlayer() && GetOwningPlayer()->GetWorld())
		{
			FTimerHandle TempHandle;
			FTimerDelegate TempDelegate = FTimerDelegate::CreateUObject(this, &UPBGameLogWidget::RemoveAndUpdateMessage, 0);
			
			if (GetWorld())
			{
				GetWorld()->GetTimerManager().SetTimer(TempHandle, TempDelegate, 10.0f, false);
			}			
		}
	}

		
}

void UPBGameLogWidget::AddBattleLog(const FPBLogMessage& msg)
{
	MsgArray.Add(msg);

	if (MsgArray.Num() > MaxGameLog)
	{
		MsgArray.RemoveAt(0);
	}

}

void UPBGameLogWidget::RemoveAndUpdateMessage(int32 index /*= 0*/)
{
	if (MsgArray.Num() > index )
	{
		MsgArray.RemoveAt(index);
		UpdateMessages();

	}
}

class UTexture* UPBGameLogWidget::GetWeaponIconById(int32 id)
{

	auto Mgr = UPBGameplayStatics::GetItemTableManager(this);
	if (Mgr)
	{
		FPBWeaponTableData* D = Mgr->GetWeaponTableData(id);
		if (D)
		{
			FString IconPath = "/Game/UMG/Images/NewInGame/Icon/IconResKilllog/";
			IconPath += D->IconResKilllog + "." + D->IconResKilllog;

			// Note that LoadObject<>() does the equivalent of FindObject under the covers, so it's not necessary to try to find the object first and then load it.
			auto Tex = LoadObject<UTexture2D>(CacheForHint ? CacheForHint : nullptr, *IconPath);
			if (Tex)
			{
				CacheForHint = Tex;
			}

			return Tex;

		}
		else
		{
			UE_LOG(LogUI, Warning, TEXT("UPBGameLogWidget::GetWeaponIconById: Can't find weapon data from Id: %d"), id);
		}
	}
	
	return nullptr;

}

void UPBGameLogWidget::UpdateMessages_Implementation()
{

}
