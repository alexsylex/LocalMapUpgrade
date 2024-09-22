#include "Hooks.h"

#include "Settings.h"

#include "PlayerMapMarkerManager.h"
#include "ShaderManager.h"

#include "RE/S/ShaderAccumulator.h"

RE::TESWorldSpace* EnableWaterRenderingAndGetWorldSpace(RE::TES* a_tes)
{
	LMU::ShaderManager::GetSingleton()->EnableWaterRendering();

	return RE::TES::GetSingleton()->GetRuntimeData2().worldSpace;
}

void SetupWaterShaderTechnique(RE::BSWaterShader* a_shader, std::uint32_t a_technique)
{
	if (RE::BSGraphics::BSShaderAccumulator::GetCurrentAccumulator()->GetRuntimeData().renderMode == 18)
	{
		// Fixes water flickering
		a_technique &= ~0x802;
	}

	hooks::BSWaterShader::SetupTechnique(a_shader, a_technique);
}

bool CanProcess(RE::LocalMapMenu::InputHandler* a_localMapInputHandler, RE::InputEvent* a_event)
{
	RE::LocalMapMenu::RUNTIME_DATA& localMapMenu = a_localMapInputHandler->localMapMenu->GetRuntimeData();

	if (localMapMenu.showingMap && localMapMenu.controlsReady)
	{
		// Show the place marker buttons

		auto movie = *reinterpret_cast<RE::GFxMovie**&>(localMapMenu.localMapMovie);

		RE::GFxValue bottomBarSetDestinationButton;
		RE::GFxValue bottomBar;
		if (localMapMenu.localMapMovie.GetMember("BottomBar", &bottomBar))
		{
			bottomBar.GetMember("RightButton", &bottomBarSetDestinationButton);
			bottomBarSetDestinationButton.SetMember("visible", true);
		}
		else if (localMapMenu.localMapMovie.GetMember("_bottomBar", &bottomBar))
		{
			RE::GFxValue buttonPanel;
			bottomBar.GetMember("buttonPanel", &buttonPanel);
			buttonPanel.GetMember("button5", &bottomBarSetDestinationButton);
			bottomBarSetDestinationButton.SetMember("_visible", true);
		}
	}

	bool canProcess = hooks::LocalMapMenu::InputHandler::CanProcess(a_localMapInputHandler, a_event);

	if (!canProcess)
	{
		RE::INPUT_DEVICE device = a_event->GetDevice();

		if (localMapMenu.showingMap && localMapMenu.controlsReady &&
			(device == RE::INPUT_DEVICE::kMouse || device == RE::INPUT_DEVICE::kGamepad) &&
			a_event->GetEventType() == RE::INPUT_EVENT_TYPE::kButton)
		{
			canProcess = true;
		}
	}

	return canProcess;
}

bool ProcessButton(RE::LocalMapMenu::InputHandler* a_localMapInputHandler, RE::ButtonEvent* a_event)
{
	RE::LocalMapMenu::RUNTIME_DATA& localMapMenu = a_localMapInputHandler->localMapMenu->GetRuntimeData();

	RE::ButtonEvent* buttonEvent = a_event->AsButtonEvent();

	auto userEvents = RE::UserEvents::GetSingleton();

	if (localMapMenu.showingMap && localMapMenu.controlsReady)
	{
		if (buttonEvent)
		{
			if (buttonEvent->userEvent == userEvents->mapLookMode)
			{
				buttonEvent->userEvent = userEvents->localMapMoveMode;
			}
			else if (buttonEvent->userEvent == userEvents->localMapMoveMode)
			{
				buttonEvent->userEvent = userEvents->click;
			}
		}
	}

	bool retval = hooks::LocalMapMenu::InputHandler::ProcessButton(a_localMapInputHandler, a_event);

	if (localMapMenu.showingMap && localMapMenu.controlsReady)
	{
		if (buttonEvent)
		{
			RE::INPUT_DEVICE device = buttonEvent->GetDevice();

			if (buttonEvent->userEvent == userEvents->click ||
				buttonEvent->userEvent == userEvents->placePlayerMarker)
			{
				auto playerMapMarkerManager = LMU::PlayerMapMarkerManager::GetSingleton();

				if (playerMapMarkerManager->CanPlaceMarker())
				{ 
					RE::MenuCursor* menuCursor = RE::MenuCursor::GetSingleton();

					playerMapMarkerManager->PlaceMarker(a_localMapInputHandler->localMapMenu,
														menuCursor->cursorPosX, menuCursor->cursorPosY);
				}
				else if(buttonEvent->Value() == 0)
				{
					playerMapMarkerManager->AllowPlaceMarker();
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
