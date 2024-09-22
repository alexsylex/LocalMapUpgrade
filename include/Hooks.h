#pragma once

#include "utils/Trampoline.h"

namespace RE
{
	class BSWaterShader;
}

RE::TESWorldSpace* EnableWaterRenderingAndGetWorldSpace(RE::TES* a_tes);

void SetupWaterShaderTechnique(RE::BSWaterShader* a_shader, std::uint32_t a_technique);

bool CanProcess(RE::LocalMapMenu::InputHandler* a_localMapInputHandler, RE::InputEvent* a_event);
bool ProcessButton(RE::LocalMapMenu::InputHandler* a_localMapInputHandler, RE::ButtonEvent* a_event);

bool ToggleFogOfWar(const RE::SCRIPT_PARAMETER* a_paramInfo, RE::SCRIPT_FUNCTION::ScriptData* a_scriptData, RE::TESObjectREFR* a_thisObj, RE::TESObjectREFR* a_containingObj, RE::Script* a_scriptObj,
					RE::ScriptLocals* a_locals, double& a_result, std::uint32_t& a_opcodeOffsetPtr);

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

		class LocalMapCullingProcess
		{
			static constexpr REL::RelocationID RenderOffScreenId{ 16094, 16335 };

		public:
			static inline REL::Relocation<void (RE::LocalMapMenu::LocalMapCullingProcess::*)()> RenderOffScreen{ RenderOffScreenId };
		};
	};

	class BSWaterShader
	{
	public:
		static inline REL::Relocation<std::uintptr_t> vTable{ RE::VTABLE_BSWaterShader[0] };

		static inline REL::Relocation<bool (RE::BSWaterShader::*)(std::uint32_t)> SetupTechnique;
	};

	static inline void Install()
	{
		// `LocalMapMenu::LocalMapCullingProcess::RenderOffscreen` (call to `TES::GetWorldspace`)
		struct GetWorldSpaceHook : Hook<5>
		{
			static std::uintptr_t Address() { return LocalMapMenu::LocalMapCullingProcess::RenderOffScreen.address() + REL::VariantOffset(0x125, 0x126, 0x125).offset(); }

			GetWorldSpaceHook(std::uintptr_t a_hookedAddress) :
				Hook{ a_hookedAddress, reinterpret_cast<std::uintptr_t>(&EnableWaterRenderingAndGetWorldSpace) }
			{}
		};

		GetWorldSpaceHook getWorldSpaceHook{ GetWorldSpaceHook::Address() };

		static DefaultTrampoline defaultTrampoline{ getWorldSpaceHook.getSize() };

		defaultTrampoline.write_call(getWorldSpaceHook);

		LocalMapMenu::InputHandler::CanProcess = LocalMapMenu::InputHandler::vTable.write_vfunc(1, CanProcess);
		LocalMapMenu::InputHandler::ProcessButton = LocalMapMenu::InputHandler::vTable.write_vfunc(5, ProcessButton);

		BSWaterShader::SetupTechnique = BSWaterShader::vTable.write_vfunc(2, &SetupWaterShaderTechnique);

		RE::SCRIPT_FUNCTION* tfow = RE::SCRIPT_FUNCTION::LocateConsoleCommand("ToggleFogOfWar");
		tfow->executeFunction = &ToggleFogOfWar;
	}
}
