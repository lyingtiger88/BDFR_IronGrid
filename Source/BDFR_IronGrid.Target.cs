using UnrealBuildTool;
using System.Collections.Generic;

public class BDFR_IronGridTarget : TargetRules
{
    public BDFR_IronGridTarget(TargetInfo Target) : base(Target)
    {
        Type = TargetType.Game;
        DefaultBuildSettings = BuildSettingsVersion.Latest;
        IncludeOrderVersion = EngineIncludeOrderVersion.Latest;
        ExtraModuleNames.Add("BDFR_IronGrid");
    }
}
