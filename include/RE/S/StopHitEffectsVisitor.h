#pragma once

#include "RE/A/ActiveEffect.h"

namespace RE
{
	class StopHitEffectVisitor : public ActiveEffect::ForEachHitEffectVisitor
	{
	public:
		// override (ActiveEffect::ForEachHitEffectVisitor)
		virtual BSContainer::ForEachResult operator()(ReferenceEffect* a_hitEffect) override; // 01
	};
}
