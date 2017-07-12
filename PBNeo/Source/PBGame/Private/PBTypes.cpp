#include "PBGame.h"
#include "PBTypes.h"


FString GetTeamShortName(EPBTeamType eTeamType)
{
	switch (eTeamType)
	{
	case EPBTeamType::Alpha: return TEXT("A");
	case EPBTeamType::Bravo: return TEXT("B");
	}
	return TEXT("TeamShortName");
}

FString GetNetRoleString(ENetRole Role)
{
	switch (Role)
	{
	case ENetRole::ROLE_Authority:
		return FString("ROLE_Authority");

	case ENetRole::ROLE_AutonomousProxy:
		return FString("ROLE_AutonomousProxy");

	case ENetRole::ROLE_SimulatedProxy:
		return FString("ROLE_SimulatedProxy");

	case ENetRole::ROLE_None:
		return FString("ROLE_None");

	default:
		return FString("Unknown");
	}
}

FString GetNetModeString(ENetMode Mode)
{
	switch (Mode)
	{
	case ENetMode::NM_Client:			return FString("NM_Client");
	case ENetMode::NM_DedicatedServer:	return FString("NM_DedicatedServer");
	case ENetMode::NM_ListenServer:		return FString("NM_ListenServer");
	case ENetMode::NM_Standalone:		return FString("NM_Standalone");

	default:
		return FString("Unknown");
	}
}

extern FString GetGameModeString(EPBGameModeType Mode)
{
	switch (Mode)
	{
	case EPBGameModeType::TDM: 
		return FString("TDM");

	case EPBGameModeType::BOMB: 
		return FString("BOMB");

	default:
		return FString("Unknown");
	}
}
