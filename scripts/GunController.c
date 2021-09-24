#include "GunController.h"

OnInstantiate(GunController,
	BindOnAwake(GunController);
	BindOnStart(GunController); 
	BindOnDestroy(GunController);
	puts("GunController component is Instantiated");
	printf("GunController component is Attached to Object \"%s\"\n", COMPONENT(this)->object->name);
)

OnDestroy(GunController,

	puts("GunController component is Destroyed");
)


OnAwake(GunController,

	puts("GunController awake is called");
)

OnStart(GunController,

	puts("GunController start is called");
)
