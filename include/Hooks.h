#pragma once

#include "utils/Trampoline.h"

bool ToggleFogOfWar(const RE::SCRIPT_PARAMETER* a_paramInfo, RE::SCRIPT_FUNCTION::ScriptData* a_scriptData, RE::TESObjectREFR* a_thisObj, RE::TESObjectREFR* a_containingObj, RE::Script* a_scriptObj,
	RE::ScriptLocals* a_locals, double& a_result, std::uint32_t& a_opcodeOffsetPtr);

namespace hooks
{
	static inline void Install()
	{
		RE::SCRIPT_FUNCTION* tfow = RE::SCRIPT_FUNCTION::LocateConsoleCommand("ToggleFogOfWar");
		tfow->executeFunction = &ToggleFogOfWar;
	}
}
