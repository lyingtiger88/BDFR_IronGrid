using UnrealBuildTool;
using System.Collections.Generic;

public class BDFR_IronGridEditorTarget : TargetRules
{
    public BDFR_IronGridEditorTarget(TargetInfo Target) : base(Target)
    {
        Type = TargetType.Editor;
        DefaultBuildSettings = BuildSettingsVersion.Latest;
        IncludeOrderVersion = EngineIncludeOrderVersion.Latest;
        ExtraModuleNames.Add("BDFR_IronGrid");
    }
}
