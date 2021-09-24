#ifndef __SCENE_MANAGER_H__
#define __SCENE_MANAGER_H__

#include <engine/scene.h>
#include <buffer.h>
#include <stdlib.h>


typedef struct scene_manager_t scene_manager_t;


typedef struct scene_manager_t 
{
	LIST(scene_t*) active_scenes;
} scene_manager_t; 



scene_manager_t* scene_manager_init();
void scene_manager_destroy(scene_manager_t* scene_manager);
void scene_manager_add_active_scene(scene_manager_t* scene_manager, scene_t* scene);
LIST(scene_t*) scene_manager_get_active_scene_list(scene_manager_t* scene_manager); 
void scene_manager_remove_active_scene(scene_manager_t* scene_manager, scene_t* scene);

void scene_manager_for_each_objects_in_all_scenes(scene_manager_t* scene_manager, void (*function)(object_t*));
#endif/*__SCENE_MANAGER_H__*/