// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class FirstPerson415 : ModuleRules
{
	public FirstPerson415(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput" });
	}
}
