

#include <engine/scene_manager.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

static bool comparer(void* ptr1, void* ptr2)
{
	return ((*(scene_t**)ptr1) == (*(scene_t**)ptr2));
}

scene_manager_t* scene_manager = NULL;

scene_manager_t* scene_manager_init()
{
	scene_manager_t* scene_manager = (scene_manager_t*)malloc(sizeof(scene_manager_t)); 
	scene_manager->active_scenes = LIST_NEW(void*, 0);
	return scene_manager;
}

void scene_manager_for_each_objects_in_all_scenes(scene_manager_t* scene_manager, void (*function)(object_t*))
{
	if(scene_manager->active_scenes != NULL)
	{
		LIST() list  = LIST_GET_BINDED();
		LIST_BIND(scene_manager->active_scenes);
		uint32_t scene_count = LIST_GET_COUNT();
		uint32_t object_count;
		uint32_t component_count;
		scene_t* scene;
		for(int i = 0; i < scene_count; i++)
		{
			LIST_GET(i, &scene);
			if(scene->objects != NULL)
			{
				
				LIST_BIND(scene->objects);
				for(int j = 0; j <  LIST_GET_COUNT(); j++)
				{
					object_t* object = (*LIST_GET_PTR(object_t*, j));
					function(object);
					LIST_BIND(scene->objects);
				}
				LIST_BIND(scene_manager->active_scenes);
			}
		}
		LIST_BIND(list);	
	}
}

LIST(scene_t*) scene_manager_get_active_scene_list(scene_manager_t* scene_manager) { return scene_manager->active_scenes; }

void scene_manager_destroy(scene_manager_t* scene_manager)
{
	if(scene_manager == NULL)
		return;
	if(scene_manager->active_scenes != NULL)
	{
		LIST() list = LIST_GET_BINDED();
		LIST_BIND(scene_manager->active_scenes);
		LIST_DESTROY();
		LIST_BIND(list);
		scene_manager->active_scenes = NULL;
	}
	free(scene_manager);
	scene_manager = NULL;
}

void scene_manager_add_active_scene(scene_manager_t* scene_manager, scene_t* scene)
{
	if(scene_manager == NULL)
	{
		printf("[Error]: scene_manager_t.AddActiveScene(scene_t*) Error, scene_manager_t is not initialized\n");
		return;
	}
	if(scene_manager->active_scenes != NULL)
	{
		if(scene == NULL)
		{	printf("[Error]: scene_manager_t.AddActiveScene(scene_t*) Error, Passed scene_t* scene is NULL\n"); return; }
		LIST() list = LIST_GET_BINDED();
		LIST_BIND(scene_manager->active_scenes);
		LIST_ADD(&scene);
		LIST_BIND(list);
	}
	else 
		printf("[Error]: scene_manager_t.AddActiveScene(scene_t*) Error, scene_manager->active_scens is NULL\n");
}

void scene_manager_remove_active_scene(scene_manager_t* scene_manager, scene_t* scene)
{
	if(scene_manager == NULL)
	{
		printf("[Error]: scene_manager_t.RemoveActiveScene(scene_t*) Error, scene_manager_t is not initialized\n");
		return;
	}
	if(scene_manager->active_scenes != NULL)
	{
		if(scene == NULL)
		{	printf("[Error]: scene_manager_t.RemoveActiveScene(scene_t*) Error, Passed scene_t* scene is NULL\n"); return; }
		LIST() list = LIST_GET_BINDED();
		LIST_BIND(scene_manager->active_scenes);
		if(!LIST_REMOVE(&scene, comparer))
			printf("[Warning]: scene_manager_t.RemoveActiveScene(scene_t*) Warning, Passed scene_t* is not added active scene\n");
		LIST_BIND(list);
	}
	else 
		printf("[Error]: scene_manager_t.RemoveActiveScene(scene_t*) Error, scene_manager->active_scens is NULL\n");	
}