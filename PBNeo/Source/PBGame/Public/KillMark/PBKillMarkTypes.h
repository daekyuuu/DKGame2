
#pragma once

/*
UENUM(BlueprintType)
enum class EKillMarkTypes : uint8
{
	None = 0,
	// rare
	SpecialGunner,
	Assassin,
	BombShot,
	GasAttack,
	OneShotOneKill,
	// special
	ChainSlugger,
	PiercingShot,
	MassKill,
	ChainStopper,
	ChainKiller,
	ChainHeadShot,
	HeadShot,
	// Revenge
	HotKiller,
	Nightmare,
	MeatChopper,
	Stalker,
	Avenger,
	Payback,
	LoveShot,
	// Normal
	TripleKill,
	DoubleKill,
	Max,

};
*/

UENUM(BlueprintType)
enum class EKillMarkCategory : uint8
{
	None = 0,
	Rare,
	Special,
	Revenge,
	Normal,

};