#pragma once

UENUM(BlueprintType)
enum class EPBMovementType : uint8
{
	Standing	UMETA(DisplayName = "Standing"),
	Walking		UMETA(DisplayName = "Walking"),
	Running		UMETA(DisplayName = "Running"),
	Sprinting	UMETA(DisplayName = "Sprinting"),
	Max
};

UENUM(BlueprintType)
enum class EPBVoiceType : uint8
{
	None = 0,
	Damage,				// 대미지 입었을 때, anim notify 에서 처리함
	Death,				// 죽을 때 비명
	ThrowGrenade,		// Fire in the hole~
	// kill mark voices
	SpecialGunner,
	Assassin,
	BombShot,
	GasAttack,
	OneShotOnKill,
	ChainSlugger,
	PiercingShot,
	MassKill,
	ChainStopper,
	ChainKiller,
	ChainHeadShot,
	HeadShot,
	HotKiller,
	Nightmare,
	MeatChopper,
	Payback,
	Avenger,
	Stalker,
	LoveShot,
	DoubleKill,
	TripleKill,
	Max,
};

UENUM(BlueprintType)
enum class EPBFootstepType : uint8
{
	Step,
	Jump,
	Land,
};

UENUM(BlueprintType)
enum class EPBCharacterGender : uint8
{
	Male,
	Female
};

//
//UENUM(BlueprintType)
//enum class PBMoveDirectionType : uint8
//{
//	FORWARD = 0,
//	RIGHT45,
//	RIGHT90,
//	RIGHT135,
//	BACKWARD,
//	LEFT135,
//	LEFT90,
//	LEFT45,
//	Max
//};