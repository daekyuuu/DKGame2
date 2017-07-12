// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Engine.h"
#include "SlateBasics.h"
#include "SlateExtras.h"
#include "ParticleDefinitions.h"
#include "SoundDefinitions.h"
#include "Net/UnrealNetwork.h"
#include "Mode/PBGameMode.h"
#include "Mode/PBGameState.h"
#include "Player/PBCharacter.h"
#include "Player/PBCharacterMovement.h"
#include "Player/PBPlayerController.h"
#include "PBGameClasses.h"
#include "PBGameplayStatics.h"
#include "Runtime/Engine/Classes/Animation/AnimInstance.h"

class UBehaviorTreeComponent;

DECLARE_LOG_CATEGORY_EXTERN(LogPB, Log, All);
DECLARE_LOG_CATEGORY_EXTERN(LogUI, Log, All);
DECLARE_LOG_CATEGORY_EXTERN(LogPBWeapon, Log, All);
DECLARE_LOG_CATEGORY_EXTERN(LogCharacter, Log, All);
DECLARE_LOG_CATEGORY_EXTERN(LogWeaponProj, Log, All);
DECLARE_LOG_CATEGORY_EXTERN(LogRoomSlot, Log, All);
DECLARE_LOG_CATEGORY_EXTERN(LogSet, Log, All);

// Utility for visual logging - remove after development
#define print(text) if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 9, FColor::White,text)

static FORCEINLINE void PrintOnScreen(const FString& DebugMessage, float TimeToDisplay = 3.0f, FColor DisplayColor = FColor::White)
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, TimeToDisplay, DisplayColor, DebugMessage);
	}		
}

#define IsSafe(Target) ((Target) && (Target)->IsValidLowLevel())

#define IntToText(num) (FText::FromString(FString::FromInt(num)))
#define NameToInt(Name) (FCString::Atoi(*(Name.ToString())))

// Useful Compiler Macro
#define PB_CUR_CLASS_FUNC (FString(__FUNCTION__))
#define PB_CUR_CLASS (FString(__FUNCTION__).Left(FString(__FUNCTION__).Find(TEXT(":"))) )
#define PB_CUR_FUNC (FString(__FUNCTION__).Right(FString(__FUNCTION__).Len() - FString(__FUNCTION__).Find(TEXT("::")) - 2 ))
#define PB_CUR_LINE (PB_CUR_CLASS + "(" + FString::FromInt(__LINE__) + ")")



/**
* Enum to string template function
*/
template<typename TEnum>
static FORCEINLINE FString GetEnumValueToString(const FString& Name, TEnum Value)
{
	const UEnum* enumPtr = FindObject<UEnum>(ANY_PACKAGE, *Name, true);
	if (!enumPtr)
	{
		return FString("Invalid");
	}

	return enumPtr->GetEnumName((int32)Value);
}

/**
* Enum from string template function
*/
template <typename EnumType>
static FORCEINLINE EnumType GetEnumValueFromString(const FString& EnumName, const FString& String)
{
	UEnum* Enum = FindObject<UEnum>(ANY_PACKAGE, *EnumName, true);
	if (!Enum)
	{
		return EnumType(0);
	}
	return (EnumType)Enum->FindEnumIndex(FName(*String));
}

/**
* Enum to string Macro.
* Example) FString str = EnumToString(EWeaponType, EWeaponType::AR);
* WARNING: In the code which get the string from UE4 Enum class, they use Index of array. So if you don't use sequence numbers as a enum value, then this macro would not properly work.  
*/
#define EnumToString(EnumName, Value)\
	GetEnumValueToString<EnumName>(#EnumName, Value)\

/**
* Enum to FName Macro.
* Example) FName name = EnumToName(EWeaponType, EWeaponType::AR);
*/
#define EnumToName(EnumName, Value)\
	FName(*GetEnumValueToString<EnumName>(#EnumName, Value))\

/**
* Enum From string Macro.
* * Example) EWeaponType type = EnumFromString(EWeaponType, "AR");
*/
#define EnumFromString(EnumName, Str)\
	GetEnumValueFromString<EnumName>(#EnumName, Str)\

// Macro about Widget Event Binding.
// Note: You only can bind one function by one object.
// If you need to bind many functions in one object, just make lapping function and insert your functions in there. ex) OnNotifyXXX(){ DoA(); DoB(); DoC(); ...)
#define BindWidgetEvent(WidgetEvent, FuncName) \
	if (GetMessageDispatcher().IsValid()) \
		{	\
		if (false == GetMessageDispatcher()->WidgetEvent.IsBoundToObject(this))	\
				{	\
			GetMessageDispatcher()->WidgetEvent.AddUObject(this, &FuncName);	\
				}	\
		}\

#define UnbindWidgetEvent(WidgetEvent) \
if (GetMessageDispatcher().IsValid()) \
{ \
	GetMessageDispatcher()->WidgetEvent.RemoveAll(this);\
}\

/** when you modify this, please note that this information can be saved with instances
 * also DefaultEngine.ini [/Script/Engine.CollisionProfile] should match with this list **/
#define COLLISION_WEAPON		ECC_GameTraceChannel1
#define COLLISION_PROJECTILE	ECC_GameTraceChannel2
#define COLLISION_PICKUP		ECC_GameTraceChannel3
#define COLLISION_RAGDOLL		ECC_GameTraceChannel4
#define COLLISION_WEAPON_PIERCE ECC_GameTraceChannel5

#define MAX_PLAYER_NAME_LENGTH 16

/** Set to 1 to pretend we're building for console even on a PC, for testing purposes */
#define PB_SIMULATE_CONSOLE_UI	0

#if PLATFORM_PS4 || PLATFORM_XBOXONE || PB_SIMULATE_CONSOLE_UI
	#define PB_CONSOLE_UI 1
#else
	#define PB_CONSOLE_UI 0
#endif

#define PB_BOT_ENABLED 0

#define PB_LOADOUT_OLD