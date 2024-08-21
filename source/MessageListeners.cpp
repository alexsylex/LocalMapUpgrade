#include "Settings.h"

#undef GetModuleHandle

const SKSE::LoadInterface* skse;

void SKSEMessageListener(SKSE::MessagingInterface::Message* a_msg)
{
	// If all plugins have been loaded
	if (a_msg->type == SKSE::MessagingInterface::kPostLoad) 
	{

	}
}