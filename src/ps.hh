#pragma once
// Piotr Szczygieł library thing

#ifndef NDEBUG
#define PS_MEM_DEBUG  // Enable memory leak tracing on debug builds
#endif

#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

using usize = size_t;

using u64 = uint64_t;
using i64 = int64_t;

using u8 = uint8_t;

// Heap definitions
#pragma region

void mem_check_dump();

#define mem_alloc(count) _mem_alloc((count), __FILE__, __LINE__)
#define mem_free(ptr) _mem_free(ptr, __FILE__, __LINE__)
#define mem_realloc(old_ptr, old_size, new_size) _mem_realloc(old_ptr, old_size, new_size, __FILE__, __LINE__)

void* _mem_alloc(usize size, const char* file, int line);
void _mem_free(void* ptr, const char* file, int line);
void* _mem_realloc(void* old_ptr, usize old_size, usize new_size, const char* file, int line);

#pragma endregion

// Defer macro
#pragma region
template <typename F>
struct Defer_Helper {
    F f;
    Defer_Helper(F f) : f(f) {}
    ~Defer_Helper() { f(); }
};

template <typename F>
Defer_Helper<F> defer_helper_func(F f) {
    return Defer_Helper<F>(f);
}

#define DEFER_1(x, y) x##y
#define DEFER_2(x, y) DEFER_1(x, y)
#define DEFER_3(x) DEFER_2(x, __COUNTER__)
#define defer(code) auto DEFER_3(_defer_) = defer_helper_func([&]() { code; })
#pragma endregion

// List
#pragma region
template <typename T>
struct List {
    void push(const T& item) {
        grow(length + 1);
        items[length++] = item;
    }

    T pop() {
        assert(length > 0);
        return items[--length];
    }

    void resize(usize new_length) {
        assert(list);
        grow(list, new_length);
        length = new_length;
    }

    void grow(usize new_capacity) {
        if (capacity >= new_capacity) return;

        usize better_capacity = capacity;
        do {
            better_capacity = better_capacity * 5 / 2 + 8;
        } while (better_capacity < new_capacity);

        items = (T*)mem_realloc(items, capacity * sizeof(T), better_capacity * sizeof(T));
        assert(items);
        capacity = better_capacity;
    }

    usize capacity;
    usize length;
    T* items;
};

template <typename T>
List<T>* list_create() {
    auto list = (List<T>*)mem_alloc(sizeof(List<T>));
    assert(list);
    return list;
}

template <typename T>
void list_free(List<T>* list) {
    assert(list);
    if (list->items) mem_free(list->items);
    mem_free(list);
}
#pragma endregion

// Hashmap
#pragma region

usize hm_fn_hash(const char* key);
bool hm_fn_eq(const char* k1, const char* k2);

usize hm_fn_hash(usize key);
bool hm_fn_eq(usize k1, usize k2);

#define HM_ROTATE_LEFT(val, n) (((val) << (n)) | ((val) >> (sizeof(usize) * 8 - (n))))
#define HM_ROTATE_RIGHT(val, n) (((val) >> (n)) | ((val) << (sizeof(usize) * 8 - (n))))

template <typename K, typename V>
struct Hash_Entry {
    K key;
    V value;
    Hash_Entry* next;
};

template <typename K, typename V>
struct Hash_Map {
    Hash_Entry<K, V>* get(const K& key) {
        usize hash = hm_fn_hash(key) % size;
        auto entry = table[hash];
        while (entry) {
            if (hm_fn_eq(entry->key, key)) return entry;
            entry = entry->next;
        }
        return nullptr;
    }

    void put(const K& key, const V& value) {
        usize hash = hm_fn_hash(key) % size;

        Hash_Entry<K, V>* prev = nullptr;
        auto entry = table[hash];

        while (entry && !hm_fn_eq(entry->key, key)) {
            prev = entry;
            entry = entry->next;
        }

        if (entry) {
            entry->value = value;
            return;
        }

        entry = (Hash_Entry<K, V>*)mem_alloc(sizeof(Hash_Entry<K, V>));
        entry->key = key;
        entry->value = value;

        if (prev) {
            prev->next = entry;
        } else {
            table[hash] = entry;
        }
    }

    List<Hash_Entry<K, V>*>* items() {
        auto list = list_create<Hash_Entry<K, V>*>();
        for (int i = 0; i < size; i++) {
            auto entry = table[i];
            while (entry) {
                list->push(entry);
                entry = entry->next;
            }
        }
        return list;
    }

    usize size;
    Hash_Entry<K, V>** table;
};

template <typename K, typename V>
Hash_Map<K, V>* hm_create(usize size) {
    assert(size > 0);
    auto hm = (Hash_Map<K, V>*)mem_alloc(sizeof(Hash_Map<K, V>));
    assert(hm);
    hm->size = size;
    hm->table = (Hash_Entry<K, V>**)mem_alloc(sizeof(Hash_Entry<K, V>*) * size);
    assert(hm->table);
    return hm;
}

template <typename K, typename V>
void hm_free(Hash_Map<K, V>* hm) {
    assert(hm);
    assert(hm->table);
    for (int i = 0; i < hm->size; i++) {
        auto entry = hm->table[i];
        while (entry) {
            auto prev = entry;
            entry = entry->next;
            mem_free(prev);
        }
    }
    mem_free(hm->table);
    mem_free(hm);
}
#pragma endregion
