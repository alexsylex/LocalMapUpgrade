#pragma once

#include "utils/Trampoline.h"

#include "RE/M/MapMarker.h"

bool ToggleFogOfWar(const RE::SCRIPT_PARAMETER* a_paramInfo, RE::SCRIPT_FUNCTION::ScriptData* a_scriptData, RE::TESObjectREFR* a_thisObj, RE::TESObjectREFR* a_containingObj, RE::Script* a_scriptObj,
	RE::ScriptLocals* a_locals, double& a_result, std::uint32_t& a_opcodeOffsetPtr);

RE::RefHandle& AddCustomMarker(RE::RefHandle& a_refHandle, RE::BSTArray<RE::MapMarker>& a_mapMarkers);

namespace hooks
{
	class LocalMapMenu
	{
		static constexpr REL::RelocationID PopulateDataId{ 52081, 52971 };

	public:

		static inline REL::Relocation<void(RE::LocalMapMenu::*)()> PopulateData{ PopulateDataId };
	};

	inline REL::Relocation<RE::RefHandle&(*)(RE::RefHandle&, RE::BSTArray<RE::MapMarker>&)> AddTeleportMarkers{ RELOCATION_ID(52186, 53078) };

	static inline void Install()
	{
		// `LocalMapMenu::PopulateData` (call to ` AddTeleportMarkers`)
		struct AddTeleportMarkersHook : Hook<5>
		{
			static std::uintptr_t Address() { return LocalMapMenu::PopulateData.address() + REL::VariantOffset{ 0x657, 0x787, 0x657 }.offset(); }

			AddTeleportMarkersHook(std::uintptr_t a_hookedAddress) :
				Hook{ a_hookedAddress, reinterpret_cast<std::uintptr_t>(&AddCustomMarker) }
			{}
		};

		AddTeleportMarkersHook addTeleportMarkersHook{ AddTeleportMarkersHook::Address() };

		static DefaultTrampoline defaultTrampoline{ addTeleportMarkersHook.getSize() };

		defaultTrampoline.write_call(addTeleportMarkersHook);

		RE::SCRIPT_FUNCTION* tfow = RE::SCRIPT_FUNCTION::LocateConsoleCommand("ToggleFogOfWar");
		tfow->executeFunction = &ToggleFogOfWar;
	}
}
