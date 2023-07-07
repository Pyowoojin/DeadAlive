// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class DeadAlive : ModuleRules
{
	public DeadAlive(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay", "EnhancedInput", "Niagara", "UMG", "SlateCore", "AIModule", "NavigationSystem" });
	}
}
