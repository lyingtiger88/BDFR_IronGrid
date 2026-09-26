using UnrealBuildTool;
using System.Collections.Generic;

public class BDFR_IronGridServerTarget : TargetRules
{
    public BDFR_IronGridServerTarget(TargetInfo Target) : base(Target)
    {
        Type = TargetType.Server;
        DefaultBuildSettings = BuildSettingsVersion.Latest;
        IncludeOrderVersion = EngineIncludeOrderVersion.Latest;
        ExtraModuleNames.Add("BDFR_IronGrid");
    }
}
