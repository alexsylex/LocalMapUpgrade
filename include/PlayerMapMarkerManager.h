#pragma once

namespace LMU
{
	class PlayerMapMarkerManager
	{
	public:
		struct PlaceMarkerCallback : RE::IMessageBoxCallback
		{
			void Run(Message a_msg) final
			{

			}

			RE::NiPoint3 markerPos;
			RE::TESForm* worldOrCell;
		};

		static PlayerMapMarkerManager* GetSingleton()
		{
			static PlayerMapMarkerManager singleton;

			return &singleton;
		}

		void PlaceMarker(RE::LocalMapMenu* a_localMapMenu, float a_cursorPosX, float a_cursorPosY);
	};
}
