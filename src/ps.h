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
#define mem_alloc(count) _mem_alloc((count), __FILE__, __LINE__)
void* _mem_alloc(usize size, const char* file, int line);

#define mem_free(ptr) _mem_free(ptr, __FILE__, __LINE__)
void _mem_free(void* ptr, const char* file, int line);

#define mem_realloc(old_ptr, old_size, new_size) _mem_realloc(old_ptr, old_size, new_size, __FILE__, __LINE__)
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

#define HM_ROTATE_LEFT(val, n) (((val) << (n)) | ((val) >> (sizeof(usize) * 8 - (n))))
#define HM_ROTATE_RIGHT(val, n) (((val) >> (n)) | ((val) << (sizeof(usize) * 8 - (n))))

usize hm_fn_hash(const char* key) {
    assert(key);
    constexpr usize seed = 0x1234567890ABCDEF;
    usize hash = seed;
    while (*key) hash = HM_ROTATE_LEFT(hash, 9) + (u8)*key++;
    hash ^= seed;
    hash = (~hash) + (hash << 18);
    hash ^= hash ^ HM_ROTATE_RIGHT(hash, 31);
    hash = hash * 21;
    hash ^= hash ^ HM_ROTATE_RIGHT(hash, 11);
    hash += (hash << 6);
    hash ^= HM_ROTATE_RIGHT(hash, 22);
    return (hash + seed);
}

usize hm_fn_hash(usize key) {
    return key;
}

bool hm_fn_eq(const char* k1, const char* k2) {
    return strcmp(k1, k2) == 0;
}

bool hm_fn_eq(usize k1, usize k2) {
    return k1 == k2;
}

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

// Debug allocator implementation
#pragma region

#ifdef PS_MEM_DEBUG
struct _mem_entry {
    usize ptr;
    usize size;
    bool freed;
    bool reallocated;
    const char* file;
    int line;
    _mem_entry* next;
};

constexpr usize _mem_entry_size = 16;
static _mem_entry* _mem_allocations[_mem_entry_size] = {0};

void mem_check_dump() {
    for (int i = 0; i < _mem_entry_size; i++) {
        auto entry = _mem_allocations[i];
        while (entry) {
            const char* info = entry->freed ? (entry->reallocated ? "REALLOCATED" : "FREED") : "LEAKED";
            if (!entry->freed) printf("%s %llu bytes at %p (%s:%d)\n", info, entry->size, (void*)entry->ptr, entry->file, entry->line);
            auto tmp = entry;
            entry = entry->next;
            free(tmp);
        }
    }
}
#endif

void* _mem_alloc(usize size, const char* file, int line) {
    auto ptr = calloc(size, 1);
    assert(ptr);

#ifdef PS_MEM_DEBUG
    auto entry = _mem_allocations[(usize)ptr % _mem_entry_size];
    bool found = false;
    _mem_entry* prev = nullptr;
    while (entry) {
        prev = entry;
        if (entry->ptr == (usize)ptr) {
            found = true;
            if (!entry->freed) fprintf(stderr, "%s:%d: memory allocated at %p even though it wasn't free\n", file, line, ptr);
            entry->size = size;
            entry->freed = false;
            entry->file = file;
            entry->line = line;
            break;
        }
        entry = entry->next;
    }
    if (!found) {
        auto new_entry = (_mem_entry*)calloc(1, sizeof(_mem_entry));
        new_entry->ptr = (usize)ptr;
        new_entry->size = size;
        new_entry->freed = false;
        new_entry->file = file;
        new_entry->line = line;
        if (prev) {
            new_entry->next = prev->next;
            prev->next = new_entry;
        } else {
            _mem_allocations[(usize)ptr % _mem_entry_size] = new_entry;
        }
    }
#endif

    return ptr;
}

void _mem_free(void* ptr, const char* file, int line) {
    assert(ptr);

#ifdef PS_MEM_DEBUG
    bool found = false;
    auto entry = _mem_allocations[(usize)ptr % _mem_entry_size];
    while (entry) {
        if (entry->ptr == (usize)ptr) {
            found = true;
            if (entry->freed) fprintf(stderr, "%s:%d: freeing already freed memory at %p\n", file, line, ptr);
            entry->freed = true;
            break;
        }
        entry = entry->next;
    }
    if (!found) fprintf(stderr, "%s:%d: freeing memory before using at %p\n", file, line, ptr);
#endif

    free(ptr);
}

void* _mem_realloc(void* old_ptr, usize old_size, usize new_size, const char* file, int line) {
    if (!old_ptr) return _mem_alloc(new_size, file, line);

    auto new_ptr = realloc(old_ptr, new_size);
    assert(new_ptr);
    memset((u8*)new_ptr + old_size, 0, new_size - old_size);

#ifdef PS_MEM_DEBUG
    if (old_ptr == new_ptr) {  // We had enough memory in our current block
        auto ptr = old_ptr;
        bool found = false;
        auto entry = _mem_allocations[(usize)ptr % _mem_entry_size];
        _mem_entry* prev = nullptr;
        while (entry) {
            prev = entry;
            if (entry->ptr == (usize)ptr) {
                found = true;
                entry->size = new_size;
                if (entry->freed) fprintf(stderr, "%s:%d: freed pointer passed to realloc at %p\n", file, line, ptr);
                entry->freed = false;
                break;
            }
            entry = entry->next;
        }
        if (!found) fprintf(stderr, "%s:%d: couldn't find previous allocation while reallocating at %p\n", file, line, ptr);
    } else {  // We got new memory on the heap
        bool found = false;
        auto entry = _mem_allocations[(usize)old_ptr % _mem_entry_size];
        while (entry) {  // Let's remove previous address
            if (entry->ptr == (usize)old_ptr) {
                found = true;
                entry->freed = true;
                entry->reallocated = true;
                break;
            }
            entry = entry->next;
        }

        if (!found) {
            fprintf(stderr, "%s:%d: old pointer not found while reallocating %p -> %p\n", file, line, old_ptr, new_ptr);
        }

        entry = _mem_allocations[(usize)new_ptr % _mem_entry_size];
        auto prev = entry;
        while (entry) {
            prev = entry;
            if (entry->ptr == (usize)new_ptr) {
                if (!entry->freed) fprintf(stderr, "%s:%d: memory allocated at %p even though it wasn't free\n", file, line, new_ptr);
                break;
            }
            entry = entry->next;
        }

        entry = (_mem_entry*)calloc(1, sizeof(_mem_entry));
        entry->ptr = (usize)new_ptr;
        entry->size = new_size;
        entry->freed = false;
        entry->file = file;
        entry->line = line;
        entry->next = nullptr;
        if (prev) {
            entry->next = prev->next;
            prev->next = entry;
        } else {
            _mem_allocations[(usize)new_ptr % _mem_entry_size] = entry;
        }
    }
#endif

    return new_ptr;
}
#pragma endregion
