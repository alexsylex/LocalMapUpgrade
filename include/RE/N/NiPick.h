#pragma once

#include "RE/N/NiAVObject.h"
#include "RE/N/NiPoint3.h"
#include "RE/N/NiSmartPointer.h"
#include "RE/N/NiTArray.h"

namespace RE
{
	class NiSwitchNode;

	template <class T>
	class NiTScrapHeapInterface
	{
	public:
		inline static T* Allocate(std::size_t a_numElements)
		{
			// TODO
			return nullptr;
		};

		inline static void Deallocate(T* a_array)
		{
			// TODO
		};
	};

	template <class T>
	class NiTScrapArray : public NiTArray<T, NiTScrapHeapInterface<T>>
	{
	public:
		NiTScrapArray(std::uint32_t a_maxSize, std::uint32_t a_growBy) :
			NiTArray<T, NiTScrapHeapInterface<T>>(a_maxSize, a_growBy)
		{}
	};
	static_assert(sizeof(NiTScrapArray<void*>) == 0x18);

	class NiPick
	{
	public:
		enum PickType
		{
			kFindAll,
			kFindFirst
		};

		enum SortType
		{
			kSort,
			kNoSort
		};

		enum IntersectType
		{
			kBoundIntersect,
			kTriangleIntersect
		};

		enum CoordinateType
		{
			kModelCoordinates,
			kWorldCoordinates
		};

		struct Record
		{
			NiPointer<NiAVObject> object;
			NiPoint3 intersect;
			NiPoint3 normal;
			float distance;
		};
		static_assert(sizeof(Record) == 0x28);

		struct Results : public NiTScrapArray<NiPick::Record*>
		{
			std::uint32_t numResults;
		};
		static_assert(sizeof(Results) == 0x20);

		NiPick* Ctor(std::uint16_t a_resultsArraySize = 0, std::uint16_t a_resultsArrayGrowBy = 8)
		{
			using func_t = decltype(&NiPick::Ctor);
			REL::Relocation<func_t> func{ REL::VariantID(70361, 71857, 0xCD8370) };

			return func(this, a_resultsArraySize, a_resultsArrayGrowBy);
		}

		void Dtor()
		{
			using func_t = decltype(&NiPick::Dtor);
			REL::Relocation<func_t> func{ REL::VariantID(70362, 71858, 0xCD8430) };

			return func(this);
		}

		bool PickObjects(const NiPoint3& a_origin, const NiPoint3& a_dir, bool a_append = false)
		{
			using func_t = decltype(&NiPick::PickObjects);
			REL::Relocation<func_t> func{ REL::VariantID(70363, 71859, 0xCD8480) };

			return func(this, a_origin, a_dir, a_append);
		}

		// members
		std::uint32_t type;					// 00	
		std::uint32_t sortType;				// 04
		std::uint32_t intersectType;		// 08
		std::uint32_t coordinateType;		// 0C
		bool frontOnly;						// 10
		bool observeCull;					// 11
		NiPointer<NiAVObject> root;			// 18
		NiPick::Results results;			// 20
		NiPick::Record* lastAddedRecord;	// 40
		std::uint8_t retnTexture;			// 48
		std::uint8_t retnNormal;			// 49
		std::uint8_t retnSmoothNormal;		// 4A
		std::uint8_t retnColour;			// 4B
	};
	static_assert(sizeof(NiPick) == 0x50);
}