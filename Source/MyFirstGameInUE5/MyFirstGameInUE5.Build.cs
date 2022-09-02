// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class MyFirstGameInUE5 : ModuleRules
{
	public MyFirstGameInUE5(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] {
			"Core"
			, "CoreUObject"
			, "Engine"
			, "InputCore"
			, "Niagara"
			, "PhysicsCore"
		});

		PrivateDependencyModuleNames.AddRange(new string[] {  });

		PublicIncludePaths.AddRange( new string[] {
			"MyFirstGameInUE5"
			, "MyFirstGameInUE5/Weapons/Components"
			, "MyFirstGameInUE5/Weapons/Actors"
			, "MyFirstGameInUE5/Global"
			, "MyFirstGameInUE5/Global/Utilities"
			, "MyFirstGameInUE5/Global/Utilities/Components"
			, "MyFirstGameInUE5/Characters"
			, "MyFirstGameInUE5/Interaction"
		});

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
