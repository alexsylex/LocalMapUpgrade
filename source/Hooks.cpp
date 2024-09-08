#include "Hooks.h"

#include "Settings.h"

#include "ShaderManager.h"

namespace RE
{
	class MapMarker;

	void PlayerCharacter__SetPlayerMapMarker(PlayerCharacter* a_player, const NiPoint3& a_position, const TESForm& a_worldOrCell)
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

RE::RefHandle& AddCustomMarker(RE::RefHandle& a_refHandle, RE::BSTArray<RE::MapMarker>& a_mapMarkers)
{
	auto factory = RE::IFormFactory::GetConcreteFormFactoryByType<RE::TESObjectREFR>();
	RE::TESObjectREFR* objRef = factory ? factory->Create() : nullptr;
	RE::ObjectRefHandle objRefHandle = objRef->CreateRefHandle();

	return a_refHandle;
}

bool CanProcess(RE::LocalMapMenu::InputHandler* a_localMapInputHandler, RE::InputEvent* a_event)
{
	bool canProcess = hooks::LocalMapMenu::InputHandler::CanProcess(a_localMapInputHandler, a_event);

	if (!canProcess)
	{
		if (a_event->GetDevice() == RE::INPUT_DEVICE::kMouse && a_event->GetEventType() == RE::INPUT_EVENT_TYPE::kButton)
		{
			canProcess = true;
		}
	}

	return canProcess;
}

bool ProcessButton(RE::LocalMapMenu::InputHandler* a_localMapInputHandler, RE::ButtonEvent* a_event)
{
	bool retval = hooks::LocalMapMenu::InputHandler::ProcessButton(a_localMapInputHandler, a_event);

	RE::ButtonEvent* buttonEvent = a_event->AsButtonEvent();

	if (buttonEvent && buttonEvent->GetDevice() == RE::INPUT_DEVICE::kMouse && buttonEvent->GetIDCode() == 1)
	{
		RE::LocalMapMenu* localMapMenu = a_localMapInputHandler->localMapMenu;
		RE::NiCamera* localMapCamera = localMapMenu->localCullingProcess.GetLocalMapCamera()->camera.get();

		RE::MenuCursor* menuCursor = RE::MenuCursor::GetSingleton();
		RE::NiPoint3 markerPos;
		RE::NiPoint3 markerDir;
		RE::BSGraphics::State* gfxState = RE::BSGraphics::State::GetSingleton();
		localMapCamera->WindowPointToRay(menuCursor->cursorPosX, menuCursor->cursorPosY, markerPos, markerDir, gfxState->screenWidth, gfxState->screenHeight);

		auto player = RE::PlayerCharacter::GetSingleton();
		auto parentCell = player->GetParentCell();

		markerPos.z = player->GetPosition().z;

		if (parentCell->IsInteriorCell())
		{
			RE::PlayerCharacter__SetPlayerMapMarker(player, markerPos, *player->GetParentCell());
		}
		else
		{
			RE::PlayerCharacter__SetPlayerMapMarker(player, markerPos, *player->GetWorldspace());
		}

		RE::TESObjectREFR* playerMapMarker = player->GetPlayerRuntimeData().playerMapMarker.get().get();
		RE::PlayerCharacter__SetMarkerTeleportData(player, playerMapMarker, player->GetInfoRuntimeData().playerMarkerPath, true);
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
