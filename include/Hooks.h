#pragma once

#include "utils/Trampoline.h"

bool ToggleFogOfWar(const RE::SCRIPT_PARAMETER* a_paramInfo, RE::SCRIPT_FUNCTION::ScriptData* a_scriptData, RE::TESObjectREFR* a_thisObj, RE::TESObjectREFR* a_containingObj, RE::Script* a_scriptObj,
	RE::ScriptLocals* a_locals, double& a_result, std::uint32_t& a_opcodeOffsetPtr);

bool CanProcess(RE::LocalMapMenu::InputHandler* a_localMapInputHandler, RE::InputEvent* a_event);
bool ProcessButton(RE::LocalMapMenu::InputHandler* a_localMapInputHandler, RE::ButtonEvent* a_event);

namespace hooks
{
	class LocalMapMenu
	{
	public:
		class InputHandler
		{
		public:
			static inline REL::Relocation<std::uintptr_t> vTable{ RE::VTABLE_LocalMapMenu__InputHandler[0] };

			static inline REL::Relocation<bool (RE::LocalMapMenu::InputHandler::*)(RE::InputEvent*)> CanProcess;
			static inline REL::Relocation<bool (RE::LocalMapMenu::InputHandler::*)(RE::ButtonEvent*)> ProcessButton;
		};
	};

	static inline void Install()
	{
		LocalMapMenu::InputHandler::CanProcess = LocalMapMenu::InputHandler::vTable.write_vfunc(1, CanProcess);
		LocalMapMenu::InputHandler::ProcessButton = LocalMapMenu::InputHandler::vTable.write_vfunc(5, ProcessButton);

		RE::SCRIPT_FUNCTION* tfow = RE::SCRIPT_FUNCTION::LocateConsoleCommand("ToggleFogOfWar");
		tfow->executeFunction = &ToggleFogOfWar;
	}
}
