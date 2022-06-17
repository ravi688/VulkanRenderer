
#include <renderer/defines.h>

#include <bufferlib/buffer.h>

typedef buf_ucount_t library_slot_handle_t;
#define LIBRARY_SLOT_HANDLE_INVALID (~0ULL)

typedef struct library_slot_t
{
	void* object;						// satellite data
	library_slot_handle_t handle; 		// handle to this slot
} library_slot_t;

typedef struct library_t
{
	BUFFER slots;
	BUFFER relocation_table;
} library_t;

BEGIN_CPP_COMPATIBLE

RENDERER_API library_t* library_new();
RENDERER_API library_t* library_create();
RENDERER_API void library_create_no_alloc(library_t OUT lib);
RENDERER_API void library_destroy(library_t* lib);
RENDERER_API void library_release_resources(library_t* lib);

RENDERER_API library_slot_handle_t library_create_slot(library_t* lib, void* object);
RENDERER_API void* library_getH(library_t* lib, library_slot_handle_t handle);
RENDERER_API void library_destroy_slotH(library_t* lib, library_slot_handle_t handle);

END_CPP_COMPATIBLE

