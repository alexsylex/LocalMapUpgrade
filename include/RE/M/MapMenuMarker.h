#pragma once

#include "RE/E/ExtraMapMarker.h"
#include "RE/T/TESForm.h"

namespace RE
{
	struct MapMenuMarker
	{
		enum Type
		{
			kLocation,
			kQuest,
			kPlayerSet,
			kYouAreHere,
			kUnk4
		};

		MapMarkerData*                        data;          // 00
		RefHandle                             ref;           // 08
		std::uint32_t                         pad0C;         // 0C
		BSFixedString                         description;   // 10
		stl::enumeration<Type, std::uint32_t> type;          // 18
		std::uint32_t                         door;          // 1C
		std::int32_t                          index;         // 20
		std::uint32_t                         pad24;         // 24
		TESQuest*                             quest;         // 28
		std::uint8_t                          unk30;         // 30
		std::uint8_t                          pad31;         // 31
		std::uint16_t                         pad32;         // 32
		std::uint32_t                         pad34;         // 34
	};
	static_assert(sizeof(MapMenuMarker) == 0x38);
}
