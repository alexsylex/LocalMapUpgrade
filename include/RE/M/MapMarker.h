#pragma once

#include "RE/E/ExtraMapMarker.h"

namespace RE
{
	class MapMarker
	{
	public:

		MapMarkerData* data;
		RefHandle ref;
		BSFixedString description;
		int unk18;
		int isDoor;
		int unk20;
		TESQuest* quest;
		bool unk30;
	};
	static_assert(sizeof(MapMarker) == 0x38);
}