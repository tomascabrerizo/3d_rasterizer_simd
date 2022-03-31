#ifndef TC_GUI_H
#define TC_GUI_H

#include "core/types.h"

// MurmurHash2, 64-bit versions, by Austin Appleb
u64 tc_hash_64(const void *key, int len, unsigned int seed)
{
	const u64 m = 0xc6a4a7935bd1e995;
	const int r = 47;

	u64 h = seed ^ (len * m);

	const u64 * data = (const u64 *)key;
	const u64 * end = data + (len/8);

	while(data != end)
	{
		u64 k = *data++;

		k *= m; 
		k ^= k >> r; 
		k *= m; 
		
		h ^= k;
		h *= m; 
	}

	const unsigned char * data2 = (const unsigned char*)data;

	switch(len & 7)
	{
	case 7: h ^= uint64_t(data2[6]) << 48;
	case 6: h ^= uint64_t(data2[5]) << 40;
	case 5: h ^= uint64_t(data2[4]) << 32;
	case 4: h ^= uint64_t(data2[3]) << 24;
	case 3: h ^= uint64_t(data2[2]) << 16;
	case 2: h ^= uint64_t(data2[1]) << 8;
	case 1: h ^= uint64_t(data2[0]);
	        h *= m;
	};
 
	h ^= h >> r;
	h *= m;
	h ^= h >> r;

	return h;
}

#define TC_HASH_UNUSED 0xFFFFFFFFFFFFFFFFULL 
#define TC_HASH_TOMBSTONE 0xFFFFFFFFFFFFFFFeULL

#define tc_Hash(K, V) {     \
    u32 num_buckets;        \
    u32 used_buckets;       \
    K *keys;                \
    V *values;              \
}

struct tc_HashU64 tc_Hash(u64, u64);

static inline u32 tc_hash_first_index(u64 key, u32 num_buckets)
{
    return key & (num_buckets - 1);
}

static inline s32 tc_hash__add_no_grow(u64 *keys, u32 num_buckets, u64 key)
{
    u32 max_distance = num_buckets;
    if(!num_buckets)
    {
        return -1;
    }
    u32 i = tc_hash_first_index(key, num_buckets);
    u32 distance = 0;
    while(keys[i] != TC_HASH_UNUSED && keys[i] != TC_HASH_TOMBSTONE)
    {
        if(distance > max_distance)
        {
            return -1;
        }
        i = (i + 1) & (num_buckets - 1);
        ++distance;
    }
    return (s32)i;
}

static inline void tc_hash__grow_to(u64 **keys_ptr, void **values_ptr, u64 value_bytes, u32 *num_buckets_ptr, u32 *used_buckets_ptr, u32 new_buckets)
{
    u64 *keys = *keys_ptr;
    void *values = values_ptr ? *values_ptr : 0;
    u32 num_buckets = *num_buckets_ptr;
    if(num_buckets >= new_buckets)
    {
        return;
    }
    u64 bytes_to_alloc = new_buckets * (sizeof(*keys) + value_bytes) + value_bytes;
    u64 *new_keys = (u64 *)malloc(bytes_to_alloc);
    void *default_value_ptr = new_keys + new_buckets;
    void *new_values = (u8 *)default_value_ptr + value_bytes;
    memset(new_keys, 0xFF, new_buckets*sizeof(*keys));
    u32 new_used = 0;
    
    for(u32 i = 0; i < num_buckets; ++i)
    {
        if(keys[i] != TC_HASH_UNUSED && keys[i] != TC_HASH_TOMBSTONE)
        {
            s32 new_i = tc_hash__add_no_grow(new_keys, num_buckets, keys[i]);
            new_keys[new_i] = keys[i];
            if(value_bytes)
            {
                memcpy((u8 *)new_values + new_i * value_bytes, (u8 *)values + i * value_bytes, value_bytes);
            }
            ++new_used;
        }
    }
    // TODO: tc_hash__free(keys_ptr, num_buckets_ptr, values_ptr, value_bytes);
    *used_buckets_ptr = new_used;
    *num_buckets_ptr = new_buckets;
    *keys_ptr = new_keys;
    if(values_ptr)
    {
        *values_ptr = new_values; 
    }
}

static inline void tc_hash__grow(u64 **keys_ptr, void **values_ptr, u64 value_bytes, u32 *num_buckets_ptr, u32 *used_buckets_ptr)
{
    u32 num_buckets = *num_buckets_ptr;
    u32 used_buckets = *used_buckets_ptr;

    u32 new_buckets = num_buckets ? num_buckets : 16;
    while((f32)used_buckets / (f32)new_buckets > 0.5f)
    {
        new_buckets *= 2;
    }
    tc_hash__grow_to(keys_ptr, values_ptr, value_bytes, num_buckets_ptr, used_buckets_ptr, new_buckets);
}

static inline u32 tc_hash__add(u64 **keys_ptr, void **values_ptr, u64 value_bytes, u32 *num_buckets_ptr, u32 *used_buckets_ptr, u64 key)
{
    if(!*num_buckets_ptr || ((f32)*used_buckets_ptr / (f32)*num_buckets_ptr) > 0.7f)
    {
        tc_hash__grow(keys_ptr, values_ptr, value_bytes, num_buckets_ptr, used_buckets_ptr);
    }
}

#endif // TC_GUI_H
