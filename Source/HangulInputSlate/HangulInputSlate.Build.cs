using UnrealBuildTool;

public class HangulInputSlate : ModuleRules
{
	public HangulInputSlate(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

		// Pure Slate module: no CoreUObject/Engine dependency so it stays usable in
		// Slate-only contexts (editor tooling, the UnrealClaude plugin, etc.).
		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core",
			"InputCore",
			"Slate",
			"SlateCore",
			"ApplicationCore",
		});

		// Windows-only OS IME detach (ImmAssociateContextEx) lives in this module.
		if (Target.Platform == UnrealTargetPlatform.Win64)
		{
			PublicSystemLibraries.Add("imm32.lib");
		}
	}
}
