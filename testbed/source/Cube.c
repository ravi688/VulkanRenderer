#include <Cube.h>
#include <mesh_filter.h>
#include <mesh_renderer.h>

#include <ecs/object.h>

#include <stdio.h>

OnInstantiate(Cube, 

	BindOnStart(Cube);
	BindOnUpdate(Cube);
	BindOnDestroy(Cube);
	this->width = 0;
	this->height = 0;
	this->depth = 0;
	puts("Cube component is instantiated");
)

OnStart(Cube,
	// object_attach_component(COMPONENT(this)->object, mesh_filter_t);
	// object_attach_component(COMPONENT(this)->object, mesh_renderer_t);
	puts("Cube component's start is called");
)

OnUpdate(Cube,
	puts("Hello World from Cube Component");
)

OnDestroy(Cube,
	puts("Cube Component is destoryed");
)