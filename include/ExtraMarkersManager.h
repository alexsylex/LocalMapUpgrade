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

		static void PostCreateMarkers(RE::GFxValue& a_iconDisplay) { singleton->PostCreateMarkersImpl(a_iconDisplay); }

		void AddExtraMarkers(RE::BSTArray<RE::MapMenuMarker>& a_mapMarkers);

		void SetAliveActorsDisplayRadius(std::uint32_t a_radius) { aliveActorsDisplayRadius = a_radius * feetToUnits; }
		void SetDeadActorsDisplayRadius(std::uint32_t a_radius) { deadActorsDisplayRadius = a_radius * feetToUnits; }

	private:
		static void AddExtraMarker(RE::ActorHandle& a_actorHandle, const RE::NiPointer<RE::Actor>& actor,
								   RE::BSTArray<RE::MapMenuMarker>& a_mapMarkers);

		void PostCreateMarkersImpl(RE::GFxValue& a_iconDisplay);

		static inline ExtraMarkersManager* singleton;

		std::uint32_t aliveActorsDisplayRadius = settings::mapmenu::localMapShowActorsOnlyWithDetectSpell ? 0 : std::numeric_limits<std::uint32_t>::max();
		std::uint32_t deadActorsDisplayRadius = settings::mapmenu::localMapShowActorsOnlyWithDetectSpell ? 0 : std::numeric_limits<std::uint32_t>::max();
	};
}