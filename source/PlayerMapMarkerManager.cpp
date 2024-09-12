#include "PlayerMapMarkerManager.h"

#include "RE/L/LocalMapMenu.h"
#include "RE/N/NiPick.h"

namespace RE
{
	RefHandle AddPlayerMapMarker(BSTArray<MapMenuMarker>& a_mapMarkers)
	{
		RefHandle refHandle;
		static REL::Relocation<RefHandle& (*)(RefHandle&, BSTArray<MapMenuMarker>&)> func{ RELOCATION_ID(52186, 53078) };
		func(refHandle, a_mapMarkers);
		return refHandle;
	}

	void PlayerCharacter__SetPlayerMapMarker(PlayerCharacter* a_player, const NiPoint3& a_position, const TESForm* a_worldOrCell)
	{
		using func_t = decltype(&PlayerCharacter__SetPlayerMapMarker);
		static REL::Relocation<func_t> func{ RELOCATION_ID(39458, 40535) };
		return func(a_player, a_position, a_worldOrCell);
	}

	void PlayerCharacter__SetMarkerTeleportData(PlayerCharacter* a_player, TESObjectREFR* a_marker, PlayerCharacter::TeleportPath* a_teleportPath, bool a_ignoreLocks)
	{
		using func_t = decltype(&PlayerCharacter__SetMarkerTeleportData);
		static REL::Relocation<func_t> func{ RELOCATION_ID(39441, 40517) };
		return func(a_player, a_marker, a_teleportPath, a_ignoreLocks);
	}
}

namespace LMU
{
	bool GetRayCollisionPosition(const RE::NiPoint3& a_rayOrigin, const RE::NiPoint3& a_rayDir,
								 RE::NiPoint3& a_rayCollision)
	{
		bool hit = false;

		if (auto pick = RE::malloc<RE::NiPick>())
		{
			pick->Ctor();
			pick->observeCull = true;
			pick->type = RE::NiPick::PickType::kFindFirst;

			if (auto lodRoot = skyrim_cast<RE::NiNode*>(RE::ShadowSceneNode::GetMain()->GetChildren()[2].get()))
			{
				if (auto landLod = skyrim_cast<RE::NiNode*>(lodRoot->GetChildren()[0].get()))
				{
					pick->root = landLod->GetChildren()[0];
				}
			}

			hit = pick->PickObjects(a_rayOrigin, a_rayDir);
			if (hit)
			{
				a_rayCollision = pick->results[0]->intersect;
			}

			pick->Dtor();
			RE::free(pick);
		}

		return hit;
	}

	void PlayerMapMarkerManager::PlaceMarker(RE::LocalMapMenu* a_localMapMenu, float a_cursorPosX, float a_cursorPosY)
	{
		RE::NiCamera* localMapCamera = a_localMapMenu->localCullingProcess.GetLocalMapCamera()->camera.get();
		float localMapViewWidth = a_localMapMenu->bottomRight.x - a_localMapMenu->topLeft.x;
		float localMapViewHeight = a_localMapMenu->bottomRight.y - a_localMapMenu->topLeft.y;

		float cursorX = a_cursorPosX - a_localMapMenu->topLeft.x;
		float cursorY = a_cursorPosY - a_localMapMenu->topLeft.y;

		RE::NiPoint3 rayOrigin;
		RE::NiPoint3 rayDir;
		if (localMapCamera->WindowPointToRay(cursorX, cursorY, rayOrigin, rayDir, localMapViewWidth, localMapViewHeight))
		{
			RE::NiPoint3 markerPos;
			if (GetRayCollisionPosition(rayOrigin, rayDir, markerPos))
			{
				auto player = RE::PlayerCharacter::GetSingleton();
				RE::TES* tes = RE::TES::GetSingleton();

				if (tes->interiorCell)
				{
					RE::PlayerCharacter__SetPlayerMapMarker(player, markerPos, tes->interiorCell);
				}
				else
				{
					RE::PlayerCharacter__SetPlayerMapMarker(player, markerPos, player->GetWorldspace());
				}

				RE::TESObjectREFR* playerMapMarker = player->GetPlayerRuntimeData().playerMapMarker.get().get();

				RE::PlayerCharacter__SetMarkerTeleportData(player, playerMapMarker,
														   player->GetInfoRuntimeData().playerMarkerPath, true);

				AddPlayerMapMarker(a_localMapMenu->mapMarkers);

				auto ui = RE::UI::GetSingleton();
				if (ui->IsMenuOpen(RE::MapMenu::MENU_NAME))
				{
					auto mapMenu = ui->GetMenu<RE::MapMenu>();
					AddPlayerMapMarker(mapMenu->GetRuntimeData2()->mapMarkers);
				}
			}
		}
	}
}
