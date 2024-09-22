#pragma once

namespace RE
{
	template <class Key, class Value>
	class BSMapItem
	{
	public:
		BSMapItem* next;
		Key a_key;
		Value val;
	};
	static_assert(sizeof(BSMapItem<void*, std::uint32_t>) == 0x18);

	template <class Key, class Value>
	class BSMapBase
	{
	public:
		using Entry = BSMapItem<Key, Value>;
		using Iterator = void*;

		BSMapBase(std::uint32_t uiHashSize = 37)
		{
			hashSize = uiHashSize;
			count = 0;

			std::uint32_t uiSize = sizeof(BSMapItem<Key, Value>*) * hashSize;
			hashTable = (BSMapItem<Key, Value>)MemoryManager::Allocate(uiSize);
			memset(hashTable, 0, hashSize * sizeof(BSMapItem<Key, Value>*));
		};
		virtual ~BSMapBase();	// 00

		virtual std::uint32_t KeyToHashIndex(Key a_key);								// 01
		virtual bool AreKeysEqual(Key a_key1, Key a_key2);								// 02
		virtual void SetValue(BSMapItem<Key, Value>* a_item, Key a_key, Value a_data);	// 03
		virtual void ClearValue(BSMapItem<Key, Value>* a_item);							// 04
		virtual BSMapItem<Key, Value>* Allocate() = 0;									// 05
		virtual void Deallocate(BSMapItem<Key, Value>* a_item) = 0;						// 06

		std::uint32_t hashSize;
		BSMapItem<Key, Value>** hashTable;
		std::uint32_t count;

		std::uint32_t GetCount() const { return count; }

		bool GetAt(Key a_key, Value& a_data)
		{
			std::uint32_t hashIndex = KeyToHashIndex(a_key);
			BSMapItem<Key, Value>* item = hashTable[hashIndex];
			while (item) {
				if (IsKeysEqual(item->a_key, a_key)) {
					a_data = item->val;
					return true;
				}
				item = item->next;
			}
			return false;
		}

		void SetAt(Key a_key, Value a_data)
		{
			std::uint32_t hashIndex = KeyToHashIndex(a_key);
			BSMapItem<Key, Value>* item = hashTable[hashIndex];
			while (item) {
				if (IsKeysEqual(item->a_key, a_key)) {
					item->val = a_data;
					return;
				}
				item = item->next;
			}
			BSMapItem<Key, Value>* a_nuEntry = Allocate();
			SetValue(a_nuEntry, a_key, a_data);
			a_nuEntry->next = hashTable[hashIndex];
			hashTable[hashIndex] = a_nuEntry;
			count++;
		}

		Value* Lookup(Key a_key)
		{
			Value* data = nullptr;
			GetAt(a_key, *data);
			return data;
		}

		bool Insert(BSMapItem<Key, Value>* a_nuEntry)
		{
			// game code does not appear to care about ordering of entries in buckets
			std::uint32_t bucket = a_nuEntry->a_key % hashSize;
			BSMapItem<Key, Value>* prev = NULL;
			for (BSMapItem<Key, Value>* cur = hashTable[bucket]; cur; cur = cur->next) {
				if (cur->a_key == a_nuEntry->a_key) {
					return false;
				} else if (!cur->next) {
					prev = cur;
					break;
				}
			}

			if (prev) {
				prev->next = a_nuEntry;
			} else {
				hashTable[bucket] = a_nuEntry;
			}

			hashSize++;
			return true;
		}

		void RemoveAll()
		{
			for (std::uint32_t i = 0; i < hashSize; i++) {
				while (hashTable[i]) {
					BSMapItem<Key, Value>* item = hashTable[i];
					hashTable[i] = hashTable[i]->next;
					ClearValue(item);
					Deallocate(item);
				}
			}

			count = 0;
		}

		Iterator GetFirstPos() const
		{
			for (std::uint32_t i = 0; i < hashSize; i++) {
				if (hashTable[i])
					return hashTable[i];
			}
			return 0;
		}

		void GetNext(Iterator& pos, Key& a_key, Value& val)
		{
			BSMapItem<Key, Value>* item = (BSMapItem<Key, Value>*)pos;

			a_key = item->a_key;
			val = item->val;

			if (item->next) {
				pos = item->next;
				return;
			}

			std::uint32_t i = KeyToHashIndex(item->a_key);
			for (++i; i < hashSize; i++) {
				item = hashTable[i];
				if (item) {
					pos = item;
					return;
				}
			}

			pos = 0;
		}
	};
	static_assert(sizeof(BSMapBase<void*, std::uint32_t>) == 0x20);

	template <class Key, class Value>
	class BSMap : public BSMapBase<Key, Value>
	{
	public:
		BSMap(std::uint32_t uiHashSize = 37) :
			BSMapBase<Key, Value>(uiHashSize){};
		~BSMap() { BSMapBase<Key, Value>::RemoveAll(); };

		// override (BSMapBase)
		virtual BSMapItem<Key, Value>* Allocate();				// 05
		virtual void Deallocate(BSMapItem<Key, Value>* pkItem);	// 06
	};
	static_assert(sizeof(BSMap<void*, std::uint32_t>) == 0x20);
}