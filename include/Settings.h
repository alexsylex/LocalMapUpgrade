#pragma once

namespace SKSE::log
{
	using level = spdlog::level::level_enum;
}
namespace logger = SKSE::log;

namespace settings
{
	void Init(const std::string& a_iniFileName);

	// Default values

	namespace debug
	{
		inline logger::level logLevel = logger::level::debug;
	}

	namespace mapmenu
	{
		inline bool localMapColor = true;
		inline bool localMapFogOfWar = true;
		inline float localMapKeyboardPanSpeed = 60.0F;
		inline bool localMapShowEnemyActors = true;
		inline bool localMapShowGuardActors = true;
		inline bool localMapShowHostileActors = true;
		inline bool localMapShowDeadActors = true;
	}
}