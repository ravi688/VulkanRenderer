#include <renderer/hash_function.h>
#include <string.h>

RENDERER_API hash_t string_hash(void* v)
{
	const char* str = DEREF_TO(const char*, v);
	u32 _dbg_hash = strlen(str);
	_debug_assert__(_dbg_hash < U16_MAX);
	u16 hash1 = CAST_TO(u16, _dbg_hash);
	u16 hash2 = 0;
	u32 hash3 = 0;
	for(u16 i = 0; i < hash1; i++)
	{
		hash2 ^= CAST_TO(u16, str[i]);
		hash3 += str[i];
	}
	return CAST_TO(hash_t, BIT64_PACK32(BIT32_PACK16(hash1, hash2), hash3));
}

RENDERER_API hash_t ptr_hash(void* v)
{
	return CAST_TO(hash_t, CAST_TO(u64, DREF_TO(char* const, v)));
}

RENDERER_API hash_t s8_hash(void* v)
{
	return CAST_TO(hash_t, CAST_TO(u64, DREF_TO(s8, v)));
}

RENDERER_API hash_t s16_hash(void* v)
{
	return CAST_TO(hash_t, CAST_TO(u64, DREF_TO(s16, v)));
}

RENDERER_API hash_t s32_hash(void* v)
{
	return CAST_TO(hash_t, CAST_TO(u64, DREF_TO(s32, v)));
}

RENDERER_API hash_t s64_hash(void* v)
{
	return CAST_TO(hash_t, CAST_TO(u64, DREF_TO(s64, v)));
}

RENDERER_API hash_t u8_hash(void* v)
{
	return CAST_TO(hash_t, CAST_TO(u64, DREF_TO(u8, v)));
}

RENDERER_API hash_t u16_hash(void* v)
{
	return CAST_TO(hash_t, CAST_TO(u64, DREF_TO(u16, v)));
}

RENDERER_API hash_t u32_hash(void* v)
{
	return CAST_TO(hash_t, CAST_TO(u64, DREF_TO(u32, v)));
}

RENDERER_API hash_t u64_hash(void* v)
{
	return CAST_TO(hash_t, DREF_TO(u64, v));
}

RENDERER_API hash_t float_hash(void* v)
{
	debug_log_warning("You are trying to calculate hash of a float value %f, which is prone to miss calculation", DREF_TO(f32, v));
	return CAST_TO(hash_t, REINTERPRET_TO(u32, DREF_TO(f32, v)));
}

RENDERER_API hash_t double_hash(void* v)
{
	debug_log_warning("You are trying to calculate hash of a double value %f, which is prone to miss calculation", DREF_TO(f64, v));
	return CAST_TO(hash_t, REINTERPRET_TO(u64, DREF_TO(f64, v)));
}

