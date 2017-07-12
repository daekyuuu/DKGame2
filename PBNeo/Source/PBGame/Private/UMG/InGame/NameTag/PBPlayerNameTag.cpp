// Fill out your copyright notice in the Description page of Project Settings.

#include "PBGame.h"
#include "PBPlayerNameTag.h"
#include "Player/PBPlayerState.h"
#include "PBPlayerController.h"
#include "Player/PBCharacter.h"

UPBPlayerNameTag::UPBPlayerNameTag(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}

// Visible == true && 시야에 들어왔을 때만 호출된다. 
// 따라서 이미 Hide 상태인 상황에서 Show 상태로 변경하기 위해서는 수동으로 업데이트 해야한다.
void UPBPlayerNameTag::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);	
}

void UPBPlayerNameTag::NativeConstruct()
{
	Super::NativeConstruct();
}

void UPBPlayerNameTag::NativeDestruct()
{
	Super::NativeDestruct();
}

FText UPBPlayerNameTag::GetUserName() const
{
	FText username = FText::FromString("UserNameUserName");
	if (OwnerCharacter && OwnerCharacter->PlayerState)
	{
		auto ps = Cast<APBPlayerState>(OwnerCharacter->PlayerState);
		if (ps)
		{
			username = FText::FromString(ps->GetShortPlayerName());
		}
	}

	return username;
}

void UPBPlayerNameTag::Update()
{
	UpdateVisiblity();
}

EPBTeamType UPBPlayerNameTag::GetOwnerCharacterTeamType() const
{
	if (OwnerCharacter)
	{
		return OwnerCharacter->GetTeamType();
	}

	return EPBTeamType::Max;
}

APBCharacter* UPBPlayerNameTag::GetLocalCharacter() const
{
	return Cast<APBCharacter>(GetOwningPlayerPawn());
}

bool UPBPlayerNameTag::IsTheOwnerCharacterLocal()
{
	return GetLocalCharacter() == OwnerCharacter;
}

bool UPBPlayerNameTag::IsItSameTeamWithLocalAndOwnerCharacter()
{
	if (GetLocalCharacter() && OwnerCharacter)
	{
		return GetLocalCharacter()->GetTeamType() == OwnerCharacter->GetTeamType();
	}
	else
	{
		return false;
	}
}

int32 UPBPlayerNameTag::GetDistanceFromLocalPlayer()
{
	float dist = -1.0f;

	if (OwnerCharacter && GetOwningCharacter())
	{
		dist = FVector::Dist(OwnerCharacter->GetActorLocation(),GetOwningCharacter()->GetActorLocation());
		// divide by 100. because UE4 use Cm as a unit and I'm going to use meter as a unit
		dist *= 0.01f;
	}

	return (int32)dist;

}

float UPBPlayerNameTag::GetHealthPercentage() const
{
	if (OwnerCharacter)
	{
		return OwnerCharacter->GetHealthPercentage();
	}

	return 0.0f;
}

void UPBPlayerNameTag::UpdateVisiblity()
{
	// can not show invalid character's name
	if (false == IsSafe(OwnerCharacter))
	{
		Hide();
		return;
	}

	// if be aimed, then show  (third priority )
	if (OwnerCharacter->IsAimingTargeted())
	{
 		Show();
	}
	else
	{
		Hide();
	}

	// second priority
	if (IsItSameTeamWithLocalAndOwnerCharacter())
	{
		Show();
	}


	// (top priority)
	if (IsTheOwnerCharacterLocal())
	{
		Hide();
	}

	// (top priority)
	if (false == OwnerCharacter->IsAlive())
	{
		Hide();
	}

	

}

