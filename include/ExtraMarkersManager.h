#pragma once

#include "Settings.h"

namespace LMU
{
	struct ExtraMarker
	{
		enum Type
		{
			kEnemy,
			kHostile,
			kGuard,
			kDead,
			kTeammate,
			kNeutral,
			kTotal
		};

		struct CreateData
		{
			enum
			{
				kName,
				kIconType,
				kCreateUndiscovered,
				kStride
			};
		};

		struct RefreshData
		{
			enum
			{
				kX,
				kY,
				kStride
			};
		};
	};

	class ExtraMarkersManager
	{
	public:
		static constexpr inline std::string_view extensionPath = "_level0.WorldMap.LocalMapMenu.IconDisplayExtension";
		static constexpr inline float feetToUnits = 21.3333333F;

		static void InitSingleton()
		{
			static ExtraMarkersManager instance;
			singleton = &instance;
		}

		static ExtraMarkersManager* GetSingleton() { return singleton; }

		void AddExtraMarkers(RE::LocalMapMenu& a_localMapMenu);

		void PostCreateMarkers(RE::GFxValue& a_iconDisplay);

		std::uint32_t GetAliveActorsDisplayRadius() const { return aliveActorsDisplayRadius / feetToUnits; }
		std::uint32_t GetUndeadActorsDisplayRadius() const { return undeadActorsDisplayRadius / feetToUnits; }
		std::uint32_t GetDeadActorsDisplayRadius() const { return deadActorsDisplayRadius / feetToUnits; }

		void SetAliveActorsDisplayRadius(std::uint32_t a_radius) { aliveActorsDisplayRadius = a_radius * feetToUnits; }
		void SetUndeadActorsDisplayRadius(std::uint32_t a_radius) { undeadActorsDisplayRadius = a_radius * feetToUnits; }
		void SetDeadActorsDisplayRadius(std::uint32_t a_radius) { deadActorsDisplayRadius = a_radius * feetToUnits; }

	private:
		static void AddExtraMarker(RE::ActorHandle& a_actorHandle, RE::Actor* actor, RE::BSTArray<RE::MapMenuMarker>& a_mapMarkers);

		static inline ExtraMarkersManager* singleton;

		std::uint32_t aliveActorsDisplayRadius = settings::mapmenu::localMapShowActorsOnlyWithDetectSpell ? 0 : std::numeric_limits<std::uint32_t>::max();
		std::uint32_t undeadActorsDisplayRadius = settings::mapmenu::localMapShowActorsOnlyWithDetectSpell ? 0 : std::numeric_limits<std::uint32_t>::max();
		std::uint32_t deadActorsDisplayRadius = settings::mapmenu::localMapShowActorsOnlyWithDetectSpell ? 0 : std::numeric_limits<std::uint32_t>::max();
	};
}