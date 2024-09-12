#include "PlayerMapMarkerManager.h"

#include "RE/N/NiPick.h"

namespace LMU
{
	bool PlayerMapMarkerManager::GetRayCollisionPosition(const RE::NiPoint3& a_rayOrigin, const RE::NiPoint3& a_rayDir,
														 RE::NiPoint3& a_rayCollision)
	{
		bool hit = false;

		if (auto pick = RE::malloc<RE::NiPick>())
		{
			pick->Ctor();
			pick->observeCull = true;
			pick->type = RE::NiPick::PickType::kFindFirst;

			if (auto lodRoot = skyrim_cast<RE::NiNode*>(RE::ShadowSceneNode::GetMain()->GetChildren()[2].get()))
			{
				if (auto landLod = skyrim_cast<RE::NiNode*>(lodRoot->GetChildren()[0].get()))
				{
					pick->root = landLod->GetChildren()[0];
				}
			}

			hit = pick->PickObjects(a_rayOrigin, a_rayDir);
			if (hit)
			{
				a_rayCollision = pick->results[0]->intersect;
			}

			pick->Dtor();
			RE::free(pick);
		}

		return hit;
	}
}
