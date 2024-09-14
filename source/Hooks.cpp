#include "Hooks.h"

#include "Settings.h"

#include "PlayerMapMarkerManager.h"
#include "ShaderManager.h"


bool CanProcess(RE::LocalMapMenu::InputHandler* a_localMapInputHandler, RE::InputEvent* a_event)
{
	bool canProcess = hooks::LocalMapMenu::InputHandler::CanProcess(a_localMapInputHandler, a_event);

	if (!canProcess)
	{
		RE::LocalMapMenu::RUNTIME_DATA& localMapMenu = a_localMapInputHandler->localMapMenu->GetRuntimeData();

		if (localMapMenu.showingMap && localMapMenu.controlsReady &&
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

	RE::LocalMapMenu::RUNTIME_DATA& localMapMenu = a_localMapInputHandler->localMapMenu->GetRuntimeData();

	if (localMapMenu.showingMap && localMapMenu.controlsReady)
	{
		RE::ButtonEvent* buttonEvent = a_event->AsButtonEvent();
		if (buttonEvent &&
			buttonEvent->GetDevice() == RE::INPUT_DEVICE::kMouse &&
			buttonEvent->GetIDCode() == 1 &&
			buttonEvent->Value() == 0)
		{
			RE::MenuCursor* menuCursor = RE::MenuCursor::GetSingleton();

			LMU::PlayerMapMarkerManager::GetSingleton()->PlaceMarker(a_localMapInputHandler->localMapMenu,
																	 menuCursor->cursorPosX, menuCursor->cursorPosY);	
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
