// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class KH_NBC09 : ModuleRules
{
	public KH_NBC09(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] 
		{ 
			"Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput" ,

            "UMG", "Slate", "SlateCore",
        });

		PrivateDependencyModuleNames.AddRange(new string[] {  });

        PublicIncludePaths.AddRange(new string[] { "KH_NBC09" });
    }
}
