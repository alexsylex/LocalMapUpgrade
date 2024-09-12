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

		static bool GetRayCollisionPosition(const RE::NiPoint3& a_rayOrigin, const RE::NiPoint3& a_rayDir,
											RE::NiPoint3& a_rayCollision);
	};
}
