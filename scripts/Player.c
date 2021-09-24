#include "Player.h"

OnInstantiate(Player,
	BindOnAwake(Player);
	BindOnStart(Player); 
	BindOnDestroy(Player);

	this->walk_speed = 100.0f; 
	this->run_speed = 50.0f; 
	this->move_speed = 70.0f; 
	this->jump_height = 100.0f; 
	this->crouch_angle = 23.0f;
	this->crouch_speed = 12.0f;

	puts("Player component is Instantiated");
	printf("Player component is Attached to Object \"%s\"\n", COMPONENT(this)->object->name);
)

OnDestroy(Player,

	puts("Player component is Destroyed");
)


OnAwake(Player,

	puts("Player awake is called");
)

OnStart(Player,

	puts("Player start is called");
)
