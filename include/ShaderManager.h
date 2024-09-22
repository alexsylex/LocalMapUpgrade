#pragma once

#include "Settings.h"

namespace LMU
{
	class ShaderManager
	{
	public:
		static void InitSingleton()
		{
			if (!singleton)
			{
				static ShaderManager singletonInstance;
				singleton = &singletonInstance;
			}
		}

		static ShaderManager* GetSingleton() { return singleton; }

		void ToggleFogOfWarLocalMapShader();

		void EnableWaterRendering() { enableWaterRendering = true; }
		void DisableWaterRendering() { enableWaterRendering = false; }

	private:
		ShaderManager();

		REX::W32::ID3D11PixelShader* CompilePixelShader(const char* a_pixelShaderSrc,
														const std::vector<const char*>& a_defineNames);

		static inline ShaderManager* singleton = nullptr;

		bool& enableWaterRendering = *REL::Relocation<bool*>{ RELOCATION_ID(513342, 391120) }.get();

		RE::BSGraphics::PixelShader* localMapPixelShader = nullptr;

		REX::W32::ID3D11PixelShader* originalProgram = nullptr;
		REX::W32::ID3D11PixelShader* noFogOfWarProgram = nullptr;
		REX::W32::ID3D11PixelShader* colorProgram = nullptr;
		REX::W32::ID3D11PixelShader* colorNoFogOfWarProgram = nullptr;

		bool colorEnabled = settings::mapmenu::localMapColor;
		bool fogOfWarEnabled = settings::mapmenu::localMapFogOfWar;
	};
}