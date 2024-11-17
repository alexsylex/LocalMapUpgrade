#pragma once

#include "utils/Trampoline.h"

namespace RE
{
	class BSWaterShader;
}

bool FakeNotSmallWorld(RE::TESWorldSpace* a_worldSpace);

void AddExtraAndQuestMarkersToMap(RE::BSTArray<RE::MapMenuMarker>& a_mapMarkers, RE::BSTArray<RE::BGSInstancedQuestObjective>& a_objectives,
								  std::uint32_t a_arg3);

bool InvokeCreateAndPostProcessMarkers(RE::GFxValue::ObjectInterface* a_objIface, void* a_data, RE::GFxValue* a_result, const char* a_name, const RE::GFxValue* a_args,
									   std::uint32_t a_numArgs, bool a_isDObj);

void SetupWaterShaderTechnique(RE::BSWaterShader* a_shader, std::uint32_t a_technique);

bool CanProcess(RE::LocalMapMenu::InputHandler* a_localMapInputHandler, RE::InputEvent* a_event);
bool ProcessButton(RE::LocalMapMenu::InputHandler* a_localMapInputHandler, RE::ButtonEvent* a_event);

bool ToggleFogOfWar(const RE::SCRIPT_PARAMETER* a_paramInfo, RE::SCRIPT_FUNCTION::ScriptData* a_scriptData, RE::TESObjectREFR* a_thisObj, RE::TESObjectREFR* a_containingObj, RE::Script* a_scriptObj,
					RE::ScriptLocals* a_locals, double& a_result, std::uint32_t& a_opcodeOffsetPtr);

namespace hooks
{
	class LocalMapMenu
	{
		static constexpr REL::VariantID AdvanceId{ 52078, 52966, 0x90ED80 };
		static constexpr REL::VariantID PopulateDataId{ 52081, 52971, 0x90F3C0 };
		//static constexpr REL::VariantID RefreshMarkersId{ 52090, 52980, 0x910A60 };
		
	public:
		class InputHandler
		{
		public:
			static inline REL::Relocation<std::uintptr_t> vTable{ RE::VTABLE_LocalMapMenu__InputHandler[0] };			

			static inline REL::Relocation<bool (RE::LocalMapMenu::InputHandler::*)(RE::InputEvent*)> CanProcess;
			static inline REL::Relocation<bool (RE::LocalMapMenu::InputHandler::*)(RE::ButtonEvent*)> ProcessButton;
		};

		class LocalMapCullingProcess
		{
			static constexpr REL::VariantID RenderOffScreenId{ 16094, 16335, 0x206C90 };

		public:
			static inline REL::Relocation<void (RE::LocalMapMenu::LocalMapCullingProcess::*)()> RenderOffScreen{ RenderOffScreenId };
		};

		static inline REL::Relocation<void (RE::LocalMapMenu::*)()> Advance{ AdvanceId };
		static inline REL::Relocation<void (RE::LocalMapMenu::*)()> PopulateData{ PopulateDataId };
	};
	
	class BSWaterShader
	{
	public:
		static inline REL::Relocation<std::uintptr_t> vTable{ RE::VTABLE_BSWaterShader[0] };

		static inline REL::Relocation<bool (RE::BSWaterShader::*)(std::uint32_t)> SetupTechnique;
	};

	class TESWorldSpace
	{
	public:
		static inline REL::Relocation<bool (RE::TESWorldSpace::*)()> IsSmallWorld;
	};

	class GFxValue
	{
	public:
		class ObjectInterface
		{
		public:
			static inline REL::Relocation<bool (RE::GFxValue::ObjectInterface::*)(void*, RE::GFxValue*, const char*, const RE::GFxValue*, std::uint32_t, bool)> Invoke;
		};
	};

	inline REL::Relocation<void (*)(RE::BSTArray<RE::MapMenuMarker>&, RE::BSTArray<RE::BGSInstancedQuestObjective>, std::uint32_t)> AddQuestMarkersToMap;

	static inline void Install()
	{
		// `LocalMapMenu::LocalMapCullingProcess::RenderOffscreen` (call to `TESWorldSpace::IsSmallWorld`)
		struct IsSmallWorldHook : Hook<5>
		{
			static std::uintptr_t Address() { return LocalMapMenu::LocalMapCullingProcess::RenderOffScreen.address() + REL::Relocate(0x105, 0x105, 0x13A); }

			IsSmallWorldHook(std::uintptr_t a_hookedAddress)
			: Hook{ a_hookedAddress, reinterpret_cast<std::uintptr_t>(&FakeNotSmallWorld) }
			{ }
		};

		// `LocalMapMenu::PopulateData` (call to `AddQuestMarkersToMapHook`)
		struct AddQuestMarkersToMapHook : Hook<5>
		{
			static std::uintptr_t Address() { return LocalMapMenu::PopulateData.address() + REL::Relocate(0x673, 0x7A0, 0x673); }

			AddQuestMarkersToMapHook(std::uintptr_t a_hookedAddress)
				: Hook{ a_hookedAddress, reinterpret_cast<std::uintptr_t>(&AddExtraAndQuestMarkersToMap) }
			{ }
		};

		// `LocalMapMenu::Advance` (call to `GFxValue::ObjectInterface::Invoke` for "CreateMarkers")
		struct InvokeCreateMarkersHook : Hook<5>
		{
			static std::uintptr_t Address() { return LocalMapMenu::Advance.address() + REL::Relocate(0xDF, 0xD9, 0x127); }

			InvokeCreateMarkersHook(std::uintptr_t a_hookedAddress)
				: Hook{ a_hookedAddress, reinterpret_cast<std::uintptr_t>(&InvokeCreateAndPostProcessMarkers) }
			{ }
		};

		IsSmallWorldHook isSmallWorldHook{ IsSmallWorldHook::Address() };
		AddQuestMarkersToMapHook addQuestMarkersToMapHook{ AddQuestMarkersToMapHook::Address() };
		InvokeCreateMarkersHook invokeCreateMarkersHook{ InvokeCreateMarkersHook::Address() };

		static DefaultTrampoline defaultTrampoline
		{
			isSmallWorldHook.getSize() +
			addQuestMarkersToMapHook.getSize() +
			invokeCreateMarkersHook.getSize()
		};

		TESWorldSpace::IsSmallWorld = defaultTrampoline.write_call(isSmallWorldHook);
		AddQuestMarkersToMap = defaultTrampoline.write_call(addQuestMarkersToMapHook);
		GFxValue::ObjectInterface::Invoke = defaultTrampoline.write_call(invokeCreateMarkersHook);

		LocalMapMenu::InputHandler::CanProcess = LocalMapMenu::InputHandler::vTable.write_vfunc(1, CanProcess);
		
		// `LocalMapMenu::InputHandler::ProcessButton` offset in the virtual table changes in VR
		LocalMapMenu::InputHandler::ProcessButton = LocalMapMenu::InputHandler::vTable.write_vfunc(REL::Module::IsVR() ? 8 : 5, ProcessButton);

		BSWaterShader::SetupTechnique = BSWaterShader::vTable.write_vfunc(2, &SetupWaterShaderTechnique);

		RE::SCRIPT_FUNCTION* tfow = RE::SCRIPT_FUNCTION::LocateConsoleCommand("ToggleFogOfWar");
		tfow->executeFunction = &ToggleFogOfWar;
	}
}
