using UnrealBuildTool;
using System.IO;

public class BDFR_IronGrid : ModuleRules
{
    public BDFR_IronGrid(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        // Current source layout keeps headers in module subfolders such as
        // Tank/, Player/, UI/, and Game/. Add the module root explicitly so
        // includes like "Tank/IronGridTankPawn.h" resolve reliably in UE 5.8.
        PrivateIncludePaths.Add(ModuleDirectory);

        PublicDependencyModuleNames.AddRange(new string[]
        {
            "Core",
            "CoreUObject",
            "Engine",
            "InputCore",
            "Paper2D",
            "Slate",
            "SlateCore"
        });
    }
}
