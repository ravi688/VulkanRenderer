#include <mesh_renderer.h>
#include <debug.h>

OnInstantiate(mesh_renderer_t,	
	BindOnDestroy(mesh_renderer_t); 
	BindOnAwake(mesh_renderer_t); 
	BindOnStart(mesh_renderer_t);
	BindOnPreRender(mesh_renderer_t);
	BindOnUpdate(mesh_renderer_t);
	log_msg("OnInstantiate\n");
)

OnDestroy(mesh_renderer_t,
	log_msg("OnDestroy\n");
)


OnAwake(mesh_renderer_t,
	log_msg("OnAwake\n");
)

OnStart(mesh_renderer_t,
	log_msg("OnStart\n");
)

OnUpdate(mesh_renderer_t, 
	// log_msg("OnUpdate\n");
)

OnPreRender(mesh_renderer_t, 
	log_msg("OnPreRender\n");
)
