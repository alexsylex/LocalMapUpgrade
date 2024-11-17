#include "ExtraMarkersManager.h"

#include "Settings.h"

#include "RE/L/LocalMapCamera.h"
#include "RE/M/MapMenuMarker.h"

namespace RE
{
	std::int32_t TESObjectREFR_GetInventoryCount(TESObjectREFR* a_object, bool a_useDataHandlerInventory = false, bool a_unk03 = false)
	{
		using func_t = decltype(&TESObjectREFR_GetInventoryCount);
		REL::Relocation<func_t> func{ REL::VariantID{ 19274, 19700, 0x29F980 } };
		return func(a_object, a_useDataHandlerInventory, a_unk03);
	}

	std::int32_t ExtraDataList_GetDroppedWeapon(ExtraDataList* a_extraList, TESObjectREFRPtr& a_weapon)
	{
		using func_t = decltype(&ExtraDataList_GetDroppedWeapon);
		REL::Relocation<func_t> func{ REL::VariantID{ 11616, 11762, 0x1266A0 } };
		return func(a_extraList, a_weapon);
	}

	std::int32_t ExtraDataList_GetDroppedUtil(ExtraDataList* a_extraList, TESObjectREFRPtr& a_util)
	{
		using func_t = decltype(&ExtraDataList_GetDroppedUtil);
		REL::Relocation<func_t> func{ REL::VariantID{ 11617, 11763, 0x126870 } };
		return func(a_extraList, a_util);
	}

	bool TESObjectREFR_HasAnyDroppedItem(TESObjectREFR* a_ref)
	{
		if (std::int32_t inventoryCount = RE::TESObjectREFR_GetInventoryCount(a_ref))
		{
			return true;
		}
		else
		{
			if (a_ref->formType == RE::FormType::ActorCharacter)
			{
				RE::TESObjectREFRPtr carriedDroppedWeapon;
				RE::ExtraDataList_GetDroppedWeapon(&a_ref->extraList, carriedDroppedWeapon);
				if (carriedDroppedWeapon)
				{
					return true;
				}

				RE::TESObjectREFRPtr carriedDroppedUtil;
				RE::ExtraDataList_GetDroppedUtil(&a_ref->extraList, carriedDroppedUtil);
				if (carriedDroppedUtil)
				{
					return true;
				}
			}

			return false;
		}
	}

	// Added here because the virtual function seems broken in CommonLibVR
	bool Actor__IsDead(Actor* a_actor, bool a_notEssential = true)
	{
		ACTOR_LIFE_STATE lifeState = a_actor->AsActorState()->actorState1.lifeState;

		if (lifeState == ACTOR_LIFE_STATE::kDying ||
			lifeState == ACTOR_LIFE_STATE::kDead ||
			lifeState == ACTOR_LIFE_STATE::kRecycle)
		{
			return true;
		}

		if (!a_notEssential)
		{
			return lifeState == ACTOR_LIFE_STATE::kEssentialDown;
		}

		return false;
	}
}

namespace LMU
{
	void ExtraMarkersManager::AddExtraMarker(RE::ActorHandle& a_actorHandle, const RE::NiPointer<RE::Actor>& a_actor,
											 RE::BSTArray<RE::MapMenuMarker>& a_mapMarkers)
	{
		RE::MapMenuMarker mapMarker
		{
			.data = nullptr,
			.ref = a_actorHandle.native_handle(),
			.description = a_actor->GetDisplayFullName(),
			.type = RE::MapMenuMarker::Type::kLocation,
			.door = 0,
			.index = -1,
			.quest = nullptr,
			.unk30 = 1
		};

		a_mapMarkers.push_back(mapMarker);
	}

	void ExtraMarkersManager::AddExtraMarkers(RE::BSTArray<RE::MapMenuMarker>& a_mapMarkers)
	{
		RE::PlayerCharacter* player = RE::PlayerCharacter::GetSingleton();
		RE::BSTArray<RE::ActorHandle>& actorHandles = RE::ProcessLists::GetSingleton()->highActorHandles;
		RE::BSTArray<RE::ActorHandle>& enemyHandles = REL::Module::IsVR() ? player->GetVRInfoRuntimeData()->actorsToDisplayOnTheHUDArray : player->GetInfoRuntimeData().actorsToDisplayOnTheHUDArray;

		for (RE::ActorHandle& actorHandle : actorHandles)
		{
			if (RE::NiPointer<RE::Actor> actor = actorHandle.get())
			{
				bool isActorEnemy = false;

				for (RE::ActorHandle& enemyActorHandle : enemyHandles)
				{
					if (actorHandle == enemyActorHandle)
					{
						isActorEnemy = true;
						AddExtraMarker(actorHandle, actor, a_mapMarkers);
						break;
					}
				}

				if (Actor__IsDead(actor.get()))
				{
					if (RE::TESObjectREFR_HasAnyDroppedItem(actor.get()))
					{
						AddExtraMarker(actorHandle, actor, a_mapMarkers);
					}
				}
				else if (!isActorEnemy && actor->AsActorValueOwner()->GetActorValue(RE::ActorValue::kAggression) != 0.0F)
				{
					if (actor->IsHostileToActor(player))
					{
						AddExtraMarker(actorHandle, actor, a_mapMarkers);
					}
					else if (actor->IsGuard())
					{
						AddExtraMarker(actorHandle, actor, a_mapMarkers);
					}
				}
			}
		}
	}

	void ExtraMarkersManager::PostCreateMarkers(RE::GFxValue& a_iconDisplay)
	{
		RE::GFxValue extraMarkersData;
		a_iconDisplay.GetMember("ExtraMarkerData", &extraMarkersData);

		if (!extraMarkersData.IsArray())
		{
			return;
		}

		extraMarkersData.ClearElements();

		RE::PlayerCharacter* player = RE::PlayerCharacter::GetSingleton();
		RE::BSTArray<RE::ActorHandle>& actorHandles = RE::ProcessLists::GetSingleton()->highActorHandles;
		RE::BSTArray<RE::ActorHandle>& enemyHandles = REL::Module::IsVR() ? player->GetVRInfoRuntimeData()->actorsToDisplayOnTheHUDArray : player->GetInfoRuntimeData().actorsToDisplayOnTheHUDArray;

		for (RE::ActorHandle& actorHandle : actorHandles)
		{
			if (RE::NiPointer<RE::Actor> actor = actorHandle.get())
			{
				bool isActorEnemy = false;

				for (RE::ActorHandle& enemyActorHandle : enemyHandles)
				{
					if (actorHandle == enemyActorHandle)
					{
						isActorEnemy = true;
						extraMarkersData.PushBack(ExtraMarker::Type::kEnemy);
						break;
					}
				}

				if (Actor__IsDead(actor.get()))
				{
					if (RE::TESObjectREFR_HasAnyDroppedItem(actor.get()))
					{
						extraMarkersData.PushBack(ExtraMarker::Type::kDead);
					}
				}
				else if (!isActorEnemy && actor->AsActorValueOwner()->GetActorValue(RE::ActorValue::kAggression) != 0.0F)
				{
					if (actor->IsHostileToActor(player))
					{
						extraMarkersData.PushBack(ExtraMarker::Type::kHostile);
					}
					else if (actor->IsGuard())
					{
						extraMarkersData.PushBack(ExtraMarker::Type::kGuard);
					}
				}
			}
		}

		a_iconDisplay.Invoke("PostCreateMarkers", std::array<RE::GFxValue, 4>{ settings::mapmenu::localMapShowEnemyActors, settings::mapmenu::localMapShowHostileActors,
																			   settings::mapmenu::localMapShowGuardActors, settings::mapmenu::localMapShowDeadActors });
	}
}