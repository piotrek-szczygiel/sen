#pragma once
// Piotr Szczygieł library thing

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
void* _mem_alloc(usize size, const char* file, int line);
void _mem_free(void* ptr, const char* file, int line);
void* _mem_realloc(void* old_ptr, usize old_size, usize new_size, const char* file, int line);

#define mem_alloc(T, count) (T*)_mem_alloc(sizeof(T) * (count), __FILE__, __LINE__)
#define mem_create(T) (T*)_mem_alloc(sizeof(T), __FILE__, __LINE__)
#define mem_free(ptr) _mem_free(ptr, __FILE__, __LINE__)
#define mem_realloc(old_ptr, old_size, new_size) _mem_realloc(old_ptr, old_size, new_size, __FILE__, __LINE__)
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
    usize capacity;
    usize length;
    T* items;
};

template <typename T>
List<T>* list_create() {
    auto list = mem_create(List<T>);
    assert(list);
    return list;
}

template <typename T>
void list_free(List<T>* list) {
    assert(list);
    mem_free(list->items);
    mem_free(list);
}

template <typename T>
void list_push(List<T>* list, const T& item) {
    assert(list);
    list_grow(list, list->length + 1);
    list->items[list->length++] = item;
}

template <typename T>
T list_pop(List<T>* list) {
    assert(list);
    assert(list->length > 0);
    return list->items[--list->length];
}

template <typename T>
void list_resize(List<T>* list, usize new_length) {
    assert(list);
    list_grow(list, new_length);
    list->length = new_length;
}

template <typename T>
void list_grow(List<T>* list, usize new_capacity) {
    assert(list);
    if (list->capacity >= new_capacity) return;

    usize better_capacity = list->capacity;
    do {
        better_capacity = better_capacity * 5 / 2 + 8;
    } while (better_capacity < new_capacity);

    list->items = mem_realloc(list->items, list->capacity * sizeof(T), better_capacity * sizeof(T));
    assert(list->items);
    list->capacity = better_capacity;
}
#pragma endregion

// Hashmap
#pragma region
template <typename V>
struct Hash_Entry {
    const char* key;
    V value;
    Hash_Entry* next;
};

template <typename V>
struct Dictionary {
    usize size;
    Hash_Entry<V>** table;
};

template <typename V>
Dictionary<V>* dict_create(usize size) {
    assert(size > 0);
    auto hm = mem_create(Dictionary<V>);
    assert(hm);
    hm->size = size;
    hm->table = mem_alloc(Hash_Entry<V>*, size);
    assert(hm->table);
    return hm;
}

template <typename V>
void dict_free(Dictionary<V>* hm) {
    assert(hm);
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

#define DICT_ROTATE_LEFT(val, n) (((val) << (n)) | ((val) >> (sizeof(usize) * 8 - (n))))
#define DICT_ROTATE_RIGHT(val, n) (((val) >> (n)) | ((val) << (sizeof(usize) * 8 - (n))))

template <typename V>
usize dict_fn_hash(Dictionary<V>* hm, const char* key) {
    assert(key);
    constexpr usize seed = 0x1234567890ABCDEF;
    usize hash = seed;
    while (*key) hash = DICT_ROTATE_LEFT(hash, 9) + (u8)*key++;
    hash ^= seed;
    hash = (~hash) + (hash << 18);
    hash ^= hash ^ DICT_ROTATE_RIGHT(hash, 31);
    hash = hash * 21;
    hash ^= hash ^ DICT_ROTATE_RIGHT(hash, 11);
    hash += (hash << 6);
    hash ^= DICT_ROTATE_RIGHT(hash, 22);
    return (hash + seed) % hm->size;
}

bool dict_fn_eq(const char* k1, const char* k2) {
    return strcmp(k1, k2) == 0;
}

template <typename V>
V* dict_get(Dictionary<V>* hm, const char* key) {
    assert(hm);
    usize hash = dict_fn_hash(hm, key);
    auto entry = hm->table[hash];
    while (entry) {
        if (dict_fn_eq(entry->key, key)) return &entry->value;
        entry = entry->next;
    }
    return nullptr;
}

template <typename V>
void dict_put(Dictionary<V>* hm, const char* key, const V& value) {
    assert(hm);
    usize hash = dict_fn_hash(hm, key);

    Hash_Entry<V>* prev = nullptr;
    auto entry = hm->table[hash];

    while (entry && !dict_fn_eq(entry->key, key)) {
        prev = entry;
        entry = entry->next;
    }

    if (entry) {
        entry->value = value;
        return;
    }

    entry = mem_create(Hash_Entry<V>);
    entry->key = key;
    entry->value = value;

    if (prev) {
        prev->next = entry;
    } else {
        hm->table[hash] = entry;
    }
}
#pragma endregion

// Debug allocator implementation
#pragma region
void* _mem_alloc(usize size, const char* file, int line) {
    printf("allocating at %s:%d\n", file, line);
    auto ptr = calloc(size, 1);
    assert(ptr);
    return ptr;
}

void _mem_free(void* ptr, const char* file, int line) {
    printf("freeing at %s:%d\n", file, line);
    free(ptr);
}

void* _mem_realloc(void* old_ptr, usize old_size, usize new_size, const char* file, int line) {
    printf("reallocating at %s:%d\n", file, line);
    auto new_ptr = realloc(old_ptr, new_size);
    assert(new_ptr);
    memset((u8*)new_ptr + old_size, 0, new_size - old_size);
    return new_ptr;
}
#pragma endregion
