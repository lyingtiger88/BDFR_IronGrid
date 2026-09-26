using UnrealBuildTool;

public class BDFR_IronGrid : ModuleRules
{
    public BDFR_IronGrid(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new string[]
        {
            "Core",
            "CoreUObject",
            "Engine",
            "InputCore",
            "Paper2D"
        });
    }
}
