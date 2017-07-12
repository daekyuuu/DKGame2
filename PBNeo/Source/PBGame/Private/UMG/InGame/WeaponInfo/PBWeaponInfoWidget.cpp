// Fill out your copyright notice in the Description page of Project Settings.

#include "PBGame.h"
#include "PBWeaponInfoWidget.h"

#include "PBCharacter.h"
#include "PBWeapon.h"




void UPBWeaponInfoWidget::NativeConstruct()
{
	Super::NativeConstruct();

	BindWidgetEvent(OnReloadEnded, UPBWeaponInfoWidget::NotifyWeaponChange);
	BindWidgetEvent(OnEquipFinished, UPBWeaponInfoWidget::NotifyWeaponChange);
	BindWidgetEvent(OnWeaponChange, UPBWeaponInfoWidget::NotifyWeaponChange);

	// call it at first time to make sure.
	NotifyWeaponChange();
}

void UPBWeaponInfoWidget::NativeDestruct()
{

	UnbindWidgetEvent(OnReloadEnded);
	UnbindWidgetEvent(OnEquipFinished);
	UnbindWidgetEvent(OnWeaponChange);

	Super::NativeDestruct();

}

int32 UPBWeaponInfoWidget::GetCurrentAmmoInClip() const
{
	auto weap = GetCurrentWeapon();
	if (false == IsSafe(weap))
	{
		return 0;
	}

	return weap->GetCurrentAmmoInClip();
}

int32 UPBWeaponInfoWidget::GetTotalAmmo() const
{ 

	auto weap = GetCurrentWeapon();
	if (false == IsSafe(weap))
	{
		return 0;
	}

	const int32 AmmoPerClip = weap->GetAmmoPerClip();
	int32 AmmoInClip = weap->GetCurrentAmmoInClip();
	int32 CurrAmmo = weap->GetCurrentAmmo();

	return CurrAmmo - AmmoInClip;
}


float UPBWeaponInfoWidget::GetHealthPercentage() const
{
	float HealthPercent = 1.0f;

	auto owningChar = GetOwningCharacter();
	if (false == IsSafe(owningChar))
	{
		return 0.0f;
	}

	return owningChar->GetHealthPercentage();
}


int32 UPBWeaponInfoWidget::GetCeiledHealth() const
{
	auto OwningChar = GetOwningCharacter();
	if (false == IsSafe(OwningChar))
	{
		return 0.0f;
	}

	// If the health is 12.1f, then return 13;
	return FMath::CeilToInt(OwningChar->GetHealth());

}

void UPBWeaponInfoWidget::NotifyWeaponChange()
{
	UpdateLayout();
}

///////////////////////////////// Helper functions /////////////////////////////////////////

class APBWeapon* UPBWeaponInfoWidget::GetCurrentWeapon() const
{
	if (GetOwningPlayerPawn() && GetOwningPlayerPawn()->IsLocallyControlled())
	{
		APBCharacter* ownChar = Cast<APBCharacter>(GetOwningPlayerPawn());
		if (nullptr != ownChar)
		{
			return ownChar->GetCurrentWeapon();
		}
	}

	return nullptr;
}


FText UPBWeaponInfoWidget::GetWeapClassName() const
{
	FText NameText;
	if (GetCurrentWeapon())
	{
		auto WeapClass = GetCurrentWeapon()->GetClass();
		if (WeapClass)
		{
			FString NameString = WeapClass->GetName();
			int index = NameString.Find("_C");
			NameString.RemoveAt(index, 2);

			NameText = FText::FromString(NameString);
		}
	}

	return NameText;


}



UObject* UPBWeaponInfoWidget::GetWeaponIcon() const
{
	auto weap = GetCurrentWeapon();
	if (false == IsSafe(weap))
	{
		return nullptr;
	}

	return weap->DefaultIcon;
}

EWeaponType UPBWeaponInfoWidget::GetCurrWeapType()
{
	auto weap = GetCurrentWeapon();
	if (false == IsSafe(weap))
	{
		return EWeaponType::AR;
	}

	return weap->GetWeaponType();
}



FText UPBWeaponInfoWidget::GetGrenadeAmmo() const
{
	APBWeapon* Weap = nullptr;
	if (GetOwningCharacter())
	{
		Weap = GetOwningCharacter()->GetWeapon(EWeaponSlot::Grenade);
		if (Weap)
		{
			return IntToText(Weap->GetCurrentAmmo());
		}

	}

	return IntToText(0);
}

FText UPBWeaponInfoWidget::GetSpecialAmmo() const
{
	APBWeapon* Weap = nullptr;
	if (GetOwningCharacter())
	{
		Weap = GetOwningCharacter()->GetWeapon(EWeaponSlot::Special);
		if (Weap)
		{
			return IntToText(Weap->GetCurrentAmmo());
		}

	}

	return IntToText(0);
}

