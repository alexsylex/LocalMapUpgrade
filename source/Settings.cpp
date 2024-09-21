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
				MakeSetting("bLocalMapColor:MapMenu", localMapColor),
				MakeSetting("bLocalMapFogOfWar:MapMenu", localMapFogOfWar)
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
			localMapColor = iniSettingCollection->GetSetting<bool>("bLocalMapColor:MapMenu");
			localMapFogOfWar = iniSettingCollection->GetSetting<bool>("bLocalMapFogOfWar:MapMenu");
		}
	}
}