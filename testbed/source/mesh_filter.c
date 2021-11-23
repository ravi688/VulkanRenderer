#include <mesh_filter.h>
#include <renderer/debug.h>

OnInstantiate(mesh_filter_t,
	BindOnDestroy(mesh_filter_t);
	BindOnStart(mesh_filter_t); 
	BindOnAwake(mesh_filter_t);
	log_msg("OnInstantiate\n");
)

OnStart(mesh_filter_t,
	log_msg("OnStart\n");
)


OnAwake(mesh_filter_t,
	log_msg("OnAwake\n");
)

OnDestroy(mesh_filter_t,
	log_msg("OnDestroy\n");
)

