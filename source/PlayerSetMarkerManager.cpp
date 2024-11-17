#include "PlayerSetMarkerManager.h"

#include "RE/L/LocalMapMenu.h"
#include "RE/N/NiPick.h"

#undef MessageBox

namespace RE
{
	void PlayerCharacter__SetMarkerTeleportData(PlayerCharacter* a_player, TESObjectREFR* a_marker, PlayerCharacter::TeleportPath* a_teleportPath, bool a_ignoreLocks)
	{
		using func_t = decltype(&PlayerCharacter__SetMarkerTeleportData);
		static REL::Relocation<func_t> func{ REL::VariantID(39441, 40517, 0x6C4D30) };
		return func(a_player, a_marker, a_teleportPath, a_ignoreLocks);
	}

	void PlayerCharacter__SetPlayerMapMarker(PlayerCharacter* a_player, const NiPoint3& a_position, const TESForm* a_worldOrCell)
	{
		using func_t = decltype(&PlayerCharacter__SetPlayerMapMarker);
		static REL::Relocation<func_t> func{ REL::VariantID(39458, 40535, 0x6C74D0) };
		return func(a_player, a_position, a_worldOrCell);
	}

	void PlayerCharacter__RemovePlayerMapMarker(PlayerCharacter* a_player)
	{
		using func_t = decltype(&PlayerCharacter__RemovePlayerMapMarker);
		static REL::Relocation<func_t> func{ REL::VariantID(39459, 40536, 0x6C7630) };
		return func(a_player);
	}

	std::uint32_t UI__OpenMessageBox(const BSString& a_title, const BSTSmartPointer<IMessageBoxCallback>& a_callback,
									 std::uint8_t a_arg3, std::uint32_t a_arg4, std::int32_t a_arg5,
									 const BSTArray<BSString>& a_options)
	{
		using func_t = decltype(&UI__OpenMessageBox);
		static REL::Relocation<func_t> func{ REL::VariantID(51421, REL::Module::get().version() < SKSE::RUNTIME_SSE_1_6_1130 ? 52270 : 442726, 0x8D82D0) };
		return func(a_title, a_callback, a_arg3, a_arg4, a_arg5, a_options);
	}

	RefHandle AddPlayerMapMarkerToMap(BSTArray<MapMenuMarker>& a_mapMarkers)
	{
		using func_t = RefHandle& (*)(RefHandle&, BSTArray<MapMenuMarker>&);
		static REL::Relocation<func_t> func{ REL::VariantID(52186, 53078, 0x913AB0) };
		RefHandle refHandle;
		func(refHandle, a_mapMarkers);
		return refHandle;
	}

	void MapMenu__SetMarkers(MapMenu* a_mapMenu)
	{
		using func_t = decltype(&MapMenu__SetMarkers);
		static REL::Relocation<func_t> func{ REL::VariantID(51421, 53108, 0x9184C0) };
		return func(a_mapMenu);
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

		for (RE::NiPointer<RE::NiAVObject>& child : a_node->GetChildren()) if (child)
		{
			if (RE::NiNode* node = child->AsNode())
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

	void PlaceMarkerImmediate(RE::LocalMapMenu* a_localMapMenu, float a_wndPointX, float a_wndPointY)
	{
		RE::NiCamera* localMapCamera = a_localMapMenu->localCullingProcess.GetLocalMapCamera()->camera.get();
		float localMapViewWidth = a_localMapMenu->bottomRight.x - a_localMapMenu->topLeft.x;
		float localMapViewHeight = a_localMapMenu->bottomRight.y - a_localMapMenu->topLeft.y;

		float wndPointX = a_wndPointX - a_localMapMenu->topLeft.x;
		float wndPointY = a_wndPointY - a_localMapMenu->topLeft.y;

		RE::NiPoint3 rayOrigin;
		RE::NiPoint3 rayDir;
		if (localMapCamera->WindowPointToRay(wndPointX, wndPointY, rayOrigin, rayDir, localMapViewWidth, localMapViewHeight))
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

				RE::ObjectRefHandle playerMapMarker = REL::Module::IsVR() ? player->GetVRInfoRuntimeData()->playerMapMarker : player->GetInfoRuntimeData().playerMapMarker;
				RE::PlayerCharacter::TeleportPath* playerMarkerTeleportPath = REL::Module::IsVR() ? player->GetVRInfoRuntimeData()->playerMarkerPath : player->GetInfoRuntimeData().playerMarkerPath;

				if (playerMapMarker && playerMarkerTeleportPath)
				{
					RE::PlayerCharacter__SetMarkerTeleportData(player, playerMapMarker.get().get(), playerMarkerTeleportPath, true);

					AddPlayerMapMarkerToMap(a_localMapMenu->mapMarkers);

					// Only if the map menu is open. Make sure because someone maybe makes a minimap mod.
					auto mapMenu = RE::UI::GetSingleton()->GetMenu<RE::MapMenu>().get();
					RE::BSTArray<RE::MapMenuMarker>& mapMenuMarkers = REL::Module::IsVR() ? mapMenu->GetVRRuntimeData2()->mapMarkers :
																							mapMenu->GetRuntimeData2()->mapMarkers;

					AddPlayerMapMarkerToMap(mapMenuMarkers);

					if (REL::Module::IsVR())
					{
						RE::MapMenu__SetMarkers(mapMenu);
					}
				}
			}
		}
	}

	void PlayerSetMarkerManager::MessageBox::Callback::Run(Message a_optionIndex)
	{
		auto player = RE::PlayerCharacter::GetSingleton();

		switch (static_cast<std::uint32_t>(a_optionIndex))
		{
		case 0: // Move
			PlaceMarkerImmediate(localMapMenu, wndPointX, wndPointY);
			break;
		case 2: // Remove
			RE::PlayerCharacter__RemovePlayerMapMarker(player);

			if (REL::Module::IsVR())
			{
				auto mapMenu = RE::UI::GetSingleton()->GetMenu<RE::MapMenu>().get();
				RE::MapMenu__SetMarkers(mapMenu);
			}
			break;
		default: // Leave
			break;
		}

		if (REL::Module::IsVR())
		{
			auto messageBoxMenu = RE::UI::GetSingleton()->GetMenu<RE::MessageBoxMenu>().get();

			RE::GFxMovieView* movie = messageBoxMenu->uiMovie.get();

			RE::GFxValue messageMenu;
			if (movie->GetVariable(&messageMenu, "MessageMenu"))
			{
				RE::GFxValue y;
				messageMenu.GetMember("_y", &y);
				messageMenu.SetMember("_y", y.GetNumber() + 280.0F);

			}
		}
	}

	void PlayerSetMarkerManager::PlaceMarker(RE::LocalMapMenu* a_localMapMenu, float a_wndPointX, float a_wndPointY)
	{
		allowPlaceMarker = false;

		auto player = RE::PlayerCharacter::GetSingleton();
		
		RE::ObjectRefHandle playerMapMarker = REL::Module::IsVR() ? player->GetVRInfoRuntimeData()->playerMapMarker : player->GetInfoRuntimeData().playerMapMarker;

		if (playerMapMarker)
		{
			messageBox.callback->SetData(a_localMapMenu, a_wndPointX, a_wndPointY);

			RE::UI__OpenMessageBox(messageBox.title, messageBox.callback, 0, 25, 4, messageBox.options);

			if (REL::Module::IsVR())
			{
				auto messageBoxMenu = RE::UI::GetSingleton()->GetMenu<RE::MessageBoxMenu>().get();

				RE::GFxMovieView* movie = messageBoxMenu->uiMovie.get();

				RE::GFxValue messageMenu;
				if (movie->GetVariable(&messageMenu, "MessageMenu"))
				{
					RE::GFxValue y;
					messageMenu.GetMember("_y", &y);
					messageMenu.SetMember("_y", y.GetNumber() - 280.0F);

				}
			}
		}
		else
		{
			PlaceMarkerImmediate(a_localMapMenu, a_wndPointX, a_wndPointY);
		}
	}
}
