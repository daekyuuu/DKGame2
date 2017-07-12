// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


UENUM()
enum class EWeaponState : uint8
{
	Idle,
	FireReady,
	Firing,
	Reloading,
	Equipping,
	MeleeAttack,
	End,
};

UENUM(BlueprintType)
enum class EFireTrigger : uint8
{
	Fire1		UMETA(DisplayName = "FireTrigger1"),// FireŸ��1 (��: ���콺 ���ʹ�ư Ŭ��)		
	Fire2		UMETA(DisplayName = "FireTrigger2"),// FireŸ��2 (��: ���콺 �����ʹ�ư Ŭ��)
	Max
};

UENUM()
enum class EFirePoint : uint8
{
	Point_R,
	Point_L,
	Point_RL,
};

UENUM()
enum class EFireMode : uint8
{
	Single,					// ���ʸ�
	Dual_Alternating,		// ����
	Dual_Simultaneous,		// ����
};

//Use it as character 1P, 3P animation types.
UENUM(BlueprintType)
enum class EWeaponStance : uint8
{
	Rifle		UMETA(DisplayName = "Rifle"),
	Pistol		UMETA(DisplayName = "Pistol"),
	DualHand	UMETA(DisplayName = "DualHand"),
	DualGun		UMETA(DisplayName = "DualGun"),
};

UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	AR			UMETA(DisplayName = "Assult"),
	SMG			UMETA(DisplayName = "SMG"),
	SR			UMETA(DisplayName = "Sniper"),
	SG			UMETA(DisplayName = "Shotgun"),
	PS			UMETA(DisplayName = "Pistol"),
	LC			UMETA(DisplayName = "Launcher"),
	RPG			UMETA(DisplayName = "RPG"),
	KNIFE		UMETA(DisplayName = "Knife"),
	GRENADE		UMETA(DisplayName = "Grenade"),
	BOMB		UMETA(DisplayName = "Bomb"),
	DEFUSE		UMETA(DisplayName = "Defuse"),
	SPECIAL,
	Max, // meaning of maxNum or None
};

UENUM(BlueprintType)
enum class EWeaponPoint : uint8 // Weapon attach point
{
	Point_R			UMETA(DisplayName = "RHand"),
	Point_L			UMETA(DisplayName = "LHand"),
};

UENUM(BlueprintType)
enum class EWeaponSlot : uint8
{
	Primary,
	Secondary,
	Melee,
	Grenade,
	Special,
	ModeSpecial,
	Max, // meaning of maxNum or None
};

#define BombMissionWeaponSlot EWeaponSlot::ModeSpecial
#define BombMissionBombSlotIdx 0
#define BombMissionDefuseKitSlotIdx 1

UENUM(BlueprintType)
enum class EAttackDirType : uint8
{
	L_To_R		UMETA(DisplayName = "Left_To_Right"),
	R_To_L		UMETA(DisplayName = "Right_To_Left"),
	T_To_B		UMETA(DisplayName = "Top_To_Bottom"),
	B_To_T		UMETA(DisplayName = "Bottom_To_Top"),
	LT_To_RB	UMETA(DisplayName = "LeftTop_To_RightBottom"),
	LB_To_RT	UMETA(DisplayName = "LeftBottom_To_RightTop"),
	RT_To_LB	UMETA(DisplayName = "RightTop_To_LeftBottom"),
	RB_To_LT	UMETA(DisplayName = "RightBottom_To_LeftTop"),
	FORWARD		UMETA(DisplayName = "Forward"),
};

UENUM(BlueprintType)
enum class EWeaponSoundType : uint8
{
	Fire,
	FireLoop,
	FireFinish,
	OutofAmmo,
	Reload,
	Equip,
	MeleeAttack,
};

UENUM(BlueprintType)
enum class EWeaponAnimType : uint8
{
	//InGame
	Fire,
	FireZoom,
	Reload,
	Equip,
	MeleeAttack,

	//Lobby
	Lobby_Equip,
	Lobby_WalkingStart,
	Lobby_WalkingFinish,
};

//Function key �� ������ �� ���⿡ ��ȭ�� ����� Mode2 �� �ȴ�. (��: Dual Kriss �� ��� Mode2 ������)
UENUM(BlueprintType)
enum class EWeaponMode : uint8
{
	Mode1,
	Mode2,
};