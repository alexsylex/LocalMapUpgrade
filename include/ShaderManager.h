#pragma once

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

	private:
		ShaderManager();

		REX::W32::ID3D11PixelShader* CompilePixelShader(const char* a_pixelShaderSrc,
														const std::vector<const char*>& a_defineNames);

		static inline ShaderManager* singleton = nullptr;

		RE::BSGraphics::PixelShader* localMapPixelShader = nullptr;

		REX::W32::ID3D11PixelShader* originalProgram = nullptr;
		REX::W32::ID3D11PixelShader* noFogOfWarProgram = nullptr;
		REX::W32::ID3D11PixelShader* colorProgram = nullptr;
		REX::W32::ID3D11PixelShader* colorNoFogOfWarProgram = nullptr;

		bool fogOfWarEnabled = true;
		bool colorEnabled = false;
	};
}