#include "Animator.h"

OnInstantiate(Animator,
	BindOnAwake(Animator);
	BindOnStart(Animator); 
	BindOnDestroy(Animator);
	puts("Animator component is Instantiated");
	printf("Animator component is Attached to Object \"%s\"\n", COMPONENT(this)->object->name);
)

OnDestroy(Animator,

	puts("Animator component is Destroyed");
)


OnAwake(Animator,

	puts("Animator awake is called");
)

OnStart(Animator,

	puts("Animator start is called");
)
