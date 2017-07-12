// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class PBNeoTarget : TargetRules
{
    public PBNeoTarget(TargetInfo Target)
    {
        Type = TargetType.Game;
        //bUsesSteam = true;
    }

    //
    // TargetRules interface.
    //

    public override void SetupBinaries(
        TargetInfo Target,
        ref List<UEBuildBinaryConfiguration> OutBuildBinaryConfigurations,
        ref List<string> OutExtraModuleNames
        )
    {
        OutExtraModuleNames.Add("PBGame");
    }

    public override void SetupGlobalEnvironment(
            TargetInfo Target,
            ref LinkEnvironmentConfiguration OutLinkEnvironmentConfiguration,
            ref CPPEnvironmentConfiguration OutCPPEnvironmentConfiguration
            )
    {
        if (Target.Platform == UnrealTargetPlatform.PS4)
        {
            OutCPPEnvironmentConfiguration.Definitions.Add("GARLIC_HEAP_SIZE=(2600ULL * 1024 * 1024)");
            OutCPPEnvironmentConfiguration.Definitions.Add("ONION_HEAP_SIZE=(200ULL * 1024 * 1024)");
            OutCPPEnvironmentConfiguration.Definitions.Add("RESERVED_MEMORY_SIZE=(1ULL * 1024 * 1024)");
            OutCPPEnvironmentConfiguration.Definitions.Add("LIBC_MALLOC_SIZE=(32ULL * 1024 * 1024)");
            OutCPPEnvironmentConfiguration.Definitions.Add("LIBC_MALLOC_SIZE=(32ULL * 1024 * 1024)");

            //for a real game these could be behind a call to a virtual function defined in a partial class in a protected folder also.
            OutCPPEnvironmentConfiguration.Definitions.Add("UE4_PROJECT_NPTITLEID=NPXX52399_00");
            OutCPPEnvironmentConfiguration.Definitions.Add("UE4_PROJECT_NPTITLESECRET=4fa07e85d0612d46a795593cc340224dbd343245f647d2d00e5922c149ba68e5624e4b37afe3fd1c0d9e8d878faa3f91b60910e492c801a7065b79359147fecc7cdf12723c3d3a582c4f20981c756f261e1bdcd8d58ef572a247b0e47a31a1116fbf5623d38f98faaf7529bb56ef24b98af450620c1c2436fecf2de8444b13a9");
        }
    }
}
