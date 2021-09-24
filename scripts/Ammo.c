#include "Ammo.h"

OnInstantiate(Ammo,
	BindOnAwake(Ammo);
	BindOnStart(Ammo); 
	BindOnDestroy(Ammo);
	puts("Ammo component is Instantiated");
	printf("Ammo component is Attached to Object \"%s\"\n", COMPONENT(this)->object->name);
)

OnDestroy(Ammo,

	puts("Ammo component is Destroyed");
)


OnAwake(Ammo,

	puts("Ammo awake is called");
)

OnStart(Ammo,

	puts("Ammo start is called");
)
