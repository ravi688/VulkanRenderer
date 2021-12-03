

#include "TestBehaviour.h"
#include <ecs/object.h>
#include <stdio.h>

OnInstantiate(TestBehaviour,

	BindOnStart(TestBehaviour);
	BindOnDestroy(TestBehaviour);
	BindOnUpdate(TestBehaviour);
	puts("TestBehaviour is instantiated");
)


OnStart(TestBehaviour,
	puts("TestBehaviour::OnStart is called");
)


OnDestroy(TestBehaviour,
	puts("TestBehaviour::OnUpdate is callled");
)

OnUpdate(TestBehaviour, 

)