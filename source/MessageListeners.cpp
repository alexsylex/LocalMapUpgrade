#include "Settings.h"

#include "ShaderManager.h"
#include "ExtraMarkersManager.h"
#include "PlayerSetMarkerManager.h"

#include "FullAPI.h"

#include "IUI/API.h"

const SKSE::LoadInterface* skse;

namespace LMU
{
	bool isIconDisplayExtensionPatched = false;
}

void InfinityUIMessageListener(SKSE::MessagingInterface::Message* a_msg);

void SKSEMessageListener(SKSE::MessagingInterface::Message* a_msg)
{
	// If all plugins have been loaded
	if (a_msg->type == SKSE::MessagingInterface::kPostLoad)
	{
		if (SKSE::GetMessagingInterface()->RegisterListener("InfinityUI", InfinityUIMessageListener))
		{
			logger::info("Successfully registered for Infinity UI messages!");
		}
		else
		{
			SKSE::stl::report_and_fail
			(
				std::format
				(
					"\n\n"
					"\"Infinity UI\" installation not detected.\n\n"
					"Please, download it from:\n"
					"www.nexusmods.com/skyrimspecialedition/mods/74483"
				)
			);
		}
	}
	// If the data handler has loaded all its forms
	else if (a_msg->type == SKSE::MessagingInterface::kDataLoaded) 
	{
		LMU::ShaderManager::InitSingleton();

		LMU::API::PixelShaderPropertiesHookMessage pixelShaderPropertiesHook;
		pixelShaderPropertiesHook.SetPixelShaderProperties = &LMU::ShaderManager::SetPixelShaderProperties;
		pixelShaderPropertiesHook.GetPixelShaderProperties = &LMU::ShaderManager::GetPixelShaderProperties;
		DispatchMessage(pixelShaderPropertiesHook);

		LMU::ExtraMarkersManager::InitSingleton();
	}
}

void InfinityUIMessageListener(SKSE::MessagingInterface::Message* a_msg)
{
	using namespace IUI;

	if (!a_msg || std::string_view(a_msg->sender) != "InfinityUI")
	{
		return;
	}

	if (auto message = API::TranslateAs<API::Message>(a_msg))
	{
		std::string_view movieUrl = message->movie->GetMovieDef()->GetFileURL();

		if (movieUrl.find("Map") == std::string::npos)
		{
			return;
		}

		switch (a_msg->type)
		{
		case API::Message::Type::kStartLoadInstances:

			logger::info("Started loading patches");
			break;

		case API::Message::Type::kPostPatchInstance:

			if (auto msg = API::TranslateAs<API::PostPatchInstanceMessage>(a_msg))
			{				
				RE::GFxValue iconDisplayExtension;
				msg->newInstance._objectInterface->_movieRoot->GetVariable(&iconDisplayExtension, LMU::ExtraMarkersManager::extensionPath.data());

				if (msg->newInstance._value.obj == iconDisplayExtension._value.obj)
				{
					LMU::isIconDisplayExtensionPatched = true;
				}
			}
			break;

		case API::Message::Type::kFinishLoadInstances:

			logger::info("Finished loading HUD patches");

			if (!LMU::isIconDisplayExtensionPatched)
			{
				SKSE::stl::report_and_fail
				(
					std::format
					(
						"\n\n"
						"\"Data\\Interface\\InfinityUI\\Map\\WorldMap\\LocalMapMenu\\IconDisplayExtension.swf\" not found.\n"
						"Please, check your installation files."
					)
				);
			}
			break;

		case API::Message::Type::kPostInitExtensions:

			if (auto msg = API::TranslateAs<API::PostInitExtensionsMessage>(a_msg))
			{
				logger::debug("Extensions initialization finished");
			}
			break;

		default:
			break;
		}
	}
}