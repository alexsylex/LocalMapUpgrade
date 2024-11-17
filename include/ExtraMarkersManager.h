#pragma once

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

		static void InitSingleton()
		{
			static ExtraMarkersManager instance;
			singleton = &instance;
		}

		static ExtraMarkersManager* GetSingleton() { return singleton; }

		static void PostCreateMarkers(RE::GFxValue& a_iconDisplay);

		void ConfigureScaleformExtension();

		void AddExtraMarkers(RE::BSTArray<RE::MapMenuMarker>& a_mapMarkers);

	private:
		static void AddExtraMarker(RE::ActorHandle& a_actorHandle, const RE::NiPointer<RE::Actor>& actor,
								   RE::BSTArray<RE::MapMenuMarker>& a_mapMarkers);

		static inline ExtraMarkersManager* singleton;
	};
}