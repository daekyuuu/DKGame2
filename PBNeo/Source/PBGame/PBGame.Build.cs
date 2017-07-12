// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class PBGame : ModuleRules
{
	public PBGame(TargetInfo Target)
	{
		PrivateIncludePaths.AddRange(
			new string[] { 
				"PBGame/Classes/Player",
				"PBGame/Private",
                "PBGame/Private/SequencerWidget",
            }
		);

        PublicDependencyModuleNames.AddRange(
			new string[] {
				"Core",
				"CoreUObject",
				"Engine",
				"OnlineSubsystem",
				"OnlineSubsystemUtils",
				"AssetRegistry",
                "AIModule",
				"GameplayTasks",
                "LevelSequence",
                "MovieScene",
                "MovieSceneTracks",
                "Sockets",
                "Networking",
                "UMG",
                "EngineSettings",
            }
		);

        PrivateDependencyModuleNames.AddRange(
			new string[] {
				"InputCore",
				"Slate",
				"SlateCore",
                "MoviePlayer",
				"Json"
			}
		);

		DynamicallyLoadedModuleNames.AddRange(
			new string[] {
				"OnlineSubsystemNull",
				"NetworkReplayStreaming",
				"NullNetworkReplayStreaming",
				"HttpNetworkReplayStreaming"
			}
		);

		PrivateIncludePathModuleNames.AddRange(
			new string[] {
				"NetworkReplayStreaming"
			}
		);

        //if ((Target.Platform == UnrealTargetPlatform.Win32) || (Target.Platform == UnrealTargetPlatform.Win64) || (Target.Platform == UnrealTargetPlatform.Linux) || (Target.Platform == UnrealTargetPlatform.Mac))
        //{
        //    if (UEBuildConfiguration.bCompileSteamOSS == true)
        //    {
        //        DynamicallyLoadedModuleNames.Add("OnlineSubsystemSteam");
        //    }
        //}
        //else if (Target.Platform == UnrealTargetPlatform.PS4)
        //{
        //    DynamicallyLoadedModuleNames.Add("OnlineSubsystemPS4");
        //}
        //else if (Target.Platform == UnrealTargetPlatform.XboxOne)
        //{
        //    DynamicallyLoadedModuleNames.Add("OnlineSubsystemLive");
        //}
    }
}
