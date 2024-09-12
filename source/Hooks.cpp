#include "Hooks.h"

#include "RE/L/LocalMapMenu.h"

#include "Settings.h"

#include "PlayerMapMarkerManager.h"
#include "ShaderManager.h"

namespace RE
{
	RefHandle AddPlayerMapMarker(BSTArray<MapMenuMarker>& a_mapMarkers)
	{
		RefHandle refHandle;
		static REL::Relocation<RefHandle&(*)(RefHandle&, BSTArray<MapMenuMarker>&)> func{RELOCATION_ID(52186, 53078)};
		func(refHandle, a_mapMarkers);
		return refHandle;
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

	void PlayerCharacter__SetMarkerTeleportData(PlayerCharacter* a_player, TESObjectREFR* a_marker, PlayerCharacter::TeleportPath* a_teleportPath, bool a_ignoreLocks)
	{
		using func_t = decltype(&PlayerCharacter__SetMarkerTeleportData);
		static REL::Relocation<func_t> func{ RELOCATION_ID(39441, 40517) };
		return func(a_player, a_marker, a_teleportPath, a_ignoreLocks);
	}
}

bool CanProcess(RE::LocalMapMenu::InputHandler* a_localMapInputHandler, RE::InputEvent* a_event)
{
	bool canProcess = hooks::LocalMapMenu::InputHandler::CanProcess(a_localMapInputHandler, a_event);

	if (!canProcess)
	{
		if (a_localMapInputHandler->localMapMenu->GetRuntimeData().controlsReady &&
			a_event->GetDevice() == RE::INPUT_DEVICE::kMouse &&
			a_event->GetEventType() == RE::INPUT_EVENT_TYPE::kButton)
		{
			canProcess = true;
		}
	}

	return canProcess;
}

bool ProcessButton(RE::LocalMapMenu::InputHandler* a_localMapInputHandler, RE::ButtonEvent* a_event)
{
	bool retval = hooks::LocalMapMenu::InputHandler::ProcessButton(a_localMapInputHandler, a_event);

	if (a_localMapInputHandler->localMapMenu->GetRuntimeData().controlsReady)
	{
		RE::ButtonEvent* buttonEvent = a_event->AsButtonEvent();
		if (buttonEvent &&
			buttonEvent->GetDevice() == RE::INPUT_DEVICE::kMouse &&
			buttonEvent->GetIDCode() == 1)
		{
			RE::LocalMapMenu* localMapMenu = a_localMapInputHandler->localMapMenu;
			RE::MenuCursor* menuCursor = RE::MenuCursor::GetSingleton();
			float cursorX = menuCursor->cursorPosX - localMapMenu->topLeft.x;
			float cursorY = menuCursor->cursorPosY - localMapMenu->topLeft.y;
			RE::NiCamera* localMapCamera = localMapMenu->localCullingProcess.GetLocalMapCamera()->camera.get();
			float localMapViewWidth = localMapMenu->bottomRight.x - localMapMenu->topLeft.x;
			float localMapViewHeight = localMapMenu->bottomRight.y - localMapMenu->topLeft.y;

			RE::NiPoint3 rayOrigin;
			RE::NiPoint3 rayDir;
			if (localMapCamera->WindowPointToRay(cursorX, cursorY, rayOrigin, rayDir, localMapViewWidth, localMapViewHeight))
			{
				RE::NiPoint3 markerPos;
				if (LMU::PlayerMapMarkerManager::GetRayCollisionPosition(rayOrigin, rayDir, markerPos))
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
					RE::PlayerCharacter__SetMarkerTeleportData(player, playerMapMarker, player->GetInfoRuntimeData().playerMarkerPath, true);

					AddPlayerMapMarker(localMapMenu->mapMarkers);
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

	return retval;
}

bool ToggleFogOfWar(const RE::SCRIPT_PARAMETER* a_paramInfo, RE::SCRIPT_FUNCTION::ScriptData* a_scriptData,
					RE::TESObjectREFR* a_thisObj, RE::TESObjectREFR* a_containingObj, RE::Script* a_scriptObj,
					RE::ScriptLocals* a_locals, double& a_result, std::uint32_t& a_opcodeOffsetPtr)
{
	LMU::ShaderManager::GetSingleton()->ToggleFogOfWarLocalMapShader();
	
	return true;
}
