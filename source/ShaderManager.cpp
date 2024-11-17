#include "ShaderManager.h"

#include "utils/Logger.h"

#include <d3dcompiler.h>

namespace RE::VR
{
	class ImageSpaceManager
	{
	public:
		enum ImageSpaceEffectEnum
		{
			ISLocalMap = 101, // BSImagespaceShaderLocalMap
		};
	};
}

namespace LMU
{
	static constexpr const char* pixelShaderSrc =
	{
#include "ISLocalMap.hlsl.h"
	};

	ShaderManager::ShaderManager()
	{
		std::uint32_t isLocalMapIndex = REL::Module::IsVR() ? RE::VR::ImageSpaceManager::ISLocalMap : RE::ImageSpaceManager::ISLocalMap;

		RE::ImageSpaceEffect* localMapShaderEffect = RE::ImageSpaceManager::GetSingleton()->effects[isLocalMapIndex];

		auto localMapShader = skyrim_cast<RE::BSImagespaceShader*>(localMapShaderEffect);

		if (localMapShader)
		{
			localMapPixelShader = *localMapShader->pixelShaders.begin();

			// Squared Black & White
			squaredShaders.blackNWhite.fogOfWar = CompilePixelShader(pixelShaderSrc);
			squaredShaders.blackNWhite.noFogOfWar = CompilePixelShader(pixelShaderSrc, { "NO_FOG_OF_WAR" });

			// Squared Color
			squaredShaders.color.fogOfWar = CompilePixelShader(pixelShaderSrc, { "COLOR" });
			squaredShaders.color.noFogOfWar = CompilePixelShader(pixelShaderSrc, { "COLOR", "NO_FOG_OF_WAR" });

			// Round Black & White
			roundShaders.blackNWhite.fogOfWar = CompilePixelShader(pixelShaderSrc, { "ROUND" });
			roundShaders.blackNWhite.noFogOfWar = CompilePixelShader(pixelShaderSrc, { "ROUND", "NO_FOG_OF_WAR" });

			// Round Color
			roundShaders.color.fogOfWar = CompilePixelShader(pixelShaderSrc, { "ROUND", "COLOR" });
			roundShaders.color.noFogOfWar = CompilePixelShader(pixelShaderSrc, { "ROUND", "COLOR", "NO_FOG_OF_WAR" });

			isFogOfWarEnabled = settings::mapmenu::localMapFogOfWar;

			SetPixelShaderProperties(shape, style);
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
			logger::critical("Pixel shader failed to compile.");
			if (errorBlob)
			{
				logger::critical("{}", static_cast<LPCSTR>(errorBlob->GetBufferPointer()));
			}
		}
		else
		{
			logger::debug("Pixel shader succesfully compiled.");
		}

		REX::W32::ID3D11Device* device = RE::BSGraphics::Renderer::GetSingleton()->GetRuntimeData().forwarder;

		REX::W32::ID3D11PixelShader* pixelShaderProgram = nullptr;

		if (FAILED(device->CreatePixelShader(pixelShaderBlob->GetBufferPointer(), pixelShaderBlob->GetBufferSize(),
			nullptr, &pixelShaderProgram)))
		{
			logger::critical("Failed to create pixel shader.");
		}
		else
		{
			logger::debug("Pixel shader succesfully created.");
		}

		return pixelShaderProgram;
	}

	void ShaderManager::ToggleFogOfWarLocalMapShader()
	{
		isFogOfWarEnabled = !isFogOfWarEnabled;

		SetPixelShaderProperties(shape, style);

		if (RE::ConsoleLog::IsConsoleMode())
		{
			RE::ConsoleLog::GetSingleton()->Print("Fog of war - %s.", isFogOfWarEnabled ? "ENABLED" : "DISABLED");
		}
	}

	void ShaderManager::SetPixelShaderProperties(PixelShaderProperty::Shape a_shape, PixelShaderProperty::Style a_style)
	{
		PixelShaderGroup& styleShaders = a_shape == PixelShaderProperty::Shape::kRound ? roundShaders : squaredShaders;
		PixelShaderGroup::FogOfWarGroup& shaders = a_style == PixelShaderProperty::Style::kColor ? styleShaders.color : styleShaders.blackNWhite;
		localMapPixelShader->shader = isFogOfWarEnabled ? shaders.fogOfWar : shaders.noFogOfWar;
	}

	void ShaderManager::GetPixelShaderProperties(PixelShaderProperty::Shape& a_shape, PixelShaderProperty::Style& a_style)
	{
		a_shape = singleton->shape;
		a_style = singleton->style;
	}
}