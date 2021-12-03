#include <Cube.h>
#include <mesh_filter.h>
#include <mesh_renderer.h>

#include <renderer/debug.h>
#include <ecs/object.h>


OnInstantiate(Cube, 

	BindOnStart(Cube);
	BindOnUpdate(Cube);
	BindOnDestroy(Cube);
	this->width = 0;
	this->height = 0;
	this->depth = 0;
	log_msg("OnInstantiate\n");
)

OnStart(Cube,
	object_attach_component(COMPONENT(this)->object, mesh_filter_t);
	object_attach_component(COMPONENT(this)->object, mesh_renderer_t);
	log_msg("OnStart\n");
)

OnUpdate(Cube,
	// log_msg("OnUpdate\n");
)

OnDestroy(Cube,
	log_msg("OnDestroy\n");
)