#include "Settings.h"

#include "ShaderManager.h"

const SKSE::LoadInterface* skse;

void SKSEMessageListener(SKSE::MessagingInterface::Message* a_msg)
{
	// If the data handler has loaded all its forms
	if (a_msg->type == SKSE::MessagingInterface::kDataLoaded) 
	{
		LMU::ShaderManager::InitSingleton();
	}
}