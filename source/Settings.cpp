#include "Settings.h"

#include "utils/INISettingCollection.h"

namespace settings
{
	using namespace utils;

	void Init(const std::string& a_iniFileName)
	{
		INISettingCollection* iniSettingCollection = INISettingCollection::GetSingleton();

		{
			using namespace debug;
			iniSettingCollection->AddSettings
			(
				MakeSetting("uLogLevel:Debug", static_cast<std::uint32_t>(logLevel))
			);
		}

		{
			using namespace mapmenu;
			iniSettingCollection->AddSettings
			(
				MakeSetting("bMapLocalColor:MapMenu", localMapColor),
				MakeSetting("bMapLocalFogOfWar:MapMenu", localMapFogOfWar),
				MakeSetting("fMapLocalKeyboardPanSpeed:MapMenu", localMapKeyboardPanSpeed),
				MakeSetting("bMapLocalShowEnemyActors:MapMenu", localMapShowEnemyActors),
				MakeSetting("bMapLocalShowHostileActors:MapMenu", localMapShowHostileActors),
				MakeSetting("bMapLocalShowGuardActors:MapMenu", localMapShowGuardActors),
				MakeSetting("bMapLocalShowDeadActors:MapMenu", localMapShowDeadActors),
				MakeSetting("bImmersiveMode:MapMenu", localMapShowActorsOnlyWithDetectSpell)
			);
		}

		if (!iniSettingCollection->ReadFromFile(a_iniFileName))
		{
			logger::warn("Could not read {}, falling back to default options", a_iniFileName);
		}

		{
			using namespace debug;
			logLevel = static_cast<logger::level>(iniSettingCollection->GetSetting<std::uint32_t>("uLogLevel:Debug"));
		}

		{
			using namespace mapmenu;
			localMapColor = iniSettingCollection->GetSetting<bool>("bMapLocalColor:MapMenu");
			localMapFogOfWar = iniSettingCollection->GetSetting<bool>("bMapLocalFogOfWar:MapMenu");
			localMapKeyboardPanSpeed = iniSettingCollection->GetSetting<float>("fMapLocalKeyboardPanSpeed:MapMenu");
			localMapShowEnemyActors = iniSettingCollection->GetSetting<bool>("bMapLocalShowEnemyActors:MapMenu");
			localMapShowHostileActors = iniSettingCollection->GetSetting<bool>("bMapLocalShowHostileActors:MapMenu");
			localMapShowGuardActors = iniSettingCollection->GetSetting<bool>("bMapLocalShowGuardActors:MapMenu");
			localMapShowDeadActors = iniSettingCollection->GetSetting<bool>("bMapLocalShowDeadActors:MapMenu");
			localMapShowActorsOnlyWithDetectSpell = iniSettingCollection->GetSetting<bool>("bImmersiveMode:MapMenu");
		}
	}
}