#include "ShaderManager.h"

#include "utils/Logger.h"

#include <d3dcompiler.h>

namespace LMU
{
	static constexpr const char* pixelShaderSrc =
	{
#include "ISLocalMap.hlsl.h"
	};

	ShaderManager::ShaderManager()
	{
		RE::ImageSpaceEffect* localMapShaderEffect = RE::ImageSpaceManager::GetSingleton()->effects[RE::ImageSpaceManager::ISLocalMap];

		auto localMapShader = skyrim_cast<RE::BSImagespaceShader*>(localMapShaderEffect);

		if (localMapShader)
		{
			localMapPixelShader = *localMapShader->pixelShaders.begin();

			originalProgram = localMapPixelShader->shader;
			noFogOfWarProgram = CompilePixelShader(pixelShaderSrc, { "NO_FOG_OF_WAR" });
			colorProgram = CompilePixelShader(pixelShaderSrc, { "COLOR" });
			colorNoFogOfWarProgram = CompilePixelShader(pixelShaderSrc, { "COLOR", "NO_FOG_OF_WAR" });

			if (colorEnabled)
			{
				localMapPixelShader->shader = fogOfWarEnabled ? colorProgram : colorNoFogOfWarProgram;
			}
			else if (!fogOfWarEnabled)
			{
				localMapPixelShader->shader = noFogOfWarProgram;
			}
		}
		else
		{
			logger::critical("Could not find local map shader");
		}
	}

	REX::W32::ID3D11PixelShader* ShaderManager::CompilePixelShader(const char* a_pixelShaderSrc,
																   const std::vector<const char*>& a_defineNames)
	{
		static constexpr std::uint32_t compileFlags = D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_PACK_MATRIX_COLUMN_MAJOR;

		std::vector<D3D_SHADER_MACRO> pixelShaderMacro;
		pixelShaderMacro.reserve(a_defineNames.size() + 1);

		for (const char* defineName : a_defineNames)
		{
			pixelShaderMacro.push_back(D3D_SHADER_MACRO{ .Name{ defineName }, .Definition{ "" } });
		}
		pixelShaderMacro.push_back(D3D_SHADER_MACRO{ nullptr, nullptr });

		REX::W32::ID3DBlob* pixelShaderBlob = nullptr;
		ID3DBlob* errorBlob;
		if (FAILED(D3DCompile(a_pixelShaderSrc, strlen(a_pixelShaderSrc),
			nullptr, pixelShaderMacro.data(), nullptr,
			"main", "ps_5_0", compileFlags, 0,
			reinterpret_cast<ID3DBlob**>(&pixelShaderBlob), &errorBlob)))
		{
			logger::critical("Pixel shader failed to compile");
			if (errorBlob)
			{
				logger::critical("{}", static_cast<LPCSTR>(errorBlob->GetBufferPointer()));
			}
		}
		else
		{
			logger::debug("Pixel shader succesfully compiled");
		}

		REX::W32::ID3D11Device* device = RE::BSGraphics::Renderer::GetSingleton()->GetRuntimeData().forwarder;

		REX::W32::ID3D11PixelShader* pixelShaderProgram = nullptr;

		if (FAILED(device->CreatePixelShader(pixelShaderBlob->GetBufferPointer(), pixelShaderBlob->GetBufferSize(),
			nullptr, &pixelShaderProgram)))
		{
			logger::critical("Failed to create pixel shader");
		}
		else
		{
			logger::debug("Pixel shader succesfully created");
		}

		return pixelShaderProgram;
	}

	void ShaderManager::ToggleFogOfWarLocalMapShader()
	{
		if (fogOfWarEnabled)
		{
			localMapPixelShader->shader = colorEnabled ? colorNoFogOfWarProgram : noFogOfWarProgram;
			fogOfWarEnabled = false;
		}
		else
		{
			localMapPixelShader->shader = colorEnabled ? colorProgram : originalProgram;
			fogOfWarEnabled = true;
		}

		if (RE::ConsoleLog::IsConsoleMode())
		{
			RE::ConsoleLog::GetSingleton()->Print("Fog of war - %s.", fogOfWarEnabled ? "ENABLED" : "DISABLED");
		}
	}
}