#include "PlayerMapMarkerManager.h"

#include "RE/L/LocalMapMenu.h"
#include "RE/N/NiPick.h"

#undef MessageBox

namespace RE
{
	void PlayerCharacter__SetMarkerTeleportData(PlayerCharacter* a_player, TESObjectREFR* a_marker, PlayerCharacter::TeleportPath* a_teleportPath, bool a_ignoreLocks)
	{
		using func_t = decltype(&PlayerCharacter__SetMarkerTeleportData);
		static REL::Relocation<func_t> func{ RELOCATION_ID(39441, 40517) };
		return func(a_player, a_marker, a_teleportPath, a_ignoreLocks);
	}

	void PlayerCharacter__SetPlayerMapMarker(PlayerCharacter* a_player, const NiPoint3& a_position, const TESForm* a_worldOrCell)
	{
		using func_t = decltype(&PlayerCharacter__SetPlayerMapMarker);
		static REL::Relocation<func_t> func{ RELOCATION_ID(39458, 40535) };
		return func(a_player, a_position, a_worldOrCell);
	}

	void PlayerCharacter__RemovePlayerMapMarker(PlayerCharacter* a_player)
	{
		using func_t = decltype(&PlayerCharacter__RemovePlayerMapMarker);
		static REL::Relocation<func_t> func{ RELOCATION_ID(39459, 40536) };
		return func(a_player);
	}

	std::uint32_t UI__OpenMessageBox(const BSString& a_title, const BSTSmartPointer<IMessageBoxCallback>& a_callback,
									 std::uint8_t a_arg3, std::uint32_t a_arg4, std::int32_t a_arg5,
									 const BSTArray<BSString>& a_options)
	{
		using func_t = decltype(&UI__OpenMessageBox);
		static REL::Relocation<func_t> func{ RELOCATION_ID(51421, REL::Module::get().version() == SKSE::RUNTIME_SSE_1_6_640 ? 52270 : 442726) };
		return func(a_title, a_callback, a_arg3, a_arg4, a_arg5, a_options);
	}

	RefHandle AddPlayerMapMarkerToMap(BSTArray<MapMenuMarker>& a_mapMarkers)
	{
		using func_t = RefHandle& (*)(RefHandle&, BSTArray<MapMenuMarker>&);
		static REL::Relocation<func_t> func{ RELOCATION_ID(52186, 53078) };
		RefHandle refHandle;
		func(refHandle, a_mapMarkers);
		return refHandle;
	}
}

namespace LMU
{
	bool PickObjectInNode(RE::NiPick* a_pick, RE::NiNode* a_node, const RE::NiPoint3& a_rayOrigin, const RE::NiPoint3& a_rayDir)
	{
		a_pick->root = (RE::NiPointer<RE::NiAVObject>)a_node;

		if (a_pick->PickObjects(a_rayOrigin, a_rayDir))
		{
			return true;
		}

		for (auto& child : a_node->GetChildren()) if (child)
		{
			if (auto node = child->AsNode())
			{
				if (PickObjectInNode(a_pick, node, a_rayOrigin, a_rayDir))
				{
					return true;
				}
			}
		}

		return false;
	}

	bool PickObjectInCell(RE::NiPick* a_pick, RE::TESObjectCELL* a_cell, const RE::NiPoint3& a_rayOrigin, const RE::NiPoint3& a_rayDir)
	{
		if (a_cell && a_cell->IsAttached())
		{
			RE::TESWorldSpace* worldSpace = a_cell->GetRuntimeData().worldSpace;

			bool hasWorldSpaceFixedDimensions = worldSpace && worldSpace->flags.any(RE::TESWorldSpace::Flag::kFixedDimensions);

			RE::NiPointer<RE::NiNode> cell3D = a_cell->GetRuntimeData().loadedData->cell3D;

			if (cell3D)
			{
				if (PickObjectInNode(a_pick, cell3D.get(), a_rayOrigin, a_rayDir))
				{
					return true;
				}
			}
		}

		return false;
	}

	bool GetRayCollisionPosition(const RE::NiPoint3& a_rayOrigin, const RE::NiPoint3& a_rayDir,
		RE::NiPoint3& a_rayCollision)
	{
		bool hit = false;

		if (auto pick = RE::malloc<RE::NiPick>())
		{
			pick->Ctor();
			pick->observeCull = true;
			pick->type = RE::NiPick::PickType::kFindFirst;

			RE::TES* tes = RE::TES::GetSingleton();
			RE::TESWorldSpace* worldSpace = tes->GetRuntimeData2().worldSpace;

			if (RE::TESObjectCELL* interiorCell = tes->interiorCell)
			{
				if (PickObjectInCell(pick, interiorCell, a_rayOrigin, a_rayDir))
				{
					a_rayCollision = pick->results[0]->intersect;
					hit = true;
				}
			}
			else if (worldSpace)
			{
				RE::TESObjectCELL* skyCell = worldSpace->GetSkyCell();
				if (skyCell && skyCell->IsAttached())
				{
					if (PickObjectInCell(pick, skyCell, a_rayOrigin, a_rayDir))
					{
						a_rayCollision = pick->results[0]->intersect;
						hit = true;
					}
				}

				if (!hit)
				{
					for (int gridCellX = 0; gridCellX < tes->gridCells->length; gridCellX++)
					{
						for (int gridCellY = 0; gridCellY < tes->gridCells->length; gridCellY++)
						{
							RE::TESObjectCELL* cell = tes->gridCells->GetCell(gridCellX, gridCellY);
							if (cell && cell->IsAttached())
							{
								if (PickObjectInCell(pick, cell, a_rayOrigin, a_rayDir))
								{
									a_rayCollision = pick->results[0]->intersect;
									hit = true;
								}
							}
						}
					}
				}
			}

			if (!hit)
			{
				if (tes->lodLandRoot)
				{
					RE::NiPointer<RE::NiAVObject> landRoot = tes->lodLandRoot->GetChildren()[0];
					pick->root = landRoot;
					if (pick->PickObjects(a_rayOrigin, a_rayDir))
					{
						a_rayCollision = pick->results[0]->intersect;
						hit = true;
					}
				}
			}

			pick->Dtor();
			RE::free(pick);
		}

		return hit;
	}

	bool GetRayCollisionPosition2(const RE::NiPoint3& a_rayOrigin, const RE::NiPoint3& a_rayDir,
								 RE::NiPoint3& a_rayCollision)
	{
		bool hit = false;

		if (auto pick = RE::malloc<RE::NiPick>())
		{
			pick->Ctor();
			pick->observeCull = true;
			pick->type = RE::NiPick::PickType::kFindFirst;

			RE::NiTObjectArray<RE::NiPointer<RE::NiAVObject>>& shadowScenes = RE::ShadowSceneNode::GetMain()->GetChildren();

			if (!shadowScenes.empty())
			{
				for (std::uint32_t i = 0; i < shadowScenes.size(); i++)
				{
					if (auto lodRoot = skyrim_cast<RE::NiNode*>(shadowScenes[i].get()))
					{
						RE::NiTObjectArray<RE::NiPointer<RE::NiAVObject>>& lods = lodRoot->GetChildren();
						if (!lods.empty())
						{
							pick->root = lods[0];
						}
					}

					if (pick->PickObjects(a_rayOrigin, a_rayDir))
					{
						a_rayCollision = pick->results[0]->intersect;
						hit = true;
					}
				}
			}

			pick->Dtor();
			RE::free(pick);
		}

		return hit;
	}

	void PlaceMarkerImmediate(RE::LocalMapMenu* a_localMapMenu, float a_cursorPosX, float a_cursorPosY)
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

				AddPlayerMapMarkerToMap(a_localMapMenu->mapMarkers);

				// Only if the map menu is open. Make sure because someone maybe makes a minimap mod.
				auto ui = RE::UI::GetSingleton();
				if (ui->IsMenuOpen(RE::MapMenu::MENU_NAME))
				{
					auto mapMenu = ui->GetMenu<RE::MapMenu>();
					AddPlayerMapMarkerToMap(mapMenu->GetRuntimeData2()->mapMarkers);
				}
			}
		}
	}

	void PlayerMapMarkerManager::MessageBox::Callback::Run(Message a_optionIndex)
	{
		auto player = RE::PlayerCharacter::GetSingleton();

		switch (static_cast<std::uint32_t>(a_optionIndex))
		{
		case 0: // Move
			PlaceMarkerImmediate(localMapMenu, cursorPosX, cursorPosY);
			break;
		case 2: // Remove
			RE::PlayerCharacter__RemovePlayerMapMarker(player);
			break;
		default: // Leave
			break;
		}
	}

	void PlayerMapMarkerManager::PlaceMarker(RE::LocalMapMenu* a_localMapMenu, float a_cursorPosX, float a_cursorPosY)
	{
		allowPlaceMarker = false;

		if (RE::PlayerCharacter::GetSingleton()->GetPlayerRuntimeData().playerMapMarker)
		{
			messageBox.callback->SetData(a_localMapMenu, a_cursorPosX, a_cursorPosY);

			RE::UI__OpenMessageBox(messageBox.title, messageBox.callback, 0, 25, 4, messageBox.options);
		}
		else
		{
			PlaceMarkerImmediate(a_localMapMenu, a_cursorPosX, a_cursorPosY);
		}
	}
}
