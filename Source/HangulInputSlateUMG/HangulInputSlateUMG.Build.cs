using UnrealBuildTool;

public class HangulInputSlateUMG : ModuleRules
{
	public HangulInputSlateUMG(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

		// UMG wrapper layer: UWidget subclasses require UObject/Engine/UMG.
		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core",
			"CoreUObject",
			"Engine",
			"UMG",
			"Slate",
			"SlateCore",
			"HangulInputSlate",
		});
	}
}
