// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

[SupportedPlatforms(UnrealPlatformClass.Server)]
public class PBNeoServerTarget : TargetRules
{
	public PBNeoServerTarget(TargetInfo Target)
	{
        Type = TargetType.Server;
    }

    //
    // TargetRules interface.
    //

    public override void SetupBinaries(
        TargetInfo Target,
        ref List<UEBuildBinaryConfiguration> OutBuildBinaryConfigurations,
        ref List<string> OutExtraModuleNames)
    {
        OutExtraModuleNames.Add("PBGame");
    }
}
