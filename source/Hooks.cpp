 #include "Hooks.h"

#include "Settings.h"

#include "ExtraMarkersManager.h"
#include "PlayerSetMarkerManager.h"
#include "ShaderManager.h"

#include "RE/G/GFxValue.h"
#include "RE/S/StopHitEffectsVisitor.h"
#include "RE/S/ShaderAccumulator.h"

bool FakeNotSmallWorld(RE::TESWorldSpace* a_worldSpace)
{
	hooks::TESWorldSpace::IsSmallWorld(a_worldSpace);

	return false;
}

void AddExtraAndQuestMarkersToMap(RE::BSTArray<RE::MapMenuMarker>& a_mapMarkers, RE::BSTArray<RE::BGSInstancedQuestObjective>& a_objectives,
								  std::uint32_t a_arg3)
{
	auto& localMapMenu = *(RE::LocalMapMenu*)((std::uintptr_t)&a_mapMarkers - offsetof(RE::LocalMapMenu, mapMarkers));

	LMU::ExtraMarkersManager::GetSingleton()->AddExtraMarkers(localMapMenu);

	hooks::AddQuestMarkersToMap(a_mapMarkers, a_objectives, a_arg3);
}

bool InvokeCreateAndPostProcessMarkers(RE::GFxValue::ObjectInterface* a_objIface, void* a_data, RE::GFxValue* a_result, const char* a_name, const RE::GFxValue* a_args,
									   std::uint32_t a_numArgs, bool a_isDObj)
{
	if (hooks::GFxValue::ObjectInterface::Invoke(a_objIface, a_data, a_result, a_name, a_args, a_numArgs, a_isDObj))
	{
		// Build manually our GFxValue for `iconDisplay`
		RE::GFxValue iconDisplay;
		iconDisplay._objectInterface = a_objIface;
		iconDisplay._type = a_isDObj ? RE::GFxValue::ValueType::kDisplayObject : RE::GFxValue::ValueType::kObject;
		iconDisplay._value.obj = a_data;

		LMU::ExtraMarkersManager::GetSingleton()->PostCreateMarkers(iconDisplay);

		return true;
	}

	return false;
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
	bool canProcess = hooks::LocalMapMenu::InputHandler::CanProcess(a_localMapInputHandler, a_event);

	if (!REL::Module::IsVR())
	{
		RE::LocalMapMenu::RUNTIME_DATA& localMapMenu = a_localMapInputHandler->localMapMenu->GetRuntimeData();

		if (localMapMenu.showingMap && localMapMenu.controlsReady)
		{
			// Show the place marker buttons
			RE::GFxValue bottomBarSetDestinationButton;
			RE::GFxValue bottomBar;
			if (localMapMenu.root.GetMember("BottomBar", &bottomBar))
			{
				bottomBar.GetMember("RightButton", &bottomBarSetDestinationButton);
				bottomBarSetDestinationButton.SetMember("visible", true);
			}
			else if (localMapMenu.root.GetMember("_bottomBar", &bottomBar))
			{
				RE::GFxValue buttonPanel;
				bottomBar.GetMember("buttonPanel", &buttonPanel);
				buttonPanel.GetMember("button5", &bottomBarSetDestinationButton);
				bottomBarSetDestinationButton.SetMember("_visible", true);
			}

			if (!canProcess)
			{
				RE::INPUT_DEVICE device = a_event->GetDevice();

				if (a_event->GetEventType() == RE::INPUT_EVENT_TYPE::kButton)
				{
					canProcess = true;
				}
			}
		}
	}

	return canProcess;
}

bool ProcessButton(RE::LocalMapMenu::InputHandler* a_localMapInputHandler, RE::ButtonEvent* a_event)
{
	RE::LocalMapMenu::RUNTIME_DATA& localMapMenu = a_localMapInputHandler->localMapMenu->GetRuntimeData();

	RE::ButtonEvent* buttonEvent = a_event->AsButtonEvent();
	RE::INPUT_DEVICE device = buttonEvent->GetDevice();

	bool isLocationFinderShown = false;

	RE::GFxValue locationFinder;
	localMapMenu.root.GetMember("_locationFinder", &locationFinder);
	if (locationFinder.IsObject())
	{
		RE::GFxValue locationFinderShown;
		locationFinder.GetMember("_bShown", &locationFinderShown);
		isLocationFinderShown = locationFinderShown.GetBool();
	}

	auto userEvents = RE::UserEvents::GetSingleton();

	bool skipVanillaProcessButton = false;

	if (localMapMenu.showingMap && localMapMenu.controlsReady && !isLocationFinderShown)
	{
		if (buttonEvent)
		{
#ifdef ENABLE_SKYRIM_VR
			if (REL::Module::IsVR())
			{
				if (device == RE::INPUT_DEVICE::kViveSecondary ||
					device == RE::INPUT_DEVICE::kOculusSecondary ||
					device == RE::INPUT_DEVICE::kWMRSecondary)
				{
					if (buttonEvent->userEvent == userEvents->click)
					{
						skipVanillaProcessButton = true;
					}
				}
			}
			else
#endif
			if (device == RE::INPUT_DEVICE::kMouse)
			{
				RE::MenuCursor::RUNTIME_DATA& menuCursor = RE::MenuCursor::GetSingleton()->GetRuntimeData();
				RE::LocalMapMenu* localMapMenu = a_localMapInputHandler->localMapMenu;

				if (menuCursor.cursorPosX > localMapMenu->topLeft.x && menuCursor.cursorPosX < localMapMenu->bottomRight.x &&
					menuCursor.cursorPosY > localMapMenu->topLeft.y && menuCursor.cursorPosY < localMapMenu->bottomRight.y)
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
		}
	}

	bool retval = skipVanillaProcessButton ? false : hooks::LocalMapMenu::InputHandler::ProcessButton(a_localMapInputHandler, a_event);

	if (localMapMenu.showingMap && localMapMenu.controlsReady && !isLocationFinderShown)
	{
		if (buttonEvent)
		{
			if ((buttonEvent->userEvent == userEvents->click
#ifdef ENABLE_SKYRIM_VR
				&& (REL::Module::IsVR() &&
					(device == RE::INPUT_DEVICE::kViveSecondary ||
					 device == RE::INPUT_DEVICE::kOculusSecondary ||
					 device == RE::INPUT_DEVICE::kWMRSecondary))
#endif
				) || buttonEvent->userEvent == userEvents->placePlayerMarker)
			{
				auto playerSetMarkerManager = LMU::PlayerSetMarkerManager::GetSingleton();

				if (playerSetMarkerManager->CanPlaceMarker())
				{ 
					float wndPointX{};
					float wndPointY{};

					if (REL::Module::IsVR() && device == RE::INPUT_DEVICE::kGamepad)
					{
						RE::LocalMapMenu* localMapMenu = a_localMapInputHandler->localMapMenu;

						float localMapViewWidth = localMapMenu->bottomRight.x - localMapMenu->topLeft.x;
						float localMapViewHeight = localMapMenu->bottomRight.y - localMapMenu->topLeft.y;

						// In VR, for gamepad users, use the center of the Local Map menu as reference
						wndPointX = localMapMenu->topLeft.x + localMapViewWidth / 2;
						wndPointY = localMapMenu->topLeft.y + localMapViewHeight / 2;
					}
					else
					{
						RE::MenuCursor::RUNTIME_DATA& menuCursor = RE::MenuCursor::GetSingleton()->GetRuntimeData();

						wndPointX = menuCursor.cursorPosX;
						wndPointY = menuCursor.cursorPosY;
					}

					playerSetMarkerManager->PlaceMarker(a_localMapInputHandler->localMapMenu,
														wndPointX, wndPointY);
				}
				else if (buttonEvent->Value() == 0)
				{
					playerSetMarkerManager->AllowPlaceMarker();
				}

				return true;
			}
			else
			{
				RE::LocalMapCamera* localMapCamera = a_localMapInputHandler->localMapMenu->localCullingProcess.GetLocalMapCamera();

				if (buttonEvent->userEvent == userEvents->up)
				{
					localMapCamera->translationInput.y += settings::mapmenu::localMapKeyboardPanSpeed;
				}
				else if (buttonEvent->userEvent == userEvents->down)
				{
					localMapCamera->translationInput.y -= settings::mapmenu::localMapKeyboardPanSpeed;
				}
				else if (buttonEvent->userEvent == userEvents->left)
				{
					localMapCamera->translationInput.x -= settings::mapmenu::localMapKeyboardPanSpeed;
				}
				else if (buttonEvent->userEvent == userEvents->right)
				{
					localMapCamera->translationInput.x += settings::mapmenu::localMapKeyboardPanSpeed;
				}
			}
		}
	}

	return retval;
}

bool IsDetectDeadEffect(RE::DetectLifeEffect* a_detectEffect)
{
	RE::TESConditionItem* condition = a_detectEffect->effect->baseEffect->conditions.head;
	while (condition)
	{
		if (condition->data.functionData.function == RE::FUNCTION_DATA::FunctionID::kGetDead)
		{
			return true;
		}

		condition = condition->next;
	}

	return false;
}

void DetectLifeEffectUpdate(RE::DetectLifeEffect* a_detectEffect, float a_delta)
{
	if (settings::mapmenu::localMapShowActorsOnlyWithDetectSpell)
	{
		auto extraMarkersManager = LMU::ExtraMarkersManager::GetSingleton();

		std::uint32_t detectSpellRadius = a_detectEffect->effect->GetArea();

		if (IsDetectDeadEffect(a_detectEffect))
		{
			std::uint32_t currentUndeadActorDisplayRadius = extraMarkersManager->GetUndeadActorsDisplayRadius();

			if (detectSpellRadius > currentUndeadActorDisplayRadius)
			{
				extraMarkersManager->SetUndeadActorsDisplayRadius(detectSpellRadius);
			}

			std::uint32_t currentDeadActorDisplayRadius = extraMarkersManager->GetDeadActorsDisplayRadius();

			if (detectSpellRadius > currentDeadActorDisplayRadius)
			{
				extraMarkersManager->SetDeadActorsDisplayRadius(detectSpellRadius);
			}
		}
		else
		{
			std::uint32_t currentAliveActorDisplayRadius = extraMarkersManager->GetAliveActorsDisplayRadius();

			if (detectSpellRadius > currentAliveActorDisplayRadius)
			{
				extraMarkersManager->SetAliveActorsDisplayRadius(detectSpellRadius);
			}
		}
	}

	hooks::DetectLifeEffect::Update(a_detectEffect, a_delta);
}

void ScriptEffectUpdate(RE::ScriptEffect* a_scriptEffect, float a_delta)
{
	if (settings::mapmenu::localMapShowActorsOnlyWithDetectSpell)
	{
		// Aura Whisper spell IDs
		static constexpr RE::FormID Laas = 0x8AFCC;
		static constexpr RE::FormID LaasYah = 0x8AFCD;
		static constexpr RE::FormID LaasYahNir = 0x8AFCE;

		RE::FormID thuum = a_scriptEffect->spell->GetFormID();

		if (thuum == Laas || thuum == LaasYah || thuum == LaasYahNir)
		{
			std::uint32_t scriptSpellRadius = a_scriptEffect->effect->GetArea();

			auto extraMarkersManager = LMU::ExtraMarkersManager::GetSingleton();

			std::uint32_t currentUndeadActorDisplayRadius = extraMarkersManager->GetUndeadActorsDisplayRadius();

			if (scriptSpellRadius > currentUndeadActorDisplayRadius)
			{
				extraMarkersManager->SetUndeadActorsDisplayRadius(scriptSpellRadius);
			}

			std::uint32_t currentAliveActorDisplayRadius = extraMarkersManager->GetAliveActorsDisplayRadius();

			if (scriptSpellRadius > currentAliveActorDisplayRadius)
			{
				extraMarkersManager->SetAliveActorsDisplayRadius(scriptSpellRadius);
			}
		}
	}

	hooks::ScriptEffect::Update(a_scriptEffect, a_delta);
}

RE::BSContainer::ForEachResult VisitStopHitEffects(RE::StopHitEffectsVisitor* a_stopHitEffectVisitor, RE::ReferenceEffect* a_effect)
{
	if (settings::mapmenu::localMapShowActorsOnlyWithDetectSpell)
	{
		if (auto activeEffectController = skyrim_cast<RE::ActiveEffectReferenceEffectController*>(a_effect->controller))
		{
			if (auto detectEffect = skyrim_cast<RE::DetectLifeEffect*>(activeEffectController->effect))
			{
				// Maybe here list hit effects that are going to be detached after?
			}
			else if (auto scriptEffect = skyrim_cast<RE::ScriptEffect*>(activeEffectController->effect))
			{
				// Maybe here list hit effects that are going to be detached after?
			}
		}
	}

	return hooks::StopHitEffectsVisitor::Visit(a_stopHitEffectVisitor, a_effect);
}

void DetachShaderReferenceEffect(RE::ShaderReferenceEffect* a_effect)
{
	if (settings::mapmenu::localMapShowActorsOnlyWithDetectSpell)
	{
		// Detachment of the shader has a delay of a couple of seconds after the detect life
		// effect is gone. When we enter here, a_effect->controller == nullptr, so we cannot
		// check the source of the shader effect.
		// Detach both blindly, if any is kept by `ActiveEffect::Update', it will be shown again

		auto extraMarkersManager = LMU::ExtraMarkersManager::GetSingleton();

		extraMarkersManager->SetAliveActorsDisplayRadius(0);
		extraMarkersManager->SetUndeadActorsDisplayRadius(0);
		extraMarkersManager->SetDeadActorsDisplayRadius(0);
	}

	hooks::ShaderReferenceEffect::DetachImpl(a_effect);
}

bool ToggleFogOfWar(const RE::SCRIPT_PARAMETER* a_paramInfo, RE::SCRIPT_FUNCTION::ScriptData* a_scriptData,
					RE::TESObjectREFR* a_thisObj, RE::TESObjectREFR* a_containingObj, RE::Script* a_scriptObj,
					RE::ScriptLocals* a_locals, double& a_result, std::uint32_t& a_opcodeOffsetPtr)
{
	LMU::ShaderManager::GetSingleton()->ToggleFogOfWarLocalMapShader();
	
	return true;
}
